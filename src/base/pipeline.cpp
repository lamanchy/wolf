#include "pipeline.h"

namespace wolf {

std::atomic<int> pipeline::interrupt_received{0};
std::string pipeline::config_dir;

bool pipeline::initialized{false};

pipeline::pipeline(int argc, char **argv) :
    opts(argc, argv),
    logger(Logger::getLogger(
        opts.option<command<std::string>>("logging_dir", "Path to logs", "", wolf::extras::get_executable_dir()
        )->get_value())) {
  if (initialized) {
    logger.error("Cannot create two pipelines.");
    exit(0);
  }
  initialized = true;

  bool persistent = this->option<command<bool>>("persistent", "Can pipeline store events on disk?")->get_value();

  plugin::persistent = persistent;
  if (persistent) plugin::buffer_size = 1024;
  else plugin::buffer_size = 128;

  initialize();
}
void pipeline::run() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  std::signal(SIGBREAK, catch_signal);
#endif
  std::signal(SIGINT, catch_signal);
  evaluate_options();
  logger.info("Starting pipeline");
  start();
  logger.info("Pipeline started");
  wait();
  stop();
  logger.info("Pipeline stopped");
}
void pipeline::initialize() {
  std::string path = extras::get_executable_dir();

  config_dir = this->option<command<std::string>>("config_dir", "Path to configs", "", path)->get_value();

  logger.info("Configuring STXXL");

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  _putenv_s("STXXLLOGFILE", (Logger::logging_dir + "stxxl.log").c_str());
  _putenv_s("STXXLERRLOGFILE", (Logger::logging_dir + "stxxl.errlog").c_str());
#else
  setenv("STXXLLOGFILE", (Logger::logging_dir + "stxxl.log").c_str(), 1);
  setenv("STXXLERRLOGFILE", (Logger::logging_dir + "stxxl.errlog").c_str(), 1);
#endif

  stxxl::config *cfg = stxxl::config::get_instance();
  // create a disk_config structure.
  std::string fileio = path[path.size() - 1] == '/' ? "syscall" : "wincall";
  stxxl::disk_config disk(path + "queue.tmp", 0, fileio);
  disk.autogrow = true;
  disk.unlink_on_open = true;
  disk.delete_on_exit = true;
  disk.direct = stxxl::disk_config::DIRECT_TRY;
  cfg->add_disk(disk);

  logger.info("Pipeline initialized");
}
template<typename T>
std::vector<T> pipeline::for_each_plugin(const std::function<T(plugin &)> &function) {
  // TODO this must be done better, yuck
  std::set<plugin::id_type> visited;
  std::queue<pointer> to_process;
  std::queue<pointer> to_do;
  std::for_each(plugins.begin(), plugins.end(), [&to_process](pointer &ptr) { to_process.push(ptr); });
  std::vector<T> results;

  while (!to_process.empty()) {
    pointer &ptr = to_process.front();
    auto it = visited.find(ptr->id);
    if (it == visited.end()) {
      visited.insert(ptr->id);
      to_do.push(ptr);
      for (auto &pair : ptr->outputs) to_process.push(pair.second);
    }
    to_process.pop();
  }
  while (not to_do.empty()) {
    results.push_back(function(*to_do.front()));
    to_do.pop();
  }
  return results;
}
void pipeline::for_each_plugin(const std::function<void(plugin &)> &function) {
  const std::function<int(plugin &)> fn([&function](plugin &p) {
    function(p);
    return 0;
  });
  for_each_plugin<int>(fn);
}
void pipeline::process() {
  plugin::is_thread_processor = true;
  while (plugins_running()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    for_each_plugin([](plugin &p) { while (p.process_buffer()) {}; });
//      if (not std::all_of(res.begin(), res.end(), [](bool r) { return r; }))
    // TODO build up sleep time
//      std::this_thread::yield();
  }
  std::for_each(plugins.begin(), plugins.end(), [this](pointer &) {
    for_each_plugin([](plugin &p) { while (p.process_buffer()) {}; });
  });

  logger.info("ending processor");
}
void pipeline::catch_signal(int signal) {
  ++interrupt_received;

  if (interrupt_received >= 2) {
    Logger::getLogger().error("Second interrup signal received, suicide now.");
    std::quick_exit(EXIT_FAILURE);
  }

  Logger::getLogger().info("interrupt signal received, stopping wolf");
}
void pipeline::start() {
  for_each_plugin([](plugin &p) { p.start(); });

  for (unsigned i = 0; i < number_of_processors; ++i) {
    processors.emplace_back(&pipeline::process, this);
  }
}
void pipeline::wait() {
  while (plugins_running()) {
    if (interrupt_received) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}
bool pipeline::plugins_running() {
  return std::any_of(plugins.begin(), plugins.end(), [](pointer &p) { return p->is_running(); });
}
void pipeline::stop() {
  for_each_plugin([](plugin &p) { p.stop(); });
  std::for_each(processors.begin(), processors.end(), [](std::thread &thread) { thread.join(); });
}

}