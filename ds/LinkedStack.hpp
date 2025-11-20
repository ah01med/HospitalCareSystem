#pragma once

#include <iosfwd>
#include <ostream>

template <typename T>
class LinkedStack {
public:
    LinkedStack() noexcept;
    LinkedStack(const LinkedStack& other);
    LinkedStack(LinkedStack&& other) noexcept;
    ~LinkedStack();

    LinkedStack& operator=(const LinkedStack& other);
    LinkedStack& operator=(LinkedStack&& other) noexcept;

    void push(const T& item);
    bool pop(T& out);
    bool peek(T& out) const;
    bool isEmpty() const noexcept;
    void print(std::ostream& os) const;

    template <typename Fn>
    void forEach(Fn fn) const;

private:
    struct Node {
        T     data;
        Node* next;
    };

    Node* top_;

    void clear() noexcept;
    static Node* cloneNodes(const Node* node);
};

// Template definitions

template <typename T>
LinkedStack<T>::LinkedStack() noexcept : top_(nullptr) {}

template <typename T>
LinkedStack<T>::LinkedStack(const LinkedStack& other)
    : top_(cloneNodes(other.top_)) {}

template <typename T>
LinkedStack<T>::LinkedStack(LinkedStack&& other) noexcept
    : top_(other.top_) {
    other.top_ = nullptr;
}

template <typename T>
LinkedStack<T>::~LinkedStack() {
    clear();
}

template <typename T>
LinkedStack<T>& LinkedStack<T>::operator=(const LinkedStack& other) {
    if (this != &other) {
        clear();
        top_ = cloneNodes(other.top_);
    }
    return *this;
}

template <typename T>
LinkedStack<T>& LinkedStack<T>::operator=(LinkedStack&& other) noexcept {
    if (this != &other) {
        clear();
        top_ = other.top_;
        other.top_ = nullptr;
    }
    return *this;
}

template <typename T>
void LinkedStack<T>::push(const T& item) {
    Node* node = new Node{ item, top_ };
    top_ = node;
}

template <typename T>
bool LinkedStack<T>::pop(T& out) {
    if (!top_) return false;
    Node* node = top_;
    out = node->data;
    top_ = node->next;
    delete node;
    return true;
}

template <typename T>
bool LinkedStack<T>::peek(T& out) const {
    if (!top_) return false;
    out = top_->data;
    return true;
}

template <typename T>
bool LinkedStack<T>::isEmpty() const noexcept {
    return top_ == nullptr;
}

template <typename T>
void LinkedStack<T>::print(std::ostream& os) const {
    const Node* current = top_;
    while (current) {
        os << current->data;
        current = current->next;
        if (current) os.put('\n');
    }
}

template <typename T>
template <typename Fn>
void LinkedStack<T>::forEach(Fn fn) const {
    const Node* current = top_;
    while (current) {
        fn(current->data);
        current = current->next;
    }
}

template <typename T>
void LinkedStack<T>::clear() noexcept {
    while (top_) {
        Node* node = top_;
        top_ = node->next;
        delete node;
    }
}

template <typename T>
typename LinkedStack<T>::Node* LinkedStack<T>::cloneNodes(const Node* node) {
    if (!node) return nullptr;
    Node* newTop = new Node{ node->data, nullptr };
    Node* tail = newTop;
    const Node* current = node->next;
    while (current) {
        tail->next = new Node{ current->data, nullptr };
        tail = tail->next;
        current = current->next;
    }
    return newTop;
}
