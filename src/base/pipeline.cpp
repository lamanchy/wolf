#include <base/pipeline.h>

namespace wolf {

std::atomic<int> pipeline::interrupt_received{0};

pipeline::pipeline(options _opts) :
    opts(std::move(_opts)) {
  if (pipeline_status::initialized) {
    logger.error << "Pipeline already initialized, cannot create two pipelines." << std::endl;
    exit(0);
  }

  evaluate_options();
  if (pipeline_status::persistent)
    setup_persistency();

  logger.info << "Pipeline initialized" << std::endl;

  pipeline_status::initialized = true;
}

void pipeline::run() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  std::signal(SIGBREAK, catch_signal);
#endif
  std::signal(SIGINT, catch_signal);
  logger.info << "Starting pipeline" << std::endl;
  start();
  logger.info << "Pipeline started" << std::endl;
  wait();
  logger.info << "Stopping pipeline" << std::endl;
  stop();
  logger.info << "Pipeline stopped" << std::endl;
}

void pipeline::setup_persistency() {
  std::string path = extras::get_executable_dir();

  logger.info << "Configuring STXXL" << std::endl;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  _putenv_s("STXXLLOGFILE", (logging::logger::get_logging_dir() + "stxxl.log").c_str());
  _putenv_s("STXXLERRLOGFILE", (logging::logger::get_logging_dir() + "stxxl.errlog").c_str());
#else
  setenv("STXXLLOGFILE", (logging::logger::get_logging_dir() + "stxxl.log").c_str(), 1);
  setenv("STXXLERRLOGFILE", (logging::logger::get_logging_dir() + "stxxl.errlog").c_str(), 1);
#endif

  stxxl::config *cfg = stxxl::config::get_instance();
  // create a disk_config structure.
  std::string fileio = path[path.size() - 1] == '/' ? "syscall" : "wincall";
  stxxl::disk_config disk(path + "queue.tmp", 0, fileio);
  disk.autogrow = true;
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
void pipeline::process(int i) {
  base_plugin::is_thread_processor = true;
  auto &sleeper = processors_sleepers[i];
  while (pipeline_status::is_running()) {
    for_each_plugin([&sleeper](base_plugin &p) { while (p.process_buffer()) { sleeper.decrease_sleep_time(); }});
    sleeper.increasing_sleep();
  }
}
void pipeline::catch_signal(int signal) {
  ++interrupt_received;

  if (interrupt_received >= 2) {
    logging::logger("pipeline").error << "Second interrup signal received, suiciding now." << std::endl;
    std::quick_exit(EXIT_FAILURE);
  }

  logging::logger("pipeline").info << "interrupt signal received, stopping wolf" << std::endl;
  pipeline_status::pipeline_sleeper.wake_up();
}

void pipeline::start() {
  pipeline_status::running = true;
  pipeline_status::started = true;
  for_each_plugin([](base_plugin &p) { p.do_start(); });

  processors_sleepers = std::unique_ptr<sleeper[]>(new sleeper[number_of_processors]);
  for (unsigned i = 0; i < number_of_processors; ++i)
    processors_sleepers[i].wake_up();

  for (unsigned i = 0; i < number_of_processors; ++i)
    processors.emplace_back(&pipeline::process, this, i);

  if (is_test) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ++interrupt_received;
  }
}

void pipeline::wait() {
  while (plugins_running()) {
    if (interrupt_received) {
      break;
    }
    pipeline_status::pipeline_sleeper.sleep_for(std::chrono::minutes(1));
    logging::logger::check_file_rotation();
  }
}

bool pipeline::plugins_running() {
  return std::any_of(plugins.begin(), plugins.end(), [](plugin &p) { return p->is_running(); });
}

void pipeline::stop() {
  std::for_each(plugins.begin(), plugins.end(), [](plugin &p) { p->do_stop(); });

  pipeline_status::running = false;
  for (unsigned i = 0; i < number_of_processors; ++i) processors_sleepers[i].wake_up();
  std::for_each(processors.begin(), processors.end(), [](std::thread &thread) { thread.join(); });
}

void pipeline::evaluate_options() {
  std::string path = extras::get_executable_dir();

  auto config_config = opts.add_grouped
      <input<std::string>>(options::general_config_group_name, "c,config_dir", "Path to configs", path);
  auto logging_config = opts.add_grouped
      <input<std::string>>(options::general_config_group_name, "l,logging_dir", "Path to logs", path);
  auto persistent_config = opts.add_grouped
      <input<bool>>(options::general_config_group_name, "p,persistent", "Can pipeline store events on disk?");
  auto buffer_size_config = opts.add_grouped
      <input<unsigned>>(options::general_config_group_name, "b,buffer_size",
                        "How much events fits into buffer? Affects RAM usage as well as performance", "4096");
  auto thread_processors_config = opts.add_grouped
      <input<unsigned>>(options::general_config_group_name, "t,threads",
                        "How many threads wolf uses to process events",
                        std::to_string(std::thread::hardware_concurrency()));
  auto is_test_config = opts.add_grouped
      <input<bool>>(options::general_config_group_name, "test", "Run Wolf and exit automatically after 1 second");

  opts.parse_options();
  opts.print_options();

  config_dir = config_config->value();
  logging::logger::set_logging_dir(logging_config->value());
  pipeline_status::persistent = persistent_config->value();
  pipeline_status::buffer_size = buffer_size_config->value();
  number_of_processors = thread_processors_config->value();
  is_test = is_test_config->value();
}

}