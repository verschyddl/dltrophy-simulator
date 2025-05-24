#include <iostream>
#include "SimulatorApp.h"

int main(int argc, char* argv[]) {

    try {
        Config config(argc, argv);
        SimulatorApp app(config);
        app.run();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

}
