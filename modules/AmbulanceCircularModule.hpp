#pragma once

#include <iosfwd>
#include "../models/Ambulance.hpp"
#include "../ds/CircularQueue.hpp"

// Module that manages ambulances using a circular queue
class AmbulanceCircularModule {
private:
    // Fixed-size circular queue with capacity 10
    CircularQueue<Ambulance, 10> queue;

public:
    AmbulanceCircularModule();

    // Register a new ambulance. Returns false if the queue is full.
    bool registerAmbulance(const Ambulance& a);

    // Rotate the shift order by one. Returns false if queue is empty.
    bool rotateOnce();

    // Print current rotation order to the given stream.
    void printRotation(std::ostream& os) const;

    bool isEmpty() const { return queue.isEmpty(); }
    int  getAmbulanceCount() const { return queue.getCount(); }
};
