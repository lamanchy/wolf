

#include <base/options/options.h>
wolf::options::options(int argc, char **argv) :
    g_opts(wolf::extras::get_executable_name()),
    argc(argc),
    argv(argv) {}

void wolf::options::print_help() {
  logger.info(g_opts.help(g_opts.groups()));
}

void wolf::options::parse_options() {
  try {
    auto result = g_opts.parse(argc, argv);

    for (const auto &o : all_options)
      o->validate_options(result);

  } catch (cxxopts::option_not_exists_exception &ex) {
    bool should_print_help;
    g_opts.add_options(general_config_group_name)("h,help", "Print help", cxxopts::value(should_print_help));
    g_opts.allow_unrecognised_options();
    auto result = g_opts.parse(argc, argv);

    if (not should_print_help)
      logger.fatal("Error parsing options: " + std::string(ex.what()));

    print_help();
    exit(0);
  }
}
const std::string wolf::options::general_config_group_name{"General configuration"};

void wolf::options::print_options() {
  logger.info("Provided options:");
  for (const auto &o : all_options)
    o->print_info();

  logger.info("");
}
