#include "APP.hpp"
#include <fstream>

// Thread timeouts (in ms)
#define WORLD_INTERVAL 1000
#define AIRPORTS_INTERVAL 250

mutex cout_lock; // A thread lock
int main(int argc, char *argv[]) {
  std::srand(std::time(nullptr));

  Location loc(0, 0, 0);

  Plane train(loc, 0);
  cout << train << endl;
}