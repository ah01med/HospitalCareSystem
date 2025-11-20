#include <iostream>
#include <limits>
#include <fstream>
#include <sstream>
#include <string>

#include "Menu.hpp"
#include "Utils.hpp"

#include "../modules/PatientQueueModule.hpp"
#include "../modules/SupplyStackModule.hpp"
#include "../modules/EmergencyPQModule.hpp"
#include "../modules/AmbulanceCircularModule.hpp"
#include "../models/Patient.hpp"
#include "../models/SupplyItem.hpp"
#include "../models/EmergencyCase.hpp"
#include "../models/Ambulance.hpp"

using std::string;


//  Anonymous namespace for helper functions
//  (These functions are private to this file only)

namespace {

    
    bool getlineSafe(std::istream& is, string& out) {
        if (!std::getline(is, out)) return false;
        if (!out.empty() && out.back() == '\r') out.pop_back();
        return true;
    }

    // Load Patients 
    void loadPatientsFromCsv(const string& path, PatientQueueModule& module) {
        std::ifstream in(path);
        if (!in) return; 

        string line;
        getlineSafe(in, line); // skip header

        while (getlineSafe(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            Patient p;
            std::getline(ss, p.id, ',');
            std::getline(ss, p.name, ',');
            std::getline(ss, p.conditionType, ',');

            if (!p.id.empty() && !p.name.empty()) {
                module.admit(p);
            }
        }
    }

    //  Load Supplies 
    void loadSuppliesFromCsv(const string& path, SupplyStackModule& module) {
        std::ifstream in(path);
        if (!in) return;

        string line;
        getlineSafe(in, line); 

        while (getlineSafe(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);

            SupplyItem s;
            string qtyStr;

            std::getline(ss, s.type, ',');
            std::getline(ss, qtyStr, ',');
            std::getline(ss, s.batch, ',');

            if (!qtyStr.empty()) s.quantity = std::stoi(qtyStr);
            if (!s.type.empty() && s.quantity > 0) {
                module.add(s);
            }
        }
    }

    //  Load Emergencies 
    void loadEmergenciesFromCsv(const string& path, EmergencyPQModule& module) {
        std::ifstream in(path);
        if (!in) return;

        string line;
        getlineSafe(in, line); 

        while (getlineSafe(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);

            EmergencyCase e;
            string prioStr;

            std::getline(ss, e.name, ',');
            std::getline(ss, e.type, ',');
            std::getline(ss, prioStr, ',');

            if (!prioStr.empty()) e.priority = std::stoi(prioStr);

            if (!e.name.empty()) {
                module.logCase(e);
            }
        }
    }

    //  Load Ambulances 
    void loadAmbulancesFromCsv(const string& path, AmbulanceCircularModule& module) {
        std::ifstream in(path);
        if (!in) return;

        string line;
        getlineSafe(in, line); 

        while (getlineSafe(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);

            Ambulance a;
            std::getline(ss, a.code, ',');
            std::getline(ss, a.driverName, ',');

            if (!a.code.empty()) {
                module.registerAmbulance(a);
            }
        }
    }

    // Load all seed datasets
    void loadAllSeeds(PatientQueueModule& patients,
        SupplyStackModule& supplies,
        EmergencyPQModule& emergencies,
        AmbulanceCircularModule& ambulances)
    {
        loadPatientsFromCsv("data/patients_seed.csv", patients);
        loadSuppliesFromCsv("data/supplies_seed.csv", supplies);
        loadEmergenciesFromCsv("data/emergencies_seed.csv", emergencies);
        loadAmbulancesFromCsv("data/ambulances_seed.csv", ambulances);
    }

} // end anonymous namespace



// Helper: pause screen

static void pause_and_clear() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}



// MAIN MENU LOGIC

int Menu::run() {

    PatientQueueModule        patients;
    SupplyStackModule         supplies;
    EmergencyPQModule         emergencies;
    AmbulanceCircularModule   ambulances;

    // -------- LOAD SEED DATA --------
    loadAllSeeds(patients, supplies, emergencies, ambulances);

    
    //  MAIN MENU
    for (;;) {
        std::cout <<
            "\n=== Hospital Patient Care Management System ===\n"
            "1) Patient Admission\n"
            "2) Medical Supplies\n"
            "3) Emergency Cases\n"
            "4) Ambulance Dispatch\n"
            "0) Exit\n> ";

        int choice = readIntInRange("", 0, 4);
        if (choice == 0) break;

        //  Patient Admission 
        if (choice == 1) {
            for (;;) {
                std::cout << "\n[Patient Admission]\n"
                    "1) Admit patient\n"
                    "2) Discharge earliest\n"
                    "3) View queue\n"
                    "0) Back\n> ";

                int c = readIntInRange("", 0, 3);
                if (c == 0) break;

                if (c == 1) {
                    Patient p;
                    p.id = readString("ID: ");
                    p.name = readString("Name: ");
                    p.conditionType = readString("Condition: ");
                    patients.admit(p);

                }
                else if (c == 2) {
                    Patient out;
                    if (patients.discharge(out))
                        std::cout << "Discharged: " << out.name << "\n";
                    else
                        std::cout << "No patients in queue.\n";

                }
                else if (c == 3) {
                    patients.printQueue(std::cout);
                }
                pause_and_clear();
            }
        }

        //  Supplies 
        else if (choice == 2) {
            for (;;) {
                std::cout << "\n[Medical Supplies]\n"
                    "1) Add stock\n"
                    "2) Use last added\n"
                    "3) View supplies\n"
                    "0) Back\n> ";

                int c = readIntInRange("", 0, 3);
                if (c == 0) break;

                if (c == 1) {
                    SupplyItem s;
                    s.type = readString("Type: ");
                    s.quantity = readIntInRange("Quantity: ", 1, 1000000);
                    s.batch = readString("Batch: ");
                    supplies.add(s);

                }
                else if (c == 2) {
                    SupplyItem out;
                    if (supplies.useLast(out))
                        std::cout << "Used: " << out.type
                        << " x" << out.quantity
                        << " (" << out.batch << ")\n";
                    else
                        std::cout << "No supplies available.\n";

                }
                else if (c == 3) {
                    supplies.printAll(std::cout);
                }
                pause_and_clear();
            }
        }

        // Emergency Cases 
        else if (choice == 3) {
            for (;;) {
                std::cout << "\n[Emergency Cases]\n"
                    "1) Log case\n"
                    "2) Process most critical\n"
                    "3) View by priority\n"
                    "4) Peek next critical\n"
                    "5) View statistics\n"
                    "0) Back\n> ";

                int c = readIntInRange("", 0, 5);
                if (c == 0) break;

                if (c == 1) {
                    EmergencyCase e;
                    e.name = readString("Patient name: ");
                    e.type = readString("Emergency type: ");
                    e.priority = readIntInRange("Priority (1..5): ", 1, 5);
                    emergencies.logCase(e);

                }
                else if (c == 2) {
                    EmergencyCase out;
                    emergencies.processTop(out);

                }
                else if (c == 3) {
                    emergencies.printByPriority(std::cout);

                }
                else if (c == 4) {
                    EmergencyCase out;
                    emergencies.peekTop(out);

                }
                else if (c == 5) {
                    emergencies.printStats(std::cout);
                }
                pause_and_clear();
            }
        }

        // Ambulance Dispatch 
        else if (choice == 4) {
            for (;;) {
                std::cout << "\n[Ambulance Dispatch]\n"
                    "1) Register ambulance\n"
                    "2) Rotate shift\n"
                    "3) Display rotation\n"
                    "4) Check ambulance count\n"
                    "0) Back\n> ";

                int c = readIntInRange("", 0, 4);
                if (c == 0) break;

                if (c == 1) {
                    Ambulance a;
                    a.code = readString("Code: ");
                    a.driverName = readString("Driver: ");
                    ambulances.registerAmbulance(a);

                }
                else if (c == 2) {
                    ambulances.rotateOnce();

                }
                else if (c == 3) {
                    ambulances.printRotation(std::cout);

                }
                else if (c == 4) {
                    std::cout << "Total ambulances: "
                        << ambulances.getAmbulanceCount() << "\n";
                }
                pause_and_clear();
            }
        }
    }

    return 0;
}
