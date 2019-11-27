#include <wolf.h>
#include <example_plugin.h>

int main(int argc, char *argv[]) {
  using namespace wolf;
  using std::string;

  options opts = options(argc, argv);
  auto name = opts.add<command<string>>("name", "Type your name", "no one");
  // test

  pipeline p(opts);

  p.register_plugin(
      make<cin>(),
      make<example_plugin>(name),
      make<cout>()
  );

  p.run();

  return 0;
}
