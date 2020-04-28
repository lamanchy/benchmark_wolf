#include <wolf.h>
#include <add_local_info.h>

int main(int argc, char *argv[]) {
  using namespace wolf;
  using std::string;

  options opts = options(argc, argv);
  auto output_ip = opts.add<input<string>>("output_ip", "Ip address of output", "localhost");
  auto group = opts.add<input<string>>("group", "Define the group name", "default");
  auto max_loglevel = opts.add<input<string>>(
      "max_loglevel", "Define max loglevel, one of OFF, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, ALL", "INFO");

  pipeline p(opts);

  plugin out = pipeline::chain_plugins(
      make<to::string>(),
      make<to::line>(),
      make<stats>(),
      make<tcp::output>(output_ip, "9070")
  );

  plugin common_postprocessing = pipeline::chain_plugins(
      make<from::line>(),
      make<from::string>(),
      make<add_local_info>(group, max_loglevel),
      out
  );

  p.register_plugin(
      make<tcp::input>(9556),
      common_postprocessing
  );

  p.register_plugin(
      make<tcp::input>(9555),
      common_postprocessing
  );

  p.register_plugin(
      make<tcp::input>(9559),
      common_postprocessing
  );

  p.register_plugin(
      make<tcp::input>(9557),
      make<from::line>(),
      make<lambda>(
          [](json &message) {
            message.assign_object(
                {
                    {"message", message},
                    {"type", "metrics"}
                });
          }),
      out
  );
  p.run();

  return 0;
}
