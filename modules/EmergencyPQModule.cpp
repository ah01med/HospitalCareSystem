#include "modules/EmergencyPQModule.hpp"
#include "ds/PriorityQueue.hpp"
#include <iostream>
#include <iomanip>

// comparator — higher priority first, break ties alphabetically
struct EmergencyHigher {
    bool operator()(const EmergencyCase& a, const EmergencyCase& b) const {
        if (a.priority != b.priority)
            return a.priority > b.priority;
        return a.name < b.name;
    }
};


static PriorityQueue<EmergencyCase, EmergencyHigher> g_pq;

void EmergencyPQModule::logCase(const EmergencyCase& e) {
    if (e.priority < 1 || e.priority > 5) {
        std::cout << "[Error] Invalid priority (" << e.priority
            << "). Must be 1–5.\n";
        return;
    }
    g_pq.push(e);
    std::cout << "[OK] Logged emergency: " << e.name
        << " (" << e.type << "), priority=" << e.priority << "\n";
}

bool EmergencyPQModule::processTop(EmergencyCase& out) {
    if (!g_pq.popMax(out)) {
        std::cout << "[Info] No pending emergency cases.\n";
        return false;
    }
    std::cout << "[Processing] " << out.name
        << " — " << out.type
        << " (priority " << out.priority << ")\n";
    return true;
}

void EmergencyPQModule::printByPriority(std::ostream& os) const {
    if (g_pq.isEmpty()) {
        os << "[Info] No emergency cases recorded.\n";
        return;
    }

    PriorityQueue<EmergencyCase, EmergencyHigher> temp;
    EmergencyCase c;

    os << "\n+------------------------------------------------------+\n";
    os << "|            PENDING EMERGENCY CASES (Top First)        |\n";
    os << "+----------------------+----------------------+----------+\n";
    os << "| Name                 | Type                 | Priority |\n";
    os << "+----------------------+----------------------+----------+\n";

    while (!g_pq.isEmpty()) {
        const_cast<PriorityQueue<EmergencyCase, EmergencyHigher>&>(g_pq)
            .popMax(c);

        os << "| " << std::left << std::setw(20) << c.name
            << " | " << std::left << std::setw(20) << c.type
            << " | " << std::right << std::setw(8) << c.priority << " |\n";

        temp.push(c);
    }

    os << "+----------------------+----------------------+----------+\n";

    
    while (!temp.isEmpty()) {
        temp.popMax(c);
        const_cast<PriorityQueue<EmergencyCase, EmergencyHigher>&>(g_pq)
            .push(c);
    }
}


bool EmergencyPQModule::peekTop(EmergencyCase& out) const {
    if (!g_pq.peekMax(out)) {
        std::cout << "[Info] No pending emergency cases.\n";
        return false;
    }
    std::cout << "[Next critical] " << out.name
        << " — " << out.type
        << " (priority " << out.priority << ")\n";
    return true;
}


void EmergencyPQModule::printStats(std::ostream& os) const {
    if (g_pq.isEmpty()) {
        os << "[Info] No emergency cases recorded.\n";
        return;
    }

    PriorityQueue<EmergencyCase, EmergencyHigher> temp;
    EmergencyCase c;

    int total = 0;
    int counts[6] = { 0, 0, 0, 0, 0, 0 }; 
    int maxPriority = 1;

    
    while (!g_pq.isEmpty()) {
        const_cast<PriorityQueue<EmergencyCase, EmergencyHigher>&>(g_pq)
            .popMax(c);

        ++total;
        if (c.priority >= 1 && c.priority <= 5) {
            ++counts[c.priority];
            if (c.priority > maxPriority) maxPriority = c.priority;
        }

        temp.push(c);
    }

    
    while (!temp.isEmpty()) {
        temp.popMax(c);
        const_cast<PriorityQueue<EmergencyCase, EmergencyHigher>&>(g_pq)
            .push(c);
    }

    os << "\n[Emergency Statistics]\n";
    os << "Total pending cases : " << total << "\n";
    for (int p = 1; p <= 5; ++p) {
        os << "Priority " << p << " cases : " << counts[p] << "\n";
    }
    os << "Highest current priority: " << maxPriority << "\n";
}
