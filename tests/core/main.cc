#include "Testing.hpp"
#include "suits/Object.hpp"
#include "suits/Singleton.hpp"
#include <SDL2/SDL.h>
int main(int argc, char *argv[]) {
  Testing<TestObject, TestSingleton> testing;
  return testing.run(argc, argv);
}