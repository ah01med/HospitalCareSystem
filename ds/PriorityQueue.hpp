#pragma once

template <typename U>
struct DefaultGreater {
    bool operator()(const U& a, const U& b) const { return a > b; }
};

template <typename T, typename Compare = DefaultGreater<T>>
class PriorityQueue {
public:
    PriorityQueue() : arr_(nullptr), cap_(0), len_(0), cmp_() {
        reserve(8);
    }
    ~PriorityQueue() { delete[] arr_; }

    void push(const T& v) {
        if (len_ >= cap_) grow();
        arr_[len_] = v;
        siftUp(len_);
        ++len_;
    }

    bool popMax(T& out) {
        if (len_ == 0) return false;
        out = arr_[0];
        arr_[0] = arr_[--len_];
        siftDown(0);
        return true;
    }

    bool peekMax(T& out) const {
        if (len_ == 0) return false;
        out = arr_[0];
        return true;
    }

    bool isEmpty() const { return len_ == 0; }
    int  size() const { return len_; }

private:
    T* arr_;
    int cap_;
    int len_;
    Compare cmp_;

    void reserve(int n) {
        if (n <= cap_) return;
        T* newArr = new T[n];
        for (int i = 0; i < len_; ++i) newArr[i] = arr_[i];
        delete[] arr_;
        arr_ = newArr;
        cap_ = n;
    }

    void grow() { reserve(cap_ == 0 ? 8 : cap_ * 2); }

    static int parent(int i) { return (i - 1) / 2; }
    static int left(int i) { return 2 * i + 1; }
    static int right(int i) { return 2 * i + 2; }

    void siftUp(int i) {
        while (i > 0) {
            int p = parent(i);
            if (cmp_(arr_[i], arr_[p])) {
                swap(arr_[i], arr_[p]);
                i = p;
            }
            else break;
        }
    }

    void siftDown(int i) {
        while (true) {
            int l = left(i), r = right(i), largest = i;
            if (l < len_ && cmp_(arr_[l], arr_[largest])) largest = l;
            if (r < len_ && cmp_(arr_[r], arr_[largest])) largest = r;
            if (largest != i) {
                swap(arr_[i], arr_[largest]);
                i = largest;
            }
            else break;
        }
    }

    static void swap(T& a, T& b) {
        T tmp = static_cast<T&&>(a);
        a = static_cast<T&&>(b);
        b = static_cast<T&&>(tmp);
    }
};
