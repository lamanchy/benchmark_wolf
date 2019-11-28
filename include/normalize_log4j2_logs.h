#pragma once

#include <base/plugins/base_plugin.h>
#include <extras/convert_time.h>

namespace wolf {

class normalize_log4j2_logs : public base_plugin {
 protected:
  void process(json &&message) override {
    if (message.find("timeMillis") != nullptr) {
      message["@timestamp"] = extras::convert_time(message["timeMillis"].get_unsigned());
      message.erase("timeMillis");
    } else if (message.find("instant") != nullptr) {
      message["@timestamp"] = extras::convert_time(message["instant"]["epochSecond"].get_unsigned(),
                                                   message["instant"]["nanoOfSecond"].get_unsigned());
      message.erase("instant");
    } else {
      throw std::runtime_error("Cannot find time");
    }
    output(std::move(message));
  }
};

}


