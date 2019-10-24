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
  std::string get_logging_dir() {
    return logging_dir;
  }

  static Logger &getLogger();

  void trace(std::string const &message);

  void debug(std::string const &message);

  void info(std::string const &message);

  void warn(std::string const &message);

  void error(std::string const &message);

  void fatal(std::string const &message);

 private:
  Logger() {
    set_logging_dir(wolf::extras::get_executable_dir());
  }

  std::ofstream info_file_;
  std::ofstream trace_file_;
  std::mutex lock;
  std::string logging_dir;

  static void do_log(std::ostream &stream, const std::string &level, std::string const &message);

 public:
  void set_logging_dir(const std::string &path);

  Logger(Logger const &) = delete;
  void operator=(Logger const &) = delete;
};

#endif //WOLF_LOGGER_H
