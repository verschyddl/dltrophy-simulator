#include <iostream>
#include "MainWindow.h"

int main() {
    try {

        MainWindow window(1280, 1080, "DL Trophy Smiulator");
        window.run();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
