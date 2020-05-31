#pragma once

#include <iomanip>
#include <iso646.h>
#include <string>
#include <iostream>
#include <fstream>
#include <atomic>
#include <chrono>
#include <date/date.h>
#include <mutex>
#include <utility>
#include <extras/get_executable_path.h>
#include <extras/get_time.h>

class BaseLogger {
 public:
  static BaseLogger &getLogger();
  std::string get_logging_dir() { return logging_dir; }

  void set_logging_dir(const std::string &path);

  void check_file_rotation();

  template<typename T>
  void log(const std::string &level, const std::string &prefix, const T &message) {
    if (empty) lock.lock();
    if (level == "FATAL" or level == "ERROR") {
      do_log(std::cerr, prefix, level, message);
    } else {
      do_log(std::cout, prefix, level, message);
    }

    do_log(trace_file_, prefix, level, message);

    if (level != "TRACE" and level != "DEBUG") {
      do_log(info_file_, prefix, level, message);
    }

    set_empty(message);
    if (empty) lock.unlock();

    if (empty and level == "FATAL") {
      exit(1);
    }
  }

  BaseLogger(const BaseLogger &) = delete;
  void operator=(const BaseLogger &) = delete;
 private:
  std::string get_file_path(const std::string & name);

  void setup_files();

  template<typename T>
  void set_empty(const T &message) { empty = false; }

  // value is std::endl
  void set_empty(std::ostream &(*message)(std::ostream &)) { empty = true; }

  BaseLogger() {
    set_logging_dir(wolf::extras::get_executable_dir());
  }

  std::ofstream info_file_, trace_file_;
  std::mutex lock;
  std::string logging_dir;
  std::string current_date = wolf::extras::get_date();
  static thread_local bool empty;

  template<typename T>
  void do_log(std::ostream &stream,
              const std::string &prefix,
              const std::string &level,
              const T &message) {
    if (stream) {
      if (empty) {
        stream << date::format("%F %T", std::chrono::system_clock::now())
               << " "
               << std::setw(5) << level
               << " "
               << "[" << std::setw(20) << prefix << "]"
               << " ";
      }
      stream << message;
    }
  }

};

class LogStream {
 public:
  explicit LogStream(std::string level, std::string prefix) :
      prefix(std::move(prefix)), level(std::move(level)) {}

  template<typename T>
  LogStream &operator<<(const T &message) {
    logger.log(level, prefix, message);
    return *this;
  }
  LogStream &operator<<(std::ostream &(*message)(std::ostream &)) {
    logger.log(level, prefix, message);
    return *this;
  }
 private:
  std::string prefix, level;
  BaseLogger &logger = BaseLogger::getLogger();
};

class Logger {
 public:
  LogStream trace;
  LogStream debug;
  LogStream info;
  LogStream warn;
  LogStream error;
  LogStream fatal;

  explicit Logger(const std::string &prefix) :
      trace(LogStream("TRACE", prefix)),
      debug(LogStream("DEBUG", prefix)),
      info(LogStream("INFO", prefix)),
      warn(LogStream("WARN", prefix)),
      error(LogStream("ERROR", prefix)),
      fatal(LogStream("FATAL", prefix)) {}

  static std::string get_logging_dir() {
    return BaseLogger::getLogger().get_logging_dir();
  }
  static void set_logging_dir(const std::string& path) {
    BaseLogger::getLogger().set_logging_dir(path);
  }
  static void check_file_rotation() {
    BaseLogger::getLogger().check_file_rotation();
  }
};

