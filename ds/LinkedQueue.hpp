#pragma once

template <typename T>
class LinkedQueue {
public:
    LinkedQueue() : head(nullptr), tail(nullptr) {}

    ~LinkedQueue() {
        while (head) {
            Node* n = head;
            head = head->next;
            delete n;
        }
    }

    void enqueue(const T& v) {
        Node* n = new Node{ v, nullptr };
        if (!tail) {
            head = tail = n;
        }
        else {
            tail->next = n;
            tail = n;
        }
    }

    bool dequeue(T& out) {
        if (!head) return false;
        Node* n = head;
        out = n->data;
        head = head->next;
        if (!head) tail = nullptr;
        delete n;
        return true;
    }

    bool front(T& out) const {
        if (!head) return false;
        out = head->data;
        return true;
    }

    bool isEmpty() const { return head == nullptr; }

private:
    struct Node {
        T data;
        Node* next;
    };

    Node* head;
    Node* tail;
};
