#pragma once

#include <base/plugins/base_plugin.h>
#include <extras/convert_time.h>

namespace wolf {

class normalize_nlog_logs : public base_plugin {
 protected:
  void process(json &&message) override {
    message["@timestamp"] = extras::convert_time(message["time"].as<std::string>());
    message.erase("time");
    output(std::move(message));
  }
};

}


