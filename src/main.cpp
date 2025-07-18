#include "Simulator/Simulator.hpp"
int main() {
  Simulator simulator(1000, 600, "GateWorks: Digital Circuit Simulator", 60);
  simulator.Start();
  return 0;
}
