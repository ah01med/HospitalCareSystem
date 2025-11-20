#pragma once
#include <iosfwd>
#include "models/EmergencyCase.hpp"

class EmergencyPQModule {
public:
    void logCase(const EmergencyCase& e);          // Insert new case
    bool processTop(EmergencyCase& out);           // Remove highest priority case
    void printByPriority(std::ostream& os) const;  // View all (non destructive)

    // NEW: just peek at the most critical case without removing it
    bool peekTop(EmergencyCase& out) const;

    // NEW: show statistics (total, count per priority, highest priority)
    void printStats(std::ostream& os) const;
};
