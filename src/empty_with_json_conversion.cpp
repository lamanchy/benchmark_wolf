#include <wolf.h>

int main(int argc, char *argv[]) {
  using namespace wolf;
  using std::string;

  pipeline p(argc, argv);

  p.register_plugin(
      make<tcp::input>(9556),
      make<from::line>(),
      make<from::string>(),
      make<to::string>(),
      make<to::line>(),
      make<tcp::output>("localhost", "9070")
  );
  p.run();

  return 0;
}
