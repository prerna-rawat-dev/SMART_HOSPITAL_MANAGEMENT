#include "../include/nurse.h"

#include <fstream>
#include <sstream>
#include <cstdio>

using namespace std;

const char NURSE_SEPARATOR = '|';

const char* NURSE_FILE = "DATA/nurses.txt";
const char* NURSE_TEMP = "DATA/nurses_temp.txt";
const char* NURSE_BACKUP = "DATA/nurses_backup.txt";

// Check invalid characters
bool hasInvalidNurseCharacter(const string& value)
{
    return value.find(NURSE_SEPARATOR) != string::npos ||
           value.find('\n') != string::npos ||
           value.find('\r') != string::npos;
}

// Split file line into fields
int splitNurseFields(const string& line,
                     string fields[],
                     int maxFields)
{
    int count = 0;
    string current = "";

    for (int i = 0; i < (int)line.length(); i++)
    {
        if (line[i] == NURSE_SEPARATOR)
        {
            if (count >= maxFields)
                return -1;

            fields[count++] = current;
            current = "";
        }
        else
        {
            current += line[i];
        }
    }

    if (count >= maxFields)
        return -1;

    fields[count++] = current;

    return count;
}

// Read integer safely
bool readNurseNumber(const string& value, int& number)
{
    stringstream ss(value);

    ss >> number;

    return !ss.fail() && ss.eof() && number >= 0;
}

// Safely replace original file
bool safelyReplaceNurseFile()
{
    remove(NURSE_BACKUP);

    bool originalExists = false;

    ifstream check(NURSE_FILE);

    if (check.is_open())
    {
        originalExists = true;
        check.close();
    }

    if (originalExists)
    {
        if (rename(NURSE_FILE, NURSE_BACKUP) != 0)
            return false;
    }

    if (rename(NURSE_TEMP, NURSE_FILE) != 0)
    {
        if (originalExists)
            rename(NURSE_BACKUP, NURSE_FILE);

        return false;
    }

    remove(NURSE_BACKUP);

    return true;
}

// --------------------------------------------------
// Nurse constructor
// --------------------------------------------------

Nurse::Nurse()
{
    nurseID = "";
    name = "";
    username = "";
    department = "";
    assignedDoctorID = "";
    assignedDoctorName = "";
    phone = "";
    availability = "Available";

    next = NULL;
}

// --------------------------------------------------
// NurseManager constructor
// --------------------------------------------------

NurseManager::NurseManager()
{
    head = NULL;
    tail = NULL;

    nurseCount = 0;
    nextNurseNumber = 1;

    storageReady = false;

    // Create file if it does not exist
    ofstream createFile(NURSE_FILE, ios::app);

    if (!createFile.is_open())
    {
        cout << "\nUnable to access nurse data file.\n";
        return;
    }

    createFile.close();

    if (loadNurses())
    {
        storageReady = true;
    }
    else
    {
        cout << "\nWarning: Nurse data could not be loaded safely.\n";
        cout << "Nurse records will not be modified.\n";
    }
}

// --------------------------------------------------
// NurseManager destructor
// --------------------------------------------------

NurseManager::~NurseManager()
{
    if (storageReady)
        saveNurses();

    clearNurses();
}

// --------------------------------------------------
// Append nurse to linked list
// --------------------------------------------------

void NurseManager::appendNurse(Nurse* nurse)
{
    if (nurse == NULL)
        return;

    nurse->next = NULL;

    if (head == NULL)
    {
        head = tail = nurse;
    }
    else
    {
        tail->next = nurse;
        tail = nurse;
    }

    nurseCount++;
}

// --------------------------------------------------
// Clear linked list
// --------------------------------------------------

void NurseManager::clearNurses()
{
    Nurse* current = head;

    while (current != NULL)
    {
        Nurse* nextNurse = current->next;

        delete current;

        current = nextNurse;
    }

    head = NULL;
    tail = NULL;

    nurseCount = 0;
}

// --------------------------------------------------
// Generate unique nurse ID
// --------------------------------------------------

string NurseManager::generateNurseID()
{
    string number = to_string(nextNurseNumber++);

    while (number.length() < 5)
        number = "0" + number;

    return "NUR" + number;
}

// --------------------------------------------------
// Register a new nurse
// --------------------------------------------------

bool NurseManager::registerNurse(const Nurse& nurse)
{
    if (!storageReady)
    {
        cout << "\nNurse storage is unavailable.\n";
        return false;
    }

    if (nurse.name.empty() ||
        nurse.username.empty() ||
        nurse.department.empty() ||
        nurse.assignedDoctorID.empty() ||
        nurse.assignedDoctorName.empty() ||
        nurse.phone.empty())
    {
        cout << "\nPlease fill all nurse details.\n";
        return false;
    }

    const string fields[] =
    {
        nurse.name,
        nurse.username,
        nurse.department,
        nurse.assignedDoctorID,
        nurse.assignedDoctorName,
        nurse.phone,
        nurse.availability
    };

    for (int i = 0; i < 7; i++)
    {
        if (hasInvalidNurseCharacter(fields[i]))
        {
            cout << "\nInvalid character in nurse details.\n";
            return false;
        }
    }

    if (nurse.availability != "Available" &&
        nurse.availability != "Busy" &&
        nurse.availability != "On Leave")
    {
        cout << "\nInvalid nurse availability.\n";
        return false;
    }

    Nurse* current = head;

    while (current != NULL)
    {
        if (current->username == nurse.username)
        {
            cout << "\nUsername already exists.\n";
            return false;
        }

        if (current->phone == nurse.phone)
        {
            cout << "\nPhone number already registered.\n";
            return false;
        }

        current = current->next;
    }

    int oldNumber = nextNurseNumber;

    Nurse* newNurse = new Nurse();

    newNurse->nurseID = generateNurseID();
    newNurse->name = nurse.name;
    newNurse->username = nurse.username;
    newNurse->department = nurse.department;
    newNurse->assignedDoctorID = nurse.assignedDoctorID;
    newNurse->assignedDoctorName = nurse.assignedDoctorName;
    newNurse->phone = nurse.phone;
    newNurse->availability = nurse.availability;

    appendNurse(newNurse);

    if (!saveNurses())
    {
        // Roll back registration
        if (head == newNurse)
        {
            head = tail = NULL;
        }
        else
        {
            Nurse* previous = head;

            while (previous->next != newNurse)
                previous = previous->next;

            previous->next = NULL;
            tail = previous;
        }

        delete newNurse;

        nurseCount--;
        nextNurseNumber = oldNumber;

        cout << "\nWarning: Nurse registration could not be saved.\n";

        return false;
    }

    cout << "\nNurse registered successfully!\n";
    cout << "Nurse ID: " << newNurse->nurseID << '\n';

    return true;
}

// --------------------------------------------------
// Search nurse by ID
// --------------------------------------------------

Nurse* NurseManager::searchByID(const string& nurseID) const
{
    Nurse* current = head;

    while (current != NULL)
    {
        if (current->nurseID == nurseID)
            return current;

        current = current->next;
    }

    return NULL;
}

// --------------------------------------------------
// Search nurse by name
// --------------------------------------------------

Nurse* NurseManager::searchByName(const string& name) const
{
    Nurse* current = head;

    while (current != NULL)
    {
        if (current->name == name)
            return current;

        current = current->next;
    }

    return NULL;
}

// --------------------------------------------------
// Display one nurse
// --------------------------------------------------

void NurseManager::displayNurse(const string& nurseID) const
{
    Nurse* nurse = searchByID(nurseID);

    if (nurse == NULL)
    {
        cout << "\nNurse not found.\n";
        return;
    }

    cout << "\n========== NURSE DETAILS ==========\n";

    cout << "Nurse ID: " << nurse->nurseID << '\n';
    cout << "Name: " << nurse->name << '\n';
    cout << "Username: " << nurse->username << '\n';
    cout << "Department: " << nurse->department << '\n';

    cout << "Assigned Doctor ID: "
         << nurse->assignedDoctorID << '\n';

    cout << "Assigned Doctor: "
         << nurse->assignedDoctorName << '\n';

    cout << "Phone: " << nurse->phone << '\n';
    cout << "Availability: " << nurse->availability << '\n';
}

// --------------------------------------------------
// Display all nurses
// --------------------------------------------------

void NurseManager::displayAllNurses() const
{
    if (head == NULL)
    {
        cout << "\nNo nurses registered.\n";
        return;
    }

    Nurse* current = head;

    cout << "\n========== ALL NURSES ==========\n";

    while (current != NULL)
    {
        cout << "\nNurse ID: " << current->nurseID;
        cout << "\nName: " << current->name;
        cout << "\nDepartment: " << current->department;

        cout << "\nAssigned Doctor: "
             << current->assignedDoctorName;

        cout << "\nPhone: " << current->phone;

        cout << "\nAvailability: "
             << current->availability;

        cout << "\n----------------------------------\n";

        current = current->next;
    }
}

// --------------------------------------------------
// Display nurses by department
// --------------------------------------------------

void NurseManager::displayNursesByDepartment(
    const string& department) const
{
    Nurse* current = head;

    bool found = false;

    cout << "\nNurses in " << department << ":\n";

    while (current != NULL)
    {
        if (current->department == department)
        {
            cout << "\nNurse ID: " << current->nurseID;
            cout << "\nName: " << current->name;

            cout << "\nAssigned Doctor: "
                 << current->assignedDoctorName;

            cout << "\nAvailability: "
                 << current->availability;

            cout << "\n-----------------------------\n";

            found = true;
        }

        current = current->next;
    }

    if (!found)
        cout << "\nNo nurses found in this department.\n";
}

// --------------------------------------------------
// Display nurses assigned to a doctor
// --------------------------------------------------

void NurseManager::displayNursesByDoctor(
    const string& doctorID) const
{
    Nurse* current = head;

    bool found = false;

    cout << "\nNurses assigned to doctor "
         << doctorID << ":\n";

    while (current != NULL)
    {
        if (current->assignedDoctorID == doctorID)
        {
            cout << "\nNurse ID: " << current->nurseID;
            cout << "\nName: " << current->name;
            cout << "\nDepartment: " << current->department;
            cout << "\nPhone: " << current->phone;

            cout << "\n-----------------------------\n";

            found = true;
        }

        current = current->next;
    }

    if (!found)
        cout << "\nNo nurses assigned to this doctor.\n";
}

// --------------------------------------------------
// Get nurse count
// --------------------------------------------------

int NurseManager::getNurseCount() const
{
    return nurseCount;
}

// --------------------------------------------------
// Save nurses to file
// --------------------------------------------------

bool NurseManager::saveNurses() const
{
    if (!storageReady)
        return false;

    ofstream file(NURSE_TEMP);

    if (!file.is_open())
        return false;

    Nurse* current = head;

    while (current != NULL)
    {
        file << current->nurseID << NURSE_SEPARATOR
             << current->name << NURSE_SEPARATOR
             << current->username << NURSE_SEPARATOR
             << current->department << NURSE_SEPARATOR
             << current->assignedDoctorID << NURSE_SEPARATOR
             << current->assignedDoctorName << NURSE_SEPARATOR
             << current->phone << NURSE_SEPARATOR
             << current->availability << '\n';

        if (!file.good())
        {
            file.close();
            remove(NURSE_TEMP);
            return false;
        }

        current = current->next;
    }

    file.close();

    if (!file.good())
    {
        remove(NURSE_TEMP);
        return false;
    }

    if (!safelyReplaceNurseFile())
    {
        remove(NURSE_TEMP);
        return false;
    }

    return true;
}

// --------------------------------------------------
// Load nurses from file
// --------------------------------------------------

bool NurseManager::loadNurses()
{
    ifstream file(NURSE_FILE);

    if (!file.is_open())
        return false;

    string line;

    int maximumID = 0;
    int lineNumber = 0;

    while (getline(file, line))
    {
        // Handle Windows CRLF files: getline removes \n but leaves \r.
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);

        lineNumber++;

        if (line.empty())
            continue;

        string fields[8];

        if (splitNurseFields(line, fields, 8) != 8)
        {
            cout << "\nInvalid nurse record at line "
                 << lineNumber << ".\n";

            file.close();
            return false;
        }

        for (int i = 0; i < 8; i++)
        {
            if (fields[i].empty())
            {
                cout << "\nEmpty field in nurse record at line "
                     << lineNumber << ".\n";

                file.close();
                return false;
            }

            if (hasInvalidNurseCharacter(fields[i]))
            {
                file.close();
                return false;
            }
        }

        if (fields[7] != "Available" &&
            fields[7] != "Busy" &&
            fields[7] != "On Leave")
        {
            cout << "\nInvalid availability at line "
                 << lineNumber << ".\n";

            file.close();
            return false;
        }

        Nurse* current = head;

        while (current != NULL)
        {
            if (current->nurseID == fields[0] ||
                current->username == fields[2] ||
                current->phone == fields[6])
            {
                cout << "\nDuplicate nurse record at line "
                     << lineNumber << ".\n";

                file.close();
                return false;
            }

            current = current->next;
        }

        if (fields[0].length() != 8 ||
            fields[0].substr(0, 3) != "NUR")
        {
            cout << "\nInvalid Nurse ID at line "
                 << lineNumber << ".\n";

            file.close();
            return false;
        }

        int idNumber;

        if (!readNurseNumber(fields[0].substr(3), idNumber))
        {
            file.close();
            return false;
        }

        Nurse* nurse = new Nurse();

        nurse->nurseID = fields[0];
        nurse->name = fields[1];
        nurse->username = fields[2];
        nurse->department = fields[3];
        nurse->assignedDoctorID = fields[4];
        nurse->assignedDoctorName = fields[5];
        nurse->phone = fields[6];
        nurse->availability = fields[7];

        appendNurse(nurse);

        if (idNumber > maximumID)
            maximumID = idNumber;
    }

    if (file.bad())
    {
        file.close();
        return false;
    }

    file.close();

    nextNurseNumber = maximumID + 1;

    return true;
}