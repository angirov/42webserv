#include "create_objects.hpp"

int main() {
	Config config;
	createObjects(config);
	displayConfig(config);

	// Display VirtServer and Location objects using display() functions
	std::vector<VirtServer> virtServers = config.getVirtServers();
	for (size_t i = 0; i < virtServers.size(); ++i) {
		virtServers[i].display();
	}

	return 0;
}
