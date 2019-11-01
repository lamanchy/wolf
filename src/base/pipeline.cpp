#include "pipeline.h"
#include "sleeper.h"

namespace wolf {

std::atomic<int> pipeline::interrupt_received{0};

bool pipeline::initialized{false};

pipeline::pipeline(options _opts) :
    opts(std::move(_opts)) {
  if (initialized) {
    logger.error("Pipeline already initialized, cannot create two pipelines.");
    exit(0);
  }
  initialized = true;

  evaluate_options();
  if (is_persistent())
    setup_persistency();

  logger.info("Pipeline initialized");

  // drop is just an empty plugin, condition is never true,
  // however, without this line linker fails, I already spent
  // too much time trying to fix that, so...
  if (wolf::extras::get_separator().empty()) drop();
}

void pipeline::run() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  std::signal(SIGBREAK, catch_signal);
#endif
  std::signal(SIGINT, catch_signal);
  logger.info("Starting pipeline");
  start();
  logger.info("Pipeline started");
  wait();
  logger.info("Stopping pipeline");
  stop();
  logger.info("Pipeline stopped");
}

void pipeline::setup_persistency() {
  std::string path = extras::get_executable_dir();

  logger.info("Configuring STXXL");

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  _putenv_s("STXXLLOGFILE", (logger.get_logging_dir() + "stxxl.log").c_str());
  _putenv_s("STXXLERRLOGFILE", (logger.get_logging_dir() + "stxxl.errlog").c_str());
#else
  setenv("STXXLLOGFILE", (logger.get_logging_dir() + "stxxl.log").c_str(), 1);
  setenv("STXXLERRLOGFILE", (logger.get_logging_dir() + "stxxl.errlog").c_str(), 1);
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

}

template<typename T>
std::vector<T> pipeline::for_each_plugin(const std::function<T(base_plugin &)> &function) {
  // TODO this must be done better, yuck
  std::set<base_plugin::id_type> visited;
  std::queue<plugin> to_process;
  std::queue<plugin> to_do;
  std::for_each(plugins.begin(), plugins.end(), [&to_process](plugin &ptr) { to_process.push(ptr); });
  std::vector<T> results;

  while (!to_process.empty()) {
    plugin &ptr = to_process.front();
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
void pipeline::for_each_plugin(const std::function<void(base_plugin &)> &function) {
  const std::function<int(base_plugin &)> fn([&function](base_plugin &p) {
    function(p);
    return 0;
  });
  for_each_plugin<int>(fn);
}
void pipeline::process() {
  base_plugin::is_thread_processor = true;
  sleeper sleeper;
  while (plugins_running()) {
    sleeper.sleep();
    for_each_plugin([&sleeper](base_plugin &p) { while (p.process_buffer()) { sleeper.decrease(); }});
  }
  std::for_each(plugins.begin(), plugins.end(), [this](plugin &) {
    for_each_plugin([](base_plugin &p) { while (p.process_buffer()) {}; });
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
  for_each_plugin([](base_plugin &p) { p.start(); });

  for (unsigned i = 0; i < number_of_processors; ++i) {
    processors.emplace_back(&pipeline::process, this);
  }
}

void pipeline::wait() {
  while (plugins_running()) {
    if (interrupt_received) {
      break;
    }
    // TODO wake up with conditional variable
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

bool pipeline::plugins_running() {
  return std::any_of(plugins.begin(), plugins.end(), [](plugin &p) { return p->is_running(); });
}

void pipeline::stop() {
  for_each_plugin([](base_plugin &p) { p.stop(); });
  std::for_each(processors.begin(), processors.end(), [](std::thread &thread) { thread.join(); });
}

void pipeline::evaluate_options() {
  std::string path = extras::get_executable_dir();

  auto config_config = opts.add_named
      <command<std::string>>(options::general_config_group_name, "c,config_dir", "Path to configs", path);
  auto logging_config = opts.add_named
      <command<std::string>>(options::general_config_group_name, "l,logging_dir", "Path to logs", path);
  auto persistent_config = opts.add_named
      <command<bool>>(options::general_config_group_name, "p,persistent", "Can pipeline store events on disk?");
  auto buffer_size_config = opts.add_named
      <command<unsigned>>(options::general_config_group_name, "b,buffer_size",
                          "How much events fits into buffer? Affects RAM usage as well as performance", "4096");
  auto thread_processors_config = opts.add_named
      <command<unsigned>>(options::general_config_group_name, "t,threads",
                          "How many threads wolf uses to process events",
                          std::to_string(std::thread::hardware_concurrency()));

  opts.parse_options();
  opts.print_options();

  config_dir = config_config->value();
  logger.set_logging_dir(logging_config->value());
  queue::persistent = persistent_config->value();
  queue::buffer_size = buffer_size_config->value();
  number_of_processors = thread_processors_config->value();
}

}