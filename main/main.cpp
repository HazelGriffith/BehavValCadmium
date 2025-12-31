#include <limits>
#include "include/Counter_Top.hpp"
#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/simulation/logger/stdout.hpp"
#include "cadmium/simulation/logger/csv.hpp"

using namespace cadmium;

int main(int argc, char* argv[]){
	auto counter_top = std::make_shared<Counter_Top> ("counter_top");

	auto rootCoordinator = RootCoordinator(counter_top);

	rootCoordinator.setLogger<CSVLogger>("../simulation_results/counter_test.csv",",");

	rootCoordinator.start();
	rootCoordinator.simulate(30.0);
	rootCoordinator.stop();	

	return 0;
}