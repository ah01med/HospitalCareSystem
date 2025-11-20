#include "SupplyStackModule.hpp"


#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include "../ds/LinkedStack.hpp"

namespace {

    std::size_t digits(int value) {
        if (value == 0) return 1U;

        std::size_t count = 0;
        int         v = value;

        if (v < 0) {
            ++count; 
            v = -v;
        }

        while (v > 0) {
            v /= 10;
            ++count;
        }

        return count;
    }

    void divider(std::ostream& os, std::size_t width) {
        for (std::size_t i = 0; i < width; ++i) os.put('-');
        os.put('\n');
    }

} // namespace

void SupplyStackModule::add(const SupplyItem& s) {
    if (s.quantity <= 0) {
        std::cout << "[Error] Quantity must be greater than zero." << std::endl;
        return;
    }
    if (s.batch.empty()) {
        std::cout << "[Error] Batch cannot be empty." << std::endl;
        return;
    }
    stack_->push(s);
    std::cout << "[Info] Stock added: " << s.type << " x" << s.quantity
        << " (" << s.batch << ")" << std::endl;
}

bool SupplyStackModule::useLast(SupplyItem& out) {
    if (!stack_->pop(out)) {
        std::cout << "[Error] No supplies available." << std::endl;
        return false;
    }
    return true;
}

void SupplyStackModule::printAll(std::ostream& os) const {
    std::size_t typeWidth = std::string("Type").size();
    std::size_t qtyWidth = std::string("Qty").size();
    std::size_t batchWidth = std::string("Batch").size();

    stack_->forEach([&](const SupplyItem& item) {
        if (item.type.size() > typeWidth) typeWidth = item.type.size();
        std::size_t qDigits = digits(item.quantity);
        if (qDigits > qtyWidth) qtyWidth = qDigits;
        if (item.batch.size() > batchWidth) batchWidth = item.batch.size();
        });

    const std::size_t totalWidth = typeWidth + qtyWidth + batchWidth + 10U;
    const std::size_t innerMessageWidth = totalWidth - 4U;

    const auto defaultFlags = os.flags();
    const auto defaultFill = os.fill();

    divider(os, totalWidth);

    os << "| " << std::left << std::setw(static_cast<int>(typeWidth)) << "Type"
        << " | " << std::right << std::setw(static_cast<int>(qtyWidth)) << "Qty"
        << " | " << std::left << std::setw(static_cast<int>(batchWidth)) << "Batch"
        << " |" << '\n';

    os.flags(defaultFlags);
    os.fill(defaultFill);

    divider(os, totalWidth);

    if (stack_->isEmpty()) {
        os << "| " << std::left << std::setw(static_cast<int>(innerMessageWidth))
            << "No supplies recorded" << " |" << '\n';
        os.flags(defaultFlags);
        os.fill(defaultFill);
        divider(os, totalWidth);
        return;
    }

    stack_->forEach([&](const SupplyItem& item) {
        os << "| " << std::left << std::setw(static_cast<int>(typeWidth)) << item.type
            << " | " << std::right << std::setw(static_cast<int>(qtyWidth)) << item.quantity
            << " | " << std::left << std::setw(static_cast<int>(batchWidth)) << item.batch
            << " |" << '\n';
        os.flags(defaultFlags);
        os.fill(defaultFill);
        });

    divider(os, totalWidth);
}

SupplyStackModule::SupplyStackModule() {
    stack_ = new LinkedStack<SupplyItem>();
}

SupplyStackModule::~SupplyStackModule() {
    delete stack_;
}


void runSupplySubmenu(SupplyStackModule& module) {
    for (;;) {
        std::cout << "\n[Medical Supplies]\n"
            << "1. Add stock\n"
            << "2. Use last added\n"
            << "3. View all supplies\n"
            << "0. Back\n> ";

        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[Error] Invalid input." << std::endl;
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) break;

        switch (choice) {
        case 1: {
            SupplyItem item;

            std::cout << "Type: ";
            std::getline(std::cin, item.type);

            std::cout << "Quantity: ";
            if (!(std::cin >> item.quantity)) {
                std::cin.clear();
                std::cout << "[Error] Invalid quantity." << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Batch: ";
            std::getline(std::cin, item.batch);

            module.add(item);
        } break;

        case 2: {
            SupplyItem removed;
            if (module.useLast(removed)) {
                std::cout << "[Info] Used: " << removed.type
                    << " x" << removed.quantity
                    << " (" << removed.batch << ")" << std::endl;
            }
        } break;

        case 3:
            module.printAll(std::cout);
            break;

        default:
            std::cout << "[Error] Choose a valid option (0-3)." << std::endl;
            break;
        }
    }
}