#pragma once
#include <string>
#include <ostream>   

struct Ambulance {
    std::string code;
    std::string driverName;
};

// Used by CircularQueue::display for nice printing
inline std::ostream& operator<<(std::ostream& os, const Ambulance& ambulance) {
    os << ambulance.code << " - Driver: " << ambulance.driverName;
    return os;
}
