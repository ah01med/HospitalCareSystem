#ifndef CIRCULARQUEUE_HPP
#define CIRCULARQUEUE_HPP

#include <iostream>
#include <stdexcept>
#include <string>

template <typename T, int MAX_SIZE = 10>
class CircularQueue {
private:
    T data[MAX_SIZE];
    int front;
    int rear;
    int count;

public:
    // Constructor
    CircularQueue() : front(0), rear(-1), count(0) {}

    // Check if queue is empty
    bool isEmpty() const {
        return count == 0;
    }

    // Check if queue is full
    bool isFull() const {
        return count == MAX_SIZE;
    }

    // Get current count of elements
    int getCount() const {
        return count;
    }

    // Add element to queue
    void enqueue(const T& item) {
        if (isFull()) {
            throw std::overflow_error("Queue is full");
        }
        rear = (rear + 1) % MAX_SIZE;
        data[rear] = item;
        count++;
    }

    // Remove element from queue
    T dequeue() {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty");
        }
        T item = data[front];
        front = (front + 1) % MAX_SIZE;
        count--;

        // Reset if empty
        if (isEmpty()) {
            front = 0;
            rear = -1;
        }
        return item;
    }

    // Peek at front element
    T& peekFront() {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty");
        }
        return data[front];
    }

    const T& peekFront() const {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty");
        }
        return data[front];
    }

    // Rotate front element to back
    void rotateOnce() {
        if (isEmpty() || count == 1) {
            return; 
        }
        T frontItem = dequeue();
        enqueue(frontItem);
    }

    // Display queue contents
    void display(std::ostream& os) const {
        if (isEmpty()) {
            os << "[No ambulances registered]" << std::endl;
            return;
        }

        os << "--- Current Ambulance Rotation ---" << std::endl;
        for (int i = 0; i < count; i++) {
            int index = (front + i) % MAX_SIZE;
            os << (i + 1) << ") " << data[index] << std::endl;
        }
        os << "----------------------------------" << std::endl;
    }
};

#endif