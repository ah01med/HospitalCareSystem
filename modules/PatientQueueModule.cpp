#include "PatientQueueModule.hpp"
#include "../ds/LinkedQueue.hpp"
#include <iostream>

// Single queue instance for all patient operations in this translation unit
static LinkedQueue<Patient> g_patients;

void PatientQueueModule::admit(const Patient& p) {
    g_patients.enqueue(p);
}

bool PatientQueueModule::discharge(Patient& out) {
    return g_patients.dequeue(out);
}

void PatientQueueModule::printQueue(std::ostream& os) const {
    
    LinkedQueue<Patient> temp;
    LinkedQueue<Patient> restore;
    Patient item;

    while (g_patients.dequeue(item)) {
        os << item.id << " | " << item.name
            << " | " << item.conditionType << "\n";
        restore.enqueue(item);
    }

    while (restore.dequeue(item)) {
        g_patients.enqueue(item);
    }
}
