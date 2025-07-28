#define SDL_MAIN_HANDLED
#include "ecs/ECSExample.h"
#include <iostream>

/**
 * @brief Simple program to demonstrate ECS functionality
 */
int main() {
    std::cout << "ECS Demo Application" << std::endl;
    std::cout << "===================" << std::endl;

    try {
        ecs::ECSExample ecsDemo;
        ecsDemo.runDemo();

        std::cout << "\nDemo completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error during ECS demo: " << e.what() << std::endl;
        return 1;
    }
}
