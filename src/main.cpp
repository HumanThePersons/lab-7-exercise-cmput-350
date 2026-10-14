#include <iostream>

#include "entity_manager.h"

int main() {
    // test here..,.
    EntityManager<int, float, bool> testEntityManager;
    EntityID eId = testEntityManager.createEntity();
    testEntityManager.addComponent(eId, 5);

    std::cout << testEntityManager.hasComponent<int>(eId) << '\n';  // Should print 1
    std::cout << testEntityManager.hasComponent<float>(eId) << '\n';  // Should print 0 
    std::cout << testEntityManager.hasComponent<bool>(eId) << '\n';  // Should print 0
    std::cout << testEntityManager.getComponent<int>(eId) << '\n';  // Should print 5
    // testEntityManager.getComponent<bool>(eId);  // should trigger an assert since eId given was mapped to an int
    // testEntityManager.hasComponent<char>(eId); // should give compile error: not a component type
    
    return 0;
}