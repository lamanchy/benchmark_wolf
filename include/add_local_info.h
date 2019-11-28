#pragma once

#include <asio/ip/host_name.hpp>
#include <base/plugins/base_plugin.h>

namespace wolf {

class add_local_info : public base_plugin {
 public:
  add_local_info(const option<std::string> &group,
                 const option<std::string> &max_loglevel) :
      group(group->value()),
      max_loglevel(max_loglevel->value()) {
    auto it = std::find(loglevels.begin(), loglevels.end(), this->max_loglevel);
    if (it == loglevels.end()) {
      throw std::invalid_argument(this->max_loglevel + "is not valid loglevel");
    }
  }

 protected:
  void process(json &&message) override {
    auto host = message.find("host");
    if (host == nullptr) {
      message["host"] = asio::ip::host_name();
    }

    message["group"] = group;
    auto component = message.find("component");
    if (component == nullptr) {
      message["component"] = "default";
    }

    std::string level = message["level"].get_string();
    for (const std::string &loglevel : loglevels) {
      if (loglevel == level) break;
      if (loglevel == max_loglevel) return;
    }

    if (message["message"].get_string().size() > 32000) {
      std::string m = message["message"].get_string();
      m.resize(32000);
      m += "... truncated";
      message["message"] = m;
    }

    output(std::move(message));
  }

 private:
  std::string group;
  std::string max_loglevel;
  std::vector<std::string> loglevels = {"OFF", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE", "ALL"};
};

}


