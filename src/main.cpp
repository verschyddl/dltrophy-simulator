#include <iostream>
#include "SimulatorApp.h"

int main() {

    try {
        SimulatorApp app(1280, 1080, 3413);
        app.run();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

}
