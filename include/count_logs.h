#pragma once

#include <base/plugins/base_plugin.h>
#include <base/plugins/mutexed_threaded_plugin.h>
#include <extras/convert_time.h>
namespace wolf {

class count_logs : public mutexed_threaded_plugin {
 public:
  explicit count_logs(std::vector<std::string> fields) : fields(std::move(fields)) {}

  template<typename... Args>
  plugin register_stats_output(plugin plugin, Args &&... args) {
    return register_named_output("stats", std::move(plugin), args...);
  }

 protected:
  void process(json &&message) override {
    std::string key;

    for (std::string &field : fields) {
      auto ptr = message.find(field);
      if (ptr != nullptr)
        key += ptr->get_string() + ":";
    }
    auto nanos = extras::string_to_time(message["@timestamp"].get_string()).time_since_epoch();
    key += std::to_string(std::chrono::duration_cast<std::chrono::minutes>(nanos).count());

    auto it = storage.find(key);

    if (it == storage.end()) {
      storage.insert(std::make_pair(key, json({{"count", 1}})));
      json &item = storage.find(key)->second;

      for (std::string &field : fields) {
        auto ptr = message.find(field);
        if (ptr != nullptr)
          item[field] = ptr->get_string();
      }

      item["@timestamp"] = message["@timestamp"].get_string();
    } else {
      it->second["count"] = it->second["count"].get_signed() + 1;
    }

    output(std::move(message));
  }

  void unlocked_loop() override {
    get_loop_sleeper().sleep_for(std::chrono::seconds(10));
  }

  void locked_loop() override {
    for (auto &item : storage) {
//      std::cout << "outputtting stuff" << tao::json::to_string(item.second) << std::endl;
      stats_output(std::move(item.second));
    }
    storage.clear();
  }

  void stats_output(json &&message) {
    output("stats", std::move(message));
  }

 private:
  std::map<std::string, json> storage;
  std::vector<std::string> fields;
};

}


