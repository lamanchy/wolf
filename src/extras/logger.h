//
// Created by lamanchy on 22.10.2018.
//

#ifndef WOLF_LOGGER_H
#define WOLF_LOGGER_H

#include <iso646.h>
#include <string>
#include <iostream>
#include <fstream>
#include <atomic>
#include <chrono>
#include <date/date.h>
#include <mutex>
#include <extras/get_executable_path.h>

class Logger {
 public:
  static Logger &getLogger(std::string log_dir = "") {
    if (not initialized) {
      if (log_dir.empty())
        log_dir = wolf::extras::get_executable_dir();

      setupLogger(log_dir);
    }

    static Logger instance(logging_dir);
    return instance;
  }

  static void setupLogger(const std::string &ld) {
    logging_dir = ld;
    initialized = true;
  }

  void trace(std::string const &message) {
    std::lock_guard<std::mutex> lg(lock);
    do_log(std::cout, "TRACE", message);
    do_log(trace_file_, "TRACE", message);
  }

  void debug(std::string const &message) {
    std::lock_guard<std::mutex> lg(lock);
    do_log(std::cout, "DEBUG", message);
    do_log(trace_file_, "DEBUG", message);
  }

  void info(std::string const &message) {
    std::lock_guard<std::mutex> lg(lock);
    do_log(std::cout, "INFO ", message);
    do_log(info_file_, "INFO ", message);
    do_log(trace_file_, "INFO ", message);
  }

  void warn(std::string const &message) {
    std::lock_guard<std::mutex> lg(lock);
    do_log(std::cout, "WARN ", message);
    do_log(info_file_, "WARN ", message);
    do_log(trace_file_, "WARN ", message);
  }

  void error(std::string const &message) {
    std::lock_guard<std::mutex> lg(lock);
    do_log(std::cerr, "ERROR", message);
    do_log(info_file_, "ERROR", message);
    do_log(trace_file_, "ERROR", message);
  }

 private:
  Logger() = default;

  explicit Logger(const std::string &log_dir)
      : info_file_(log_dir + "wolf.log", std::ofstream::out | std::ofstream::app),
        trace_file_(log_dir + "trace.log", std::ofstream::out | std::ofstream::app) {
    if (not info_file_ or not trace_file_) {
      std::cerr << "Couldn't create log files!" << std::endl;
      exit(1);
    }
  }

  std::ofstream info_file_;
  std::ofstream trace_file_;
  static std::atomic<bool> initialized;
  static std::string logging_dir;
  std::mutex lock;

  static void do_log(std::ostream &stream, const std::string &level, std::string const &message) {
    stream << date::format("%F %T", std::chrono::system_clock::now()) << " " << level << " " << message << std::endl;
  }

 public:
  Logger(Logger const &) = delete;
  void operator=(Logger const &) = delete;
};

#endif //WOLF_LOGGER_H
