#pragma once

#include <base/plugins/base_plugin.h>
namespace wolf {

class get_elapsed_preevents : public base_plugin {
 public:
  struct elapsed_config {
    std::string start_logId, end_logId, uniqueId, name;
  };

  static std::vector<elapsed_config> parse_file(const std::string &file_path) {
    std::ifstream file(file_path);
    std::string line;
    std::vector<elapsed_config> result;

    if (file.is_open()) {
      while (std::getline(file, line)) {
        elapsed_config config;

//        rstrip
        line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
          return not std::isspace(ch);
        }).base(), line.end());

        if (line.length() == 0) continue;
        if (line[0] == '#') continue;

        auto it = line.find(':');
        if (it == std::string::npos) {
          logging::logger("get_elapsed_preevents").fatal << "Cannot parsing file " << file_path << ", ':' is missing on line " << line << std::endl;
        }
        config.start_logId = line.substr(0, it);
        line = line.substr(it + 1);

        it = line.find(':');
        if (it == std::string::npos) {
          logging::logger("get_elapsed_preevents").fatal << "Cannot parsing file " << file_path << ", ':' is missing on line " << line << std::endl;
        }
        config.end_logId = line.substr(0, it);
        line = line.substr(it + 1);

        it = line.find(':');
        if (it == std::string::npos) {
          logging::logger("get_elapsed_preevents").fatal << "Cannot parsing file " << file_path << ", ':' is missing on line " << line << std::endl;
        }
        config.uniqueId = line.substr(0, it);
        config.name = line.substr(it + 1);

        result.push_back(config);
      }
      file.close();
    } else {
      logging::logger("get_elapsed_preevents").fatal << "Cannot open file " << file_path << " for regexes." << std::endl;
    }

    return result;
  }

  explicit get_elapsed_preevents(std::vector<elapsed_config> configs) : configs(std::move(configs)) {}

 protected:
  void process(json &&message) override {
    auto logId = message.find("logId");
    if (logId != nullptr) {
      for (elapsed_config &c : configs) {
        if (logId->get_string() == c.start_logId) {
          output_preevent("start", c, message);
        }
        if (logId->get_string() == c.end_logId) {
          output_preevent("end", c, message);
        }
      }
    }
    output(std::move(message));
  }
 private:
  std::vector<elapsed_config> configs;

  void output_preevent(const std::string &position, elapsed_config &config, json &message) {
    auto uniqueId = message.find(config.uniqueId);
    if (uniqueId == nullptr)
      throw std::runtime_error(
          std::string("Trying to get uniqueId: ") + config.uniqueId
              + " from:" + tao::json::to_string(message) + ", and... it's not there!"
      );

    metrics_output({
                       {"algorithm", "elapsed"},
                       {"position", position},
                       {"elapsedId", config.name},
                       {"uniqueId", uniqueId->get_string()},
                       {"@timestamp", message["@timestamp"].get_string()},
                       {"type", "correlation_data"},
                       {"host", message["host"].get_string()},
                       {"group", message["group"].get_string()}
                   });
  }

  void metrics_output(json &&message) {
    output("metrics", std::move(message));
  }

 public:
  template<typename... Args>
  plugin register_preevents_output(plugin plugin, Args &&... args) {
    return register_named_output("metrics", std::move(plugin), args...);
  }

};
}


