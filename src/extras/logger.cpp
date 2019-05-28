//
// Created by lamanchy on 22.10.2018.
//
#include "logger.h"

std::atomic<bool> Logger::initialized{false};
Logger &Logger::getLogger(std::string log_dir) {
  if (not initialized) {
    if (log_dir.empty())
      log_dir = wolf::extras::get_executable_dir();

    setupLogger(log_dir);
  }

  static Logger instance(logging_dir);
  return instance;
}
void Logger::setupLogger(const std::string &ld) {
  logging_dir = ld;
  initialized = true;
}
void Logger::trace(std::string const &message) {
  std::lock_guard<std::mutex> lg(lock);
  do_log(std::cout, "TRACE", message);
  do_log(trace_file_, "TRACE", message);
}
void Logger::debug(std::string const &message) {
  std::lock_guard<std::mutex> lg(lock);
  do_log(std::cout, "DEBUG", message);
  do_log(trace_file_, "DEBUG", message);
}
void Logger::info(std::string const &message) {
  std::lock_guard<std::mutex> lg(lock);
  do_log(std::cout, "INFO ", message);
  do_log(info_file_, "INFO ", message);
  do_log(trace_file_, "INFO ", message);
}
void Logger::warn(std::string const &message) {
  std::lock_guard<std::mutex> lg(lock);
  do_log(std::cout, "WARN ", message);
  do_log(info_file_, "WARN ", message);
  do_log(trace_file_, "WARN ", message);
}
void Logger::error(std::string const &message) {
  std::lock_guard<std::mutex> lg(lock);
  do_log(std::cerr, "ERROR", message);
  do_log(info_file_, "ERROR", message);
  do_log(trace_file_, "ERROR", message);
}
Logger::Logger(const std::string &log_dir)
    : info_file_(log_dir + "wolf.log", std::ofstream::out | std::ofstream::app),
      trace_file_(log_dir + "trace.log", std::ofstream::out | std::ofstream::app) {
  if (not info_file_ or not trace_file_) {
    std::cerr << "Couldn't create log files!" << std::endl;
    exit(1);
  }
}
void Logger::do_log(std::ostream &stream, const std::string &level, std::string const &message) {
  stream << date::format("%F %T", std::chrono::system_clock::now()) << " " << level << " " << message << std::endl;
}
std::string Logger::logging_dir;