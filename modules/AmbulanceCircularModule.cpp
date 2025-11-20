#include "AmbulanceCircularModule.hpp"
#include <iostream>

AmbulanceCircularModule::AmbulanceCircularModule() : queue() {}

bool AmbulanceCircularModule::registerAmbulance(const Ambulance& a) {
    if (queue.isFull()) {
        std::cout << "[Error] Ambulance queue is full. Cannot register "
            << a.code << ".\n";
        return false;
    }

    queue.enqueue(a);
    std::cout << "[Ambulance Registered] " << a.code
        << " - Driver: " << a.driverName << "\n";
    return true;
}

bool AmbulanceCircularModule::rotateOnce() {
    if (queue.isEmpty()) {
        std::cout << "[Info] No ambulances to rotate.\n";
        return false;
    }

    queue.rotateOnce();
    std::cout << "[Shift rotation done successfully]\n";
    return true;
}

void AmbulanceCircularModule::printRotation(std::ostream& os) const {
    queue.display(os);
}
