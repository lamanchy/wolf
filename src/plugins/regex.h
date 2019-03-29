#include <utility>

//
// Created by lamanchy on 10.3.19.
//

#ifndef WOLF_REGEX_H
#define WOLF_REGEX_H

#include <base/plugins/plugin.h>
#include <re2/set.h>

namespace wolf {

class regex : public plugin {
 public:
  using named_regex = std::pair<std::string, std::string>;
  explicit regex(const std::vector<named_regex> &regex_pairs) : regex_set(re2::RE2::Options(), re2::RE2::UNANCHORED) {
    for (const named_regex & pair : regex_pairs) {
      std::string error;
      int result = regex_set.Add(pair.second, &error);
      if (result == -1) {
        logger.error(error);
        exit(1);
      }
      regexes.insert(std::make_pair(result, std::make_pair(pair.first, std::make_shared<re2::RE2>(pair.second))));
    }
    int result = regex_set.Compile();
    if (result == false) {
      logger.error("Regex set run out of memory");
      exit(1);
    }
  }

  static std::vector<named_regex> parse_file(const std::string &file_path) {
    std::ifstream file(file_path);
    std::string line;
    std::vector<named_regex> result;

    if (file.is_open()) {
      while (std::getline(file, line)) {
//        rstrip
        line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
          return not std::isspace(ch);
        }).base(), line.end());

        if (line.length() == 0) continue;
        if (line[0] == '#') continue;

        auto it = line.find(':');
        if (it == std::string::npos) {
          Logger::getLogger().error("Cannot parsing file " + file_path + ", ':' is missing on line " + line);
          exit(1);
        }
        result.emplace_back(line.substr(0, it), line.substr(it+1));
      }
      file.close();
    } else {
      Logger::getLogger().error("Cannot open file " + file_path + " for regexes.");
      exit(1);
    }

    return result;
  }

 protected:
  void process(json &&message) override {
    std::vector<int> result;
    bool bool_res = regex_set.Match(message.find("message")->get_string(), &result);
    if (bool_res) {
      std::sort(result.begin(), result.end());
      int min_i = result[0];

      message["logId"] = regexes.at(min_i).first;

      re2::RE2 & regex = *regexes.at(min_i).second;

      int submatches_count = 1 + regex.NumberOfCapturingGroups();
      re2::StringPiece submatches[submatches_count];

      regex.Match(
          message["message"].get_string(), 0, message["message"].get_string().length(),
          re2::RE2::UNANCHORED, submatches, submatches_count);

      for (auto & pair : regex.NamedCapturingGroups()) {
        message[pair.first] = std::string(submatches[pair.second].data(), submatches[pair.second].length());
      }
    } else {
      message["logId"] = "default";
    }
    output(std::move(message));
  }
 private:
  re2::RE2::Set regex_set;
  std::map<int, std::pair<std::string, std::shared_ptr<re2::RE2>>> regexes;
};

}

#endif //WOLF_REGEX_H
