#pragma once
#include <string>

// ---- basic input helpers (already added earlier) ----
std::string readString(const char* prompt);
int         readIntInRange(const char* prompt, int min, int max);

// ---- string helpers ----
void        trimInPlace(std::string& s);
std::string trimCopy(const std::string& s);

// ---- seed loaders (your part) ----
// Return value: true if file opened and at least attempted; counts set accordingly.
// If file is missing, function returns false but DOES NOT crash; counts remain 0.

class PatientQueueModule;
class SupplyStackModule;
class EmergencyPQModule;
class AmbulanceCircularModule;

bool loadPatientsCSV(const char* path, PatientQueueModule& mod, int& loaded, int& skipped);
bool loadSuppliesCSV(const char* path, SupplyStackModule& mod, int& loaded, int& skipped);
bool loadEmergenciesCSV(const char* path, EmergencyPQModule& mod, int& loaded, int& skipped);
bool loadAmbulancesCSV(const char* path, AmbulanceCircularModule& mod, int& loaded, int& skipped);

// Convenience wrapper to load all four (logs nicely).
void loadAllSeedsIfPresent(
    const char* patientsPath,
    const char* suppliesPath,
    const char* emergenciesPath,
    const char* ambulancesPath,
    PatientQueueModule& patients,
    SupplyStackModule& supplies,
    EmergencyPQModule& emergencies,
    AmbulanceCircularModule& ambulances
);
