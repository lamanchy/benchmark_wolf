#include <wolf.h>

int main(int argc, char *argv[]) {
  using namespace wolf;
  using std::string;

  pipeline p(argc, argv);

  p.register_plugin(
      make<tcp::input>(9556),
      make<lambda>([](json&){}),
      make<tcp::output>("localhost", "9070")
  );
  p.run();

  return 0;
}
