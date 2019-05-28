//
// Created by lamanchy on 27.5.19.
//

#include "option.h"
wolf::options::options(int argc, char **argv) :
    g_opts(argv[0], " - example command line options"),
    argc(argc),
    argv(argv) {
  g_opts.add_options()("h,help", "Prints help");
  cxxopts::Options opts("");
  opts.add_options("")("h,help", "Prints help", cxxopts::value<bool>(_should_print_help));
  opts.allow_unrecognised_options();
  auto r = options::parse_opts(opts, argc, argv);
}
void wolf::options::print_help() {
  Logger::getLogger().info(g_opts.help(g_opts.groups()));
}
cxxopts::ParseResult wolf::options::parse_opts(cxxopts::Options &opts, int argc, char **argv) {
  try {
    return opts.parse(argc, argv);
  } catch (const cxxopts::OptionException &e) {
    Logger::getLogger().error("Error parsing options: " + std::string(e.what()));
    exit(1);
  }
}
