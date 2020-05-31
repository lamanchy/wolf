
#include <extras/logger.h>

thread_local bool BaseLogger::empty = true;

BaseLogger &BaseLogger::getLogger() {
  static BaseLogger instance;
  return instance;
}

void BaseLogger::set_logging_dir(const std::string &path) {
  std::lock_guard<std::mutex> lg(lock);
  logging_dir = path;
  setup_files();
}

void BaseLogger::check_file_rotation() {
  if (current_date != wolf::extras::get_date()) {
    std::lock_guard<std::mutex> lg(lock);
    //close
    info_file_.close();
    trace_file_.close();

    // move
    std::rename(get_file_path("info.log").c_str(),
                get_file_path("info.log." + current_date).c_str());
    std::rename(get_file_path("trace.log").c_str(),
                get_file_path("trace.log." + current_date).c_str());

    // create new
    setup_files();

    // delete oldest
    auto old = wolf::extras::get_date(std::chrono::hours(-24 * 30));
    std::remove(get_file_path("info.log." + old).c_str());
    std::remove(get_file_path("trace.log." + old).c_str());

    current_date = wolf::extras::get_date();
  }
}
std::string BaseLogger::get_file_path(const std::string &name) {
  return logging_dir + name;
}
void BaseLogger::setup_files() {
  info_file_ = std::ofstream(get_file_path("info.log"), std::ofstream::out | std::ofstream::app);
  trace_file_ = std::ofstream(get_file_path("trace.log"), std::ofstream::out | std::ofstream::app);
  if (not info_file_ or not trace_file_) {
    std::cerr << "Couldn't create log files!" << std::endl;
  }
}
