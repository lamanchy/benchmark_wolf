#include <wolf.h>
#include <add_local_info.h>
#include <normalize_nlog_logs.h>
#include <normalize_serilog_logs.h>
#include <normalize_log4j2_logs.h>

int main(int argc, char *argv[]) {
  using namespace wolf;
  using std::string;

  options opts = options(argc, argv);
  auto output_ip = opts.add<command<string>>("output_ip", "Ip address of output", "localhost");
  auto group = opts.add<command<string>>("group", "Define the group name", "default");
  auto max_loglevel = opts.add<command<string>>(
      "max_loglevel", "Define max loglevel, one of OFF, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, ALL", "INFO");

  pipeline p(opts);

  plugin out = pipeline::chain_plugins(
      make<to::string>(),
      make<to::line>(),
      make<stats>(),
      make<tcp::output>(output_ip, "9070")
  );

  plugin common_postprocessing = pipeline::chain_plugins(
      make<add_local_info>(group, max_loglevel),
      out
  );

  plugin common_preprocessing = pipeline::chain_plugins(
      make<from::line>(),
      make<from::string>()
  );

  p.register_plugin(
      make<tcp::input>(9556),
      common_preprocessing,
      make<normalize_nlog_logs>(),
      common_postprocessing
  );

  p.register_plugin(
      make<tcp::input>(9555),
      common_preprocessing,
      make<normalize_log4j2_logs>(),
      common_postprocessing
  );

  p.register_plugin(
      make<tcp::input>(9559),
      common_preprocessing,
      make<normalize_serilog_logs>(),
      common_postprocessing
  );

  p.register_plugin(
      make<tcp::input>(9557),
      make<from::line>(),
      make<lambda>(
          [group](json &message) {
            message.assign_object(
                {
                    {"message", message},
                    {"group", group->value()},
                    {"type", "metrics"}
                });
          }),
      out
  );
  p.run();

  return 0;
}
