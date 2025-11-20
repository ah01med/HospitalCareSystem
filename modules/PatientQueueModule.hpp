#pragma once

#include <iosfwd>
#include "../models/Patient.hpp"

class PatientQueueModule {
public:
    void admit(const Patient& p);
    bool discharge(Patient& out);
    void printQueue(std::ostream& os) const;
};
