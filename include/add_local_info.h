#pragma once

#include <asio/ip/host_name.hpp>
#include <base/plugins/base_plugin.h>

namespace wolf {

class add_local_info : public base_plugin {
 public:
  add_local_info(const static_option<std::string> &group,
                 const static_option<std::string> &max_loglevel) :
      group(group->value()),
      max_loglevel(max_loglevel->value()) {
    auto it = std::find(loglevels.begin(), loglevels.end(), this->max_loglevel);
    if (it == loglevels.end()) {
      throw std::invalid_argument(this->max_loglevel + "is not valid loglevel");
    }
  }

 protected:
  void process(json &&message) override {
    unsigned int port = message.metadata["port"].get_unsigned();
    if (port == 9559) {
      message["level"] = normalize_serilog_level(message["level"].get_string());
    }

    const std::string &level = message["level"].get_string();
    for (const std::string &loglevel : loglevels) {
      if (loglevel == level) break;
      if (loglevel == max_loglevel) return;
    }

    if (port == 9559) { // serilog
      message["@timestamp"] = extras::utc_time(message["timestamp"].get_string(), "%FT%H:%M:%10S%Ez");
      message.erase("timestamp");
      auto logId = message.find("logId");
      if (logId != nullptr) {
        std::string &id = logId->get_string();
        id[0] = static_cast<char>(tolower(id[0]));
      }
    } else if (port == 9555) { // log4j
      if (message.find("timeMillis") != nullptr) {
        message["@timestamp"] = extras::utc_time(message["timeMillis"].get_unsigned());
        message.erase("timeMillis");
      } else if (message.find("instant") != nullptr) {
        message["@timestamp"] = extras::utc_time(message["instant"]["epochSecond"].get_unsigned(),
                                                 message["instant"]["nanoOfSecond"].get_unsigned());
        message.erase("instant");
      } else {
        throw std::runtime_error("Cannot find time");
      }
    } else if (port == 9556) {
      message["@timestamp"] = extras::utc_time(message["time"].get_string(), "%F %H:%M:%6S");
      message.erase("time");
    }

    if (message.find("host") == nullptr) {
      message["host"] = asio::ip::host_name();
    }

    message["group"] = group;
    if (message.find("component") == nullptr) {
      message["component"] = "default";
    }

    if (message["message"].get_string().size() > 32000) {
      std::string &m = message["message"].get_string();
      m.resize(32000);
      m += "... truncated";
    }

    output(std::move(message));
  }

 private:
  std::string group;
  std::string max_loglevel;
  const std::array<const std::string, 8> loglevels = {
      "OFF",
      "FATAL",
      "ERROR",
      "WARN",
      "INFO",
      "DEBUG",
      "TRACE"
  };

  static std::string normalize_serilog_level(const std::string &level) {
    if (level == "Information") return "INFO";
    if (level == "Warning") return "WARN";
    if (level == "Debug") return "DEBUG";
    if (level == "Error") return "ERROR";
    if (level == "Fatal") return "FATAL";
    if (level == "Verbose") return "TRACE";
    throw std::runtime_error("Unknown log level " + level);
  }
};

}


