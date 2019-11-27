#pragma once

#include <base/plugins/base_plugin.h>

namespace wolf {
class example_plugin : public base_plugin {
 public:
  example_plugin(const option<std::string> &name) : name(name->value()) {}

 protected:
  void process(json &&message) override {
    auto res = json(name + " says " + message.get_string());
    output(std::move(res));
  }

 private:
  std::string name;
};
}


