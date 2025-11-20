#include "core/Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cctype>

// modules
#include "modules/PatientQueueModule.hpp"
#include "modules/SupplyStackModule.hpp"
#include "modules/EmergencyPQModule.hpp"
#include "modules/AmbulanceCircularModule.hpp"

// models
#include "models/Patient.hpp"
#include "models/SupplyItem.hpp"
#include "models/EmergencyCase.hpp"
#include "models/Ambulance.hpp"

// ---------------- input helpers ----------------
std::string readString(const char* prompt) {
    if (prompt && *prompt) std::cout << prompt;
    std::string s;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, s);
    return s;
}

int readIntInRange(const char* prompt, int min, int max) {
    for (;;) {
        if (prompt && *prompt) std::cout << prompt;
        int v;
        if (std::cin >> v && v >= min && v <= max) return v;
        std::cout << "Invalid input. Enter a number between " << min << " and " << max << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// ---------------- trimming helpers ----------------
static inline bool is_space(unsigned char c) { return std::isspace(c) != 0; }

void trimInPlace(std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && is_space(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && is_space(static_cast<unsigned char>(s[b - 1]))) --b;
    if (a == 0 && b == s.size()) return;
    s.assign(s.begin() + static_cast<std::ptrdiff_t>(a), s.begin() + static_cast<std::ptrdiff_t>(b));
}

std::string trimCopy(const std::string& s) { std::string t = s; trimInPlace(t); return t; }

// ---------------- tiny CSV helpers (no <vector>) ----------------
static void stripComment(std::string& line) {
    // Allow inline comments starting with '#'
    const size_t pos = line.find('#');
    if (pos != std::string::npos) line.erase(pos);
    trimInPlace(line);
}

// Safe getline that skips empty/comment lines and returns false on EOF
static bool nextDataLine(std::ifstream& f, std::string& out) {
    while (std::getline(f, out)) {
        // remove BOM if present on first line
        if (!out.empty() && static_cast<unsigned char>(out[0]) == 0xEF) {
            if (out.size() >= 3 &&
                static_cast<unsigned char>(out[1]) == 0xBB &&
                static_cast<unsigned char>(out[2]) == 0xBF) {
                out.erase(0, 3);
            }
        }
        stripComment(out);
        if (!out.empty()) return true;
    }
    return false;
}

static bool openFile(const char* path, std::ifstream& f) {
    f.open(path);
    if (!f.is_open()) {
        std::cout << "[Seed] File not found: " << (path ? path : "(null)") << " (starting empty)\n";
        return false;
    }
    return true;
}

// Split three fields "a,b,c" into refs; returns false if malformed.
static bool split3(const std::string& line, std::string& a, std::string& b, std::string& c) {
    std::stringstream ss(line);
    if (!std::getline(ss, a, ',')) return false;
    if (!std::getline(ss, b, ',')) return false;
    if (!std::getline(ss, c, ',')) return false;
    trimInPlace(a); trimInPlace(b); trimInPlace(c);
    return !a.empty() && !b.empty() && !c.empty();
}

// ---------------- file-specific loaders ----------------
bool loadPatientsCSV(const char* path, PatientQueueModule& mod, int& loaded, int& skipped) {
    loaded = skipped = 0;
    std::ifstream f;
    if (!openFile(path, f)) return false;

    std::string line;
    // Skip header if present
    if (nextDataLine(f, line)) {
        // If the first non-empty line looks like a header, skip it
        std::string h = trimCopy(line);
        if (h.size() >= 2 && (h.find("ID") != std::string::npos || h.find("Name") != std::string::npos)) {
            // proceed with next lines
        }
        else {
            // Not a header; process this line too
            std::string id, name, cond;
            if (split3(line, id, name, cond)) {
                mod.admit(Patient{ id, name, cond });
                ++loaded;
            }
            else {
                ++skipped;
            }
        }
    }
    // Process remaining lines
    while (nextDataLine(f, line)) {
        std::string id, name, cond;
        if (split3(line, id, name, cond)) {
            mod.admit(Patient{ id, name, cond });
            ++loaded;
        }
        else {
            ++skipped;
        }
    }
    std::cout << "[Seed] Patients: loaded=" << loaded << ", skipped=" << skipped << "\n";
    return true;
}

bool loadSuppliesCSV(const char* path, SupplyStackModule& mod, int& loaded, int& skipped) {
    loaded = skipped = 0;
    std::ifstream f;
    if (!openFile(path, f)) return false;

    std::string line;
    // Skip header if present
    if (nextDataLine(f, line)) {
        std::string h = trimCopy(line);
        if (!(h.find("Type") != std::string::npos && h.find("Quantity") != std::string::npos)) {
            // Process first line
            std::string type, qtyStr, batch;
            if (split3(line, type, qtyStr, batch)) {
                int qty = 0;
                std::stringstream ss(qtyStr);
                if (ss >> qty && qty > 0) {
                    mod.add(SupplyItem{ type, qty, batch });
                    ++loaded;
                }
                else ++skipped;
            }
            else ++skipped;
        }
    }
    while (nextDataLine(f, line)) {
        std::string type, qtyStr, batch;
        if (split3(line, type, qtyStr, batch)) {
            int qty = 0;
            std::stringstream ss(qtyStr);
            if (ss >> qty && qty > 0) {
                mod.add(SupplyItem{ type, qty, batch });
                ++loaded;
            }
            else ++skipped;
        }
        else ++skipped;
    }
    std::cout << "[Seed] Supplies: loaded=" << loaded << ", skipped=" << skipped << "\n";
    return true;
}

static bool validPriorityInt(const std::string& s, int& out) {
    std::stringstream ss(s);
    int p = 0; if (!(ss >> p)) return false;
    if (p < 1 || p > 5) return false;
    out = p; return true;
}

bool loadEmergenciesCSV(const char* path, EmergencyPQModule& mod, int& loaded, int& skipped) {
    loaded = skipped = 0;
    std::ifstream f;
    if (!openFile(path, f)) return false;

    std::string line;
    // Skip header if present
    if (nextDataLine(f, line)) {
        std::string h = trimCopy(line);
        if (!(h.find("Name") != std::string::npos && h.find("Type") != std::string::npos)) {
            // Process first line
            std::string name, type, prStr;
            if (split3(line, name, type, prStr)) {
                int pr = 0;
                if (validPriorityInt(prStr, pr)) {
                    mod.logCase(EmergencyCase{ name, type, pr });
                    ++loaded;
                }
                else ++skipped;
            }
            else ++skipped;
        }
    }
    while (nextDataLine(f, line)) {
        std::string name, type, prStr;
        if (split3(line, name, type, prStr)) {
            int pr = 0;
            if (validPriorityInt(prStr, pr)) {
                mod.logCase(EmergencyCase{ name, type, pr });
                ++loaded;
            }
            else ++skipped;
        }
        else ++skipped;
    }
    std::cout << "[Seed] Emergencies: loaded=" << loaded << ", skipped=" << skipped << "\n";
    return true;
}

bool loadAmbulancesCSV(const char* path, AmbulanceCircularModule& mod, int& loaded, int& skipped) {
    loaded = skipped = 0;
    std::ifstream f;
    if (!openFile(path, f)) return false;

    std::string line;
    // Skip header if present
    if (nextDataLine(f, line)) {
        std::string h = trimCopy(line);
        if (!(h.find("Code") != std::string::npos && h.find("Driver") != std::string::npos)) {
            std::string code, driver, dummy;
            // Here file has only two columns, so custom parse:
            std::stringstream ss(line);
            if (std::getline(ss, code, ',') && std::getline(ss, driver)) {
                trimInPlace(code); trimInPlace(driver);
                if (!code.empty() && !driver.empty()) {
                    if (mod.registerAmbulance(Ambulance{ code, driver })) ++loaded;
                    else ++skipped;
                }
                else ++skipped;
            }
            else ++skipped;
        }
    }
    while (nextDataLine(f, line)) {
        std::stringstream ss(line);
        std::string code, driver;
        if (std::getline(ss, code, ',') && std::getline(ss, driver)) {
            trimInPlace(code); trimInPlace(driver);
            if (!code.empty() && !driver.empty()) {
                if (mod.registerAmbulance(Ambulance{ code, driver })) ++loaded;
                else ++skipped;
            }
            else ++skipped;
        }
        else ++skipped;
    }
    std::cout << "[Seed] Ambulances: loaded=" << loaded << ", skipped=" << skipped << "\n";
    return true;
}

// --------------- convenience wrapper ---------------
void loadAllSeedsIfPresent(
    const char* patientsPath,
    const char* suppliesPath,
    const char* emergenciesPath,
    const char* ambulancesPath,
    PatientQueueModule& patients,
    SupplyStackModule& supplies,
    EmergencyPQModule& emergencies,
    AmbulanceCircularModule& ambulances
) {
    int lp = 0, sp = 0, le = 0, se = 0, ls = 0, ss = 0, la = 0, sa = 0;

    loadPatientsCSV(patientsPath, patients, lp, sp);
    loadSuppliesCSV(suppliesPath, supplies, ls, ss);
    loadEmergenciesCSV(emergenciesPath, emergencies, le, se);
    loadAmbulancesCSV(ambulancesPath, ambulances, la, sa);

    std::cout << "[Seed Summary] "
        << "Patients(" << lp << "/" << (lp + sp) << "), "
        << "Supplies(" << ls << "/" << (ls + ss) << "), "
        << "Emergencies(" << le << "/" << (le + se) << "), "
        << "Ambulances(" << la << "/" << (la + sa) << ")\n";
}
