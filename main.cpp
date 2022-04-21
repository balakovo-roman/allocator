#include <map>

#include "allocator.h"
#include "simple_list.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  std::cout << "------------------ start main() ------------------" << '\n';

  std::cout << "------------------ first map ------------------" << '\n';
  std::map<std::string, int, std::less<>, CustomAllocator<std::pair<const std::string, int>, 10>> map_1;

  std::cout << "------------------ fill map_1 ------------------" << '\n';
  map_1.try_emplace("hello", 5);
  map_1.try_emplace("world", 5);

  std::cout << "------------------ second map ------------------" << '\n';
  std::map<std::string, int, std::less<>, CustomAllocator<std::pair<const std::string, int>, 10>> map_2;

  std::cout << "------------------ fill map_2 ------------------" << '\n';
  map_2.try_emplace("foo", 3);

  std::cout << "------------------ operator= ------------------" << '\n';
  map_2 = std::move(map_1);

  std::cout << "------------------ end main() ------------------" << '\n';

  return 0;
}
