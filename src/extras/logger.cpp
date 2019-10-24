//
// Created by lamanchy on 22.10.2018.
//
#include "logger.h"

Logger &Logger::getLogger() {
  static Logger instance;
  return instance;
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
void Logger::fatal(std::string const &message) {
  std::lock_guard<std::mutex> lg(lock);
  do_log(std::cerr, "FATAL", message);
  do_log(info_file_, "FATAL", message);
  do_log(trace_file_, "FATAL", message);
  exit(1);
}

void Logger::do_log(std::ostream &stream, const std::string &level, std::string const &message) {
  if (stream)
    stream << date::format("%F %T", std::chrono::system_clock::now()) << " " << level << " " << message << std::endl;
}

void Logger::set_logging_dir(const std::string& path) {
  info_file_ = std::ofstream(path + "info.log", std::ofstream::out | std::ofstream::app);
  trace_file_ = std::ofstream(path + "trace.log", std::ofstream::out | std::ofstream::app);
  if (not info_file_ or not trace_file_) {
    std::cerr << "Couldn't create log files!" << std::endl;
    exit(1);
  }
  logging_dir = path;
}