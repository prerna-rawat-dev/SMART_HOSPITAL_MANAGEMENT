#include "../include/doctor.h"

#include <fstream>
#include <sstream>

namespace
{
    bool invalidField(const string& value)
    {
        return value.find('|') != string::npos ||
               value.find('\n') != string::npos ||
               value.find('\r') != string::npos;
    }

    int splitFields(
        const string& line,
        string fields[],
        int maxFields
    )
    {
        int count = 0;
        string field;

        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '|')
            {
                if (count >= maxFields)
                    return -1;

                fields[count++] = field;
                field.clear();
            }
            else
            {
                field += line[i];
            }
        }

        if (count >= maxFields)
            return -1;

        fields[count++] = field;

        return count;
    }

    bool parseDoctorNumber(
        const string& value,
        int& number
    )
    {
        stringstream input(value);

        input >> number;

        return !input.fail() && input.eof();
    }
}

// ==================================================
// DOCTOR CONSTRUCTOR
// ==================================================

Doctor::Doctor()
    : doctorID(""),
      name(""),
      username(""),
      department(""),
      specialization(""),
      phone(""),
      doctorType("Non-Emergency"),
      availability("Available"),
      next(NULL)
{
}

// ==================================================
// DOCTOR MANAGER CONSTRUCTOR
// ==================================================

DoctorManager::DoctorManager()
    : head(NULL),
      tail(NULL),
      doctorCount(0),
      nextDoctorNumber(1)
{
    loadDoctors();
}

// ==================================================
// DESTRUCTOR
// ==================================================

DoctorManager::~DoctorManager()
{
    saveDoctors();
    clearDoctors();
}

// ==================================================
// APPEND DOCTOR
// ==================================================

void DoctorManager::appendDoctor(Doctor* doctor)
{
    if (!doctor)
        return;

    doctor->next = NULL;

    if (!head)
    {
        head = tail = doctor;
    }
    else
    {
        tail->next = doctor;
        tail = doctor;
    }

    ++doctorCount;
}

// ==================================================
// CLEAR DOCTORS
// ==================================================

void DoctorManager::clearDoctors()
{
    Doctor* current = head;

    while (current)
    {
        Doctor* next = current->next;

        delete current;

        current = next;
    }

    head = NULL;
    tail = NULL;

    doctorCount = 0;
}

// ==================================================
// GENERATE DOCTOR ID
// ==================================================

string DoctorManager::generateDoctorID()
{
    string number = to_string(nextDoctorNumber++);

    while (number.size() < 5)
        number = "0" + number;

    return "DOC" + number;
}

// ==================================================
// REGISTER DOCTOR
// ==================================================

bool DoctorManager::registerDoctor(const Doctor& doctor)
{
    if (doctor.name.empty() ||
        doctor.username.empty() ||
        doctor.department.empty() ||
        doctor.specialization.empty() ||
        doctor.phone.empty())
    {
        cout << "\nPlease fill all required doctor details.\n";
        return false;
    }

    if (doctor.doctorType != "Emergency" &&
        doctor.doctorType != "Non-Emergency")
    {
        cout << "\nInvalid doctor type.\n";
        return false;
    }

    if (doctor.availability != "Available" &&
        doctor.availability != "Unavailable")
    {
        cout << "\nAvailability must be Available or Unavailable.\n";
        return false;
    }

    const string values[5] =
    {
        doctor.name,
        doctor.username,
        doctor.department,
        doctor.specialization,
        doctor.phone
    };

    for (int i = 0; i < 5; ++i)
    {
        if (invalidField(values[i]))
        {
            cout << "\nInvalid character in doctor details.\n";
            return false;
        }
    }

    for (Doctor* current = head;
         current != NULL;
         current = current->next)
    {
        if (current->username == doctor.username ||
            current->phone == doctor.phone)
        {
            cout << "\nDoctor username or phone already exists.\n";
            return false;
        }
    }

    Doctor* added = new Doctor();

    added->doctorID = generateDoctorID();
    added->name = doctor.name;
    added->username = doctor.username;
    added->department = doctor.department;
    added->specialization = doctor.specialization;
    added->phone = doctor.phone;
    added->doctorType = doctor.doctorType;
    added->availability = doctor.availability;

    appendDoctor(added);

    if (!saveDoctors())
    {
        cout << "\nDoctor was added in memory but saving doctors.txt failed.\n";
        return false;
    }

    cout << "\nDoctor registered successfully!\n";
    cout << "Doctor ID: " << added->doctorID << endl;

    return true;
}

// ==================================================
// SEARCH DOCTOR BY ID
// ==================================================

Doctor* DoctorManager::searchByID(const string& id) const
{
    for (Doctor* current = head;
         current != NULL;
         current = current->next)
    {
        if (current->doctorID == id)
            return current;
    }

    return NULL;
}

// ==================================================
// SEARCH DOCTOR BY NAME
// ==================================================

Doctor* DoctorManager::searchByName(const string& name) const
{
    for (Doctor* current = head;
         current != NULL;
         current = current->next)
    {
        if (current->name == name)
            return current;
    }

    return NULL;
}

// ==================================================
// DISPLAY DOCTOR
// ==================================================

void DoctorManager::displayDoctor(const string& id) const
{
    Doctor* doctor = searchByID(id);

    if (!doctor)
    {
        cout << "\nDoctor not found.\n";
        return;
    }

    cout << "\n========== DOCTOR DETAILS ==========\n";

    cout << "Doctor ID: " << doctor->doctorID << '\n';
    cout << "Name: " << doctor->name << '\n';
    cout << "Username: " << doctor->username << '\n';
    cout << "Department: " << doctor->department << '\n';
    cout << "Specialization: " << doctor->specialization << '\n';
    cout << "Phone: " << doctor->phone << '\n';
    cout << "Doctor Type: " << doctor->doctorType << '\n';
    cout << "Manual Availability: " << doctor->availability << '\n';

    cout << "Maximum Active Patients: "
         << MAX_PATIENTS_PER_DOCTOR << '\n';
}

// ==================================================
// DISPLAY ALL DOCTORS
// ==================================================

void DoctorManager::displayAllDoctors() const
{
    if (!head)
    {
        cout << "\nNo doctors registered.\n";
        return;
    }

    cout << "\n========== ALL DOCTORS ==========\n";

    for (Doctor* current = head;
         current != NULL;
         current = current->next)
    {
        cout << "\nDoctor ID: " << current->doctorID;
        cout << "\nName: " << current->name;
        cout << "\nDepartment: " << current->department;
        cout << "\nSpecialization: " << current->specialization;
        cout << "\nDoctor Type: " << current->doctorType;
        cout << "\nManual Availability: " << current->availability;

        cout << "\nMaximum Active Patients: "
             << MAX_PATIENTS_PER_DOCTOR;

        cout << "\n----------------------------------\n";
    }
}

// ==================================================
// DISPLAY DOCTORS BY DEPARTMENT
// ==================================================

void DoctorManager::displayDoctorsByDepartment(
    const string& department
) const
{
    bool found = false;

    cout << "\nDoctors in " << department << ":\n";

    for (Doctor* current = head;
         current != NULL;
         current = current->next)
    {
        if (current->department == department)
        {
            displayDoctor(current->doctorID);
            found = true;
        }
    }

    if (!found)
        cout << "\nNo doctors found in this department.\n";
}

// ==================================================
// UPDATE DOCTOR TYPE AND AVAILABILITY
// ==================================================

bool DoctorManager::updateDoctorTypeAndAvailability(
    const string& id,
    const string& type,
    const string& availability
)
{
    Doctor* doctor = searchByID(id);

    if (!doctor)
        return false;

    if (type != "Emergency" &&
        type != "Non-Emergency")
        return false;

    if (availability != "Available" &&
        availability != "Unavailable")
        return false;

    string oldType = doctor->doctorType;
    string oldAvailability = doctor->availability;

    doctor->doctorType = type;
    doctor->availability = availability;

    if (!saveDoctors())
    {
        doctor->doctorType = oldType;
        doctor->availability = oldAvailability;

        return false;
    }

    cout << "\nDoctor details updated successfully.\n";

    return true;
}

// ==================================================
// DISPLAY EMERGENCY DOCTORS BY DEPARTMENT
// ==================================================

void DoctorManager::displayEmergencyDoctorsByDepartment(
    const string& department
) const
{
    bool found = false;

    cout << "\n========== EMERGENCY DOCTORS ==========\n";

    for (Doctor* current = head;
         current != NULL;
         current = current->next)
    {
        if (current->department == department &&
            current->doctorType == "Emergency")
        {
            displayDoctor(current->doctorID);
            found = true;
        }
    }

    if (!found)
        cout << "\nNo emergency specialists found.\n";
}

// ==================================================
// DISPLAY ALL EMERGENCY DOCTORS
// ==================================================

void DoctorManager::displayAllEmergencyDoctors() const
{
    bool found = false;

    cout << "\n========== ALL EMERGENCY DOCTORS ==========\n";

    for (Doctor* current = head;
         current != NULL;
         current = current->next)
    {
        if (current->doctorType == "Emergency")
        {
            displayDoctor(current->doctorID);
            found = true;
        }
    }

    if (!found)
        cout << "\nNo emergency specialists registered.\n";
}

// ==================================================
// CHECK EMERGENCY DOCTOR ELIGIBILITY
// ==================================================

bool DoctorManager::isEligibleEmergencyDoctor(
    const string& id,
    const string& department,
    int activePatientCount
) const
{
    Doctor* doctor = searchByID(id);

    return doctor &&
           doctor->department == department &&
           doctor->doctorType == "Emergency" &&
           canAcceptPatient(id, activePatientCount);
}

// ==================================================
// GET DOCTOR COUNT
// ==================================================

int DoctorManager::getDoctorCount() const
{
    return doctorCount;
}

// ==================================================
// GET EFFECTIVE AVAILABILITY
// ==================================================

string DoctorManager::getEffectiveAvailability(
    const string& id,
    int activeCount
) const
{
    Doctor* doctor = searchByID(id);

    if (!doctor)
        return "Doctor Not Found";

    if (doctor->availability == "Unavailable" ||
        activeCount >= MAX_PATIENTS_PER_DOCTOR)
    {
        return "Unavailable";
    }

    return "Available";
}

// ==================================================
// CHECK DOCTOR CAPACITY
// ==================================================

bool DoctorManager::canAcceptPatient(
    const string& id,
    int activeCount
) const
{
    Doctor* doctor = searchByID(id);

    return doctor &&
           doctor->availability == "Available" &&
           activeCount < MAX_PATIENTS_PER_DOCTOR;
}

// ==================================================
// SAVE DOCTORS
// ==================================================

bool DoctorManager::saveDoctors() const
{
    ofstream file("DATA/doctors.txt");

    if (!file.is_open())
        return false;

    for (Doctor* current = head;
         current != NULL;
         current = current->next)
    {
        file << current->doctorID << '|'
             << current->name << '|'
             << current->username << '|'
             << current->department << '|'
             << current->specialization << '|'
             << current->phone << '|'
             << current->doctorType << '|'
             << current->availability << '\n';
    }

    return file.good();
}

// ==================================================
// LOAD DOCTORS
// Supports legacy 7-field, 8-field and 10-field records.
// ==================================================

bool DoctorManager::loadDoctors()
{
    ifstream file("DATA/doctors.txt");

    if (!file.is_open())
        return true;

    string line;

    int maximumID = 0;

    while (getline(file, line))
    {
        if (!line.empty() &&
            line[line.size() - 1] == '\r')
        {
            line.erase(line.size() - 1);
        }

        if (line.empty())
            continue;

        string fields[10];

        int count = splitFields(line, fields, 10);

        if (count != 7 &&
            count != 8 &&
            count != 10)
        {
            continue;
        }

        if (fields[0].empty() ||
            fields[1].empty() ||
            fields[2].empty() ||
            fields[3].empty() ||
            fields[4].empty() ||
            fields[5].empty())
        {
            continue;
        }

        bool duplicate = false;

        for (Doctor* current = head;
             current != NULL;
             current = current->next)
        {
            if (current->doctorID == fields[0] ||
                current->username == fields[2] ||
                current->phone == fields[5])
            {
                duplicate = true;
                break;
            }
        }

        if (duplicate)
            continue;

        Doctor* doctor = new Doctor();

        doctor->doctorID = fields[0];
        doctor->name = fields[1];
        doctor->username = fields[2];
        doctor->department = fields[3];
        doctor->specialization = fields[4];
        doctor->phone = fields[5];

        if (count == 10)
        {
            // Old format:
            // ID|Name|Username|Department|Specialization|Phone|
            // Type|EmergencyDuty|BackupEligible|Availability

            doctor->doctorType = fields[6];

            doctor->availability =
                (fields[9] == "Unavailable")
                ? "Unavailable"
                : "Available";
        }
        else if (count == 8)
        {
            // New format:
            // ID|Name|Username|Department|Specialization|Phone|
            // Type|Availability

            doctor->doctorType = fields[6];
            doctor->availability = fields[7];
        }
        else
        {
            // Legacy seven-field format.

            if (fields[6] == "Emergency" ||
                fields[6] == "Non-Emergency")
            {
                doctor->doctorType = fields[6];
            }
            else
            {
                doctor->doctorType = "Non-Emergency";
                doctor->availability = fields[6];
            }
        }

        if (doctor->availability != "Unavailable")
            doctor->availability = "Available";

        if (doctor->doctorType != "Emergency" &&
            doctor->doctorType != "Non-Emergency")
        {
            doctor->doctorType = "Non-Emergency";
        }

        appendDoctor(doctor);

        if (doctor->doctorID.size() > 3 &&
            doctor->doctorID.substr(0, 3) == "DOC")
        {
            int number = 0;

            if (parseDoctorNumber(
                    doctor->doctorID.substr(3),
                    number) &&
                number > maximumID)
            {
                maximumID = number;
            }
        }
    }

    nextDoctorNumber = maximumID + 1;

    return !file.bad();
}