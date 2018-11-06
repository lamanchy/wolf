//
// Created by lamanchy on 22.10.2018.
//
#include "logger.h"

std::atomic<bool> Logger::initialized{false};
std::string Logger::logging_dir;