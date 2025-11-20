#pragma once

#include <iosfwd>

#include "../models/SupplyItem.hpp"

template <typename T>
class LinkedStack;

class SupplyStackModule {
public:
    SupplyStackModule();
    ~SupplyStackModule();

    SupplyStackModule(const SupplyStackModule&) = delete;
    SupplyStackModule& operator=(const SupplyStackModule&) = delete;
    SupplyStackModule(SupplyStackModule&&) = delete;
    SupplyStackModule& operator=(SupplyStackModule&&) = delete;

    void add(const SupplyItem& s);
    bool useLast(SupplyItem& out);
    void printAll(std::ostream& os) const;

private:
    LinkedStack<SupplyItem>* stack_;
};

void runSupplySubmenu(SupplyStackModule& module);
