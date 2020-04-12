#include <wolf.h>
#include <get_elapsed_preevents.h>
#include <count_logs.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  options o(argc, argv);

  auto input_port = o.add<input<unsigned short>>("input_port", "Input port", "9069");
  auto stream_sort_seconds = o.add<input<int>>("stream_sort_seconds", "Seconds to wait with each event", "60");

  pipeline p(o);

  plugin output = pipeline::chain_plugins(
      make<to::string>(),
      make<to::line>(),
      make<tcp::output>("localhost", "9070")
  );

  plugin sort_by_time = make<stream_sort>(
      [](const json &lhs, const json &rhs) -> bool {
        return lhs.find("@timestamp")->get_string() > rhs.find("@timestamp")->get_string();
      },
      stream_sort::ready_after(std::chrono::seconds(stream_sort_seconds->value()))
  );

  p.register_plugin(
      make<tcp::input>(input_port),
      make<from::line>(),
      make<from::string>(),

      make<filter>(
          [](const json &message) {
            auto type = message.find("type");
            return type != nullptr && type->get_string() == "metrics";
          }
      )->filtered(
          make<lambda>(
              [](json &message) {
                message.assign_string(std::string(message["message"].get_string()));
              }
          ),
          make<drop>()
      ),

      make<regex>(regex::parse_file(p.get_config_dir() + "parsers")),

      make<get_elapsed_preevents>(
          get_elapsed_preevents::parse_file(p.get_config_dir() + "elapsed")
      )->register_preevents_output(
          sort_by_time,
          make<elapsed>(1800)->register_expired_output(
              make<to::influx>(
                  "elapsed",
                  std::vector<std::string>({"elapsedId", "status", "start_host", "group"}),
                  std::vector<std::string>({"uniqueId",}),
                  "start_time"
              ),
              make<drop>()
          ),
          make<to::influx>(
              "elapsed",
              std::vector<std::string>({"elapsedId", "status", "start_host", "end_host", "group"}),
              std::vector<std::string>({"uniqueId", "duration"}),
              "start_time"
          ),
          make<drop>()
      ),

      make<count_logs>(
          std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"})
      )->register_stats_output(
          make<to::influx>(
              "logs_count",
              std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"}),
              std::vector<std::string>({"count"}),
              "@timestamp"
          ),
          make<drop>()
      ),

      output
  );

  p.run();

  return 0;
}
