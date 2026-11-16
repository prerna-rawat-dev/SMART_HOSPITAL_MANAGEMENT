#include "../include/patientmanager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

using namespace std;

// ==================================================
// HELPER FUNCTIONS
// ==================================================

const char DELIMITER = '|';

bool containsInvalidCharacter(const string& value)
{
    return value.find(DELIMITER) != string::npos ||
           value.find('\n') != string::npos ||
           value.find('\r') != string::npos;
}

int splitFields(const string& line, string fields[], int maxFields)
{
    int count = 0;
    string current = "";

    for (int i = 0; i < (int)line.length(); i++)
    {
        if (line[i] == DELIMITER)
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

bool readNumber(const string& value, double& number)
{
    stringstream ss(value);
    ss >> number;

    return !ss.fail() && ss.eof();
}

bool readInteger(const string& value, int& number)
{
    stringstream ss(value);
    ss >> number;

    return !ss.fail() && ss.eof();
}

// ==================================================
// WRITE VITAL RECORD
// ==================================================

void writeVital(
    ofstream& file,
    const string& patientID,
    const VitalRecord& record)
{
    file << patientID << DELIMITER
         << record.date << DELIMITER
         << record.systolicBP << DELIMITER
         << record.diastolicBP << DELIMITER
         << record.bloodSugar << DELIMITER
         << record.temperature << DELIMITER
         << record.heartRate << DELIMITER
         << record.oxygenLevel << DELIMITER
         << record.height << DELIMITER
         << record.weight << DELIMITER
         << record.BMI << DELIMITER
         << record.abnormalFlags << DELIMITER
         << record.nurseID << '\n';
}

// ==================================================
// WRITE MEDICAL RECORD
// ==================================================

void writeMedical(
    ofstream& file,
    const string& patientID,
    const MedicalRecord& record)
{
    file << patientID << DELIMITER
         << record.date << DELIMITER
         << record.symptoms << DELIMITER
         << record.diagnosis << DELIMITER
         << record.clinicalNotes << DELIMITER
         << record.treatment << DELIMITER
         << record.doctorID << DELIMITER
         << record.doctorName << '\n';
}

// ==================================================
// SAFE FILE REPLACEMENT
// ==================================================

bool safelyReplaceFile(
    const char* temporaryFile,
    const char* originalFile,
    const char* backupFile)
{
    ifstream checkFile(originalFile);

    bool originalExists = checkFile.good();

    checkFile.close();

    // Remove previous backup.
    remove(backupFile);

    // Preserve original file.
    if (originalExists)
    {
        if (rename(originalFile, backupFile) != 0)
        {
            cout << "\nCould not create backup of "
                 << originalFile << '\n';

            return false;
        }
    }

    // Replace original with temporary file.
    if (rename(temporaryFile, originalFile) != 0)
    {
        cout << "\nCould not replace "
             << originalFile << '\n';

        if (originalExists)
            rename(backupFile, originalFile);

        return false;
    }

    if (originalExists)
        remove(backupFile);

    return true;
}

// ==================================================
// PATIENT NODE CONSTRUCTOR
// ==================================================

PatientNode::PatientNode(Patient* patient)
{
    data = patient;
    next = NULL;
    prev = NULL;
}

// ==================================================
// PATIENT MANAGER CONSTRUCTOR
// ==================================================

PatientManager::PatientManager()
{
    head = NULL;
    tail = NULL;

    patientCount = 0;
    nextPatientNumber = 1;

    storageReady = false;

    bool patientsLoaded = loadPatients();
    bool vitalsLoaded = false;
    bool medicalLoaded = false;

    if (patientsLoaded)
        vitalsLoaded = loadVitalHistory();

    if (patientsLoaded && vitalsLoaded)
        medicalLoaded = loadMedicalHistory();

    storageReady =
        patientsLoaded &&
        vitalsLoaded &&
        medicalLoaded;

    rebuildHashTables();

    if (!storageReady)
    {
        cout << "\nWARNING: Patient data could not be loaded safely.\n";
        cout << "Automatic saving has been disabled to protect existing records.\n";
    }
}

// ==================================================
// PATIENT MANAGER DESTRUCTOR
// ==================================================

PatientManager::~PatientManager()
{
    if (storageReady)
        saveAllData();

    clearPatients();
}

// ==================================================
// CLEAR PATIENTS
// ==================================================

void PatientManager::clearPatients()
{
    PatientNode* current = head;

    while (current != NULL)
    {
        PatientNode* nextNode = current->next;

        delete current->data;
        delete current;

        current = nextNode;
    }

    head = NULL;
    tail = NULL;

    patientCount = 0;

    patientIDTable.clear();
    phoneTable.clear();
}

// ==================================================
// APPEND PATIENT
// ==================================================

void PatientManager::appendPatient(Patient* patient)
{
    if (patient == NULL)
        return;

    PatientNode* newNode = new PatientNode(patient);

    if (head == NULL)
    {
        head = tail = newNode;
    }
    else
    {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }

    patientCount++;
}

// ==================================================
// GENERATE PATIENT ID
// ==================================================

string PatientManager::generatePatientID()
{
    string number = to_string(nextPatientNumber++);

    while (number.length() < 5)
        number = "0" + number;

    return "P" + number;
}

// ==================================================
// VALIDATE PATIENT
// ==================================================

bool PatientManager::validatePatient(const Patient& patient) const
{
    if (patient.name.empty() ||
        patient.phone.empty() ||
        patient.age < 0 ||
        patient.age > 120)
    {
        return false;
    }

    const string fields[] =
    {
        patient.patientID,
        patient.name,
        patient.gender,
        patient.phone,
        patient.address,
        patient.emergencyContactName,
        patient.emergencyContactPhone,
        patient.emergencyContactRelation,
        patient.bloodGroup,
        patient.allergies,
        patient.existingConditions,
        patient.currentMedicines,
        patient.initialSymptoms,
        patient.department,
        patient.doctorID,
        patient.doctorName,
        patient.status,
        patient.emergencyID,
        patient.emergencySeverity,
        patient.admissionDate,
        patient.dischargeDate,
        patient.roomID,
        patient.bedID
    };

    int count = sizeof(fields) / sizeof(fields[0]);

    for (int i = 0; i < count; i++)
    {
        if (containsInvalidCharacter(fields[i]))
            return false;
    }

    return true;
}

// ==================================================
// REBUILD HASH TABLES
// ==================================================

void PatientManager::rebuildHashTables()
{
    patientIDTable.clear();
    phoneTable.clear();

    PatientNode* current = head;

    while (current != NULL)
    {
        if (current->data != NULL)
        {
            patientIDTable.insert(
                current->data->patientID,
                current->data
            );

            phoneTable.insert(
                current->data->phone,
                current->data
            );
        }

        current = current->next;
    }
}

// ==================================================
// REGISTER PATIENT
// ==================================================

bool PatientManager::registerPatient(Patient& patient)
{
    if (!storageReady)
    {
        cout << "\nPatient storage is not ready. Registration cancelled.\n";
        return false;
    }

    bool generatedID = false;

    if (patient.patientID.empty())
    {
        patient.patientID = generatePatientID();
        generatedID = true;
    }

    if (!validatePatient(patient))
    {
        cout << "\nInvalid patient information.\n";
        return false;
    }

    if (searchByID(patient.patientID) != NULL)
    {
        cout << "\nPatient ID already exists.\n";
        return false;
    }

    if (searchByPhone(patient.phone) != NULL)
    {
        cout << "\nPhone number already registered.\n";
        return false;
    }

    Patient* newPatient = new Patient(patient);

    appendPatient(newPatient);

    rebuildHashTables();

    if (!saveAllData())
    {
        PatientNode* added = tail;

        if (added != NULL && added->data == newPatient)
        {
            tail = added->prev;

            if (tail != NULL)
                tail->next = NULL;
            else
                head = NULL;

            delete added->data;
            delete added;

            patientCount--;

            rebuildHashTables();
        }

        if (generatedID)
        {
            int usedNumber = 0;

            if (readInteger(
                    patient.patientID.substr(1),
                    usedNumber))
            {
                if (usedNumber + 1 == nextPatientNumber)
                    nextPatientNumber = usedNumber;
            }

            patient.patientID = "";
        }

        cout << "\nPatient registration failed during file saving.\n";
        cout << "Registration rolled back in memory.\n";

        return false;
    }

    cout << "\nPatient registered successfully.\n";
    cout << "Patient ID: " << patient.patientID << '\n';

    return true;
}

// ==================================================
// SEARCH BY PATIENT ID
// ==================================================

Patient* PatientManager::searchByID(
    const string& patientID) const
{
    return patientIDTable.search(patientID);
}

// ==================================================
// SEARCH BY PHONE
// ==================================================

Patient* PatientManager::searchByPhone(
    const string& phone) const
{
    return phoneTable.search(phone);
}

// ==================================================
// SEARCH BY NAME
// ==================================================

Patient* PatientManager::searchByName(
    const string& name) const
{
    PatientNode* current = head;

    while (current != NULL)
    {
        if (current->data->name == name)
            return current->data;

        current = current->next;
    }

    return NULL;
}

// ==================================================
// DISPLAY ONE PATIENT
// ==================================================

void PatientManager::displayPatient(
    const string& patientID) const
{
    Patient* patient = searchByID(patientID);

    if (patient == NULL)
    {
        cout << "\nPatient not found.\n";
        return;
    }

    patient->displayBasicInfo();
}

// ==================================================
// DISPLAY ALL PATIENTS
// ==================================================

void PatientManager::displayAllPatients() const
{
    if (head == NULL)
    {
        cout << "\nNo patients registered.\n";
        return;
    }

    PatientNode* current = head;

    cout << "\n========== ALL PATIENTS ==========\n";

    while (current != NULL)
    {
        current->data->displayBasicInfo();

        cout << "----------------------------------\n";

        current = current->next;
    }
}

// ==================================================
// UPDATE PATIENT
// ==================================================

bool PatientManager::updatePatient(
    const string& patientID)
{
    if (!storageReady)
    {
        cout << "\nPatient storage is not ready. Update cancelled.\n";
        return false;
    }

    Patient* patient = searchByID(patientID);

    if (patient == NULL)
    {
        cout << "\nPatient not found.\n";
        return false;
    }

    string newName;
    string newPhone;
    string newAddress;
    string newGender;

    string newEmergencyName;
    string newEmergencyPhone;
    string newEmergencyRelation;

    string input;

    int newAge;

    cout << "\nEnter new name: ";
    getline(cin, newName);

    cout << "Enter new age: ";
    getline(cin, input);

    if (!readInteger(input, newAge) ||
        newAge < 0 || newAge > 120)
    {
        cout << "\nInvalid age.\n";
        return false;
    }

    cout << "Enter new gender: ";
    getline(cin, newGender);

    cout << "Enter new phone: ";
    getline(cin, newPhone);

    cout << "Enter new address: ";
    getline(cin, newAddress);

    cout << "Enter emergency contact name: ";
    getline(cin, newEmergencyName);

    cout << "Enter emergency contact phone: ";
    getline(cin, newEmergencyPhone);

    cout << "Enter emergency contact relation: ";
    getline(cin, newEmergencyRelation);

    Patient* duplicate = searchByPhone(newPhone);

    if (duplicate != NULL && duplicate != patient)
    {
        cout << "\nThis phone number belongs to another patient.\n";
        return false;
    }

    // Keep a deep copy of the original patient.
    Patient oldPatient = *patient;

    Patient updatedPatient = *patient;

    updatedPatient.name = newName;
    updatedPatient.age = newAge;
    updatedPatient.gender = newGender;
    updatedPatient.phone = newPhone;
    updatedPatient.address = newAddress;

    updatedPatient.emergencyContactName = newEmergencyName;
    updatedPatient.emergencyContactPhone = newEmergencyPhone;
    updatedPatient.emergencyContactRelation = newEmergencyRelation;

    if (!validatePatient(updatedPatient))
    {
        cout << "\nInvalid information. Update cancelled.\n";
        return false;
    }

    *patient = updatedPatient;

    rebuildHashTables();

    if (!saveAllData())
    {
        *patient = oldPatient;

        rebuildHashTables();

        cout << "\nUpdate could not be saved.\n";
        cout << "Original patient information restored.\n";

        return false;
    }

    cout << "\nPatient information updated successfully.\n";

    return true;
}

// ==================================================
// DELETE PATIENT
// ==================================================

bool PatientManager::deletePatient(
    const string& patientID)
{
    if (!storageReady)
    {
        cout << "\nPatient storage is not ready. Deletion cancelled.\n";
        return false;
    }

    PatientNode* current = head;

    while (current != NULL)
    {
        if (current->data->patientID == patientID)
            break;

        current = current->next;
    }

    if (current == NULL)
    {
        cout << "\nPatient not found.\n";
        return false;
    }

    // Keep the patient and node until saving succeeds.
    PatientNode* previous = current->prev;
    PatientNode* nextNode = current->next;

    if (previous != NULL)
        previous->next = nextNode;
    else
        head = nextNode;

    if (nextNode != NULL)
        nextNode->prev = previous;
    else
        tail = previous;

    current->prev = NULL;
    current->next = NULL;

    patientCount--;

    rebuildHashTables();

    if (!saveAllData())
    {
        // Restore the node to its original position.
        current->prev = previous;
        current->next = nextNode;

        if (previous != NULL)
            previous->next = current;
        else
            head = current;

        if (nextNode != NULL)
            nextNode->prev = current;
        else
            tail = current;

        patientCount++;

        rebuildHashTables();

        cout << "\nDeletion could not be saved.\n";
        cout << "Patient record restored.\n";

        return false;
    }

    delete current->data;
    delete current;

    cout << "\nPatient deleted successfully.\n";

    return true;
}

// ==================================================
// MERGE SORT - SPLIT LIST
// ==================================================

PatientNode* PatientManager::splitList(
    PatientNode* first)
{
    if (first == NULL || first->next == NULL)
        return NULL;

    PatientNode* slow = first;
    PatientNode* fast = first->next;

    while (fast != NULL && fast->next != NULL)
    {
        slow = slow->next;
        fast = fast->next->next;
    }

    PatientNode* second = slow->next;

    slow->next = NULL;

    if (second != NULL)
        second->prev = NULL;

    return second;
}

// ==================================================
// MERGE TWO SORTED LISTS
// ==================================================

PatientNode* PatientManager::merge(
    PatientNode* left,
    PatientNode* right)
{
    if (left == NULL)
        return right;

    if (right == NULL)
        return left;

    PatientNode* result = NULL;

    if (left->data->age <= right->data->age)
    {
        result = left;
        result->next = merge(left->next, right);
    }
    else
    {
        result = right;
        result->next = merge(left, right->next);
    }

    if (result->next != NULL)
        result->next->prev = result;

    result->prev = NULL;

    return result;
}

// ==================================================
// RECURSIVE MERGE SORT
// ==================================================

PatientNode* PatientManager::mergeSort(
    PatientNode* first)
{
    if (first == NULL || first->next == NULL)
        return first;

    PatientNode* second = splitList(first);

    first = mergeSort(first);
    second = mergeSort(second);

    return merge(first, second);
}

// ==================================================
// SORT PATIENTS BY AGE
// ==================================================

void PatientManager::sortPatientsByAge()
{
    if (head == NULL || head->next == NULL)
    {
        cout << "\nNot enough patients to sort.\n";
        return;
    }

    head = mergeSort(head);

    PatientNode* current = head;

    tail = NULL;

    while (current != NULL)
    {
        tail = current;
        current = current->next;
    }

    rebuildHashTables();

    cout << "\nPatients sorted by age successfully.\n";

    displayAllPatients();
}

// ==================================================
// PATIENT COUNT
// ==================================================

int PatientManager::getPatientCount() const
{
    return patientCount;
}

// ==================================================
// CHECK EMPTY
// ==================================================

bool PatientManager::isEmpty() const
{
    return head == NULL;
}

// ==================================================
// SAVE PATIENT BASIC INFORMATION
// ==================================================

bool PatientManager::savePatients() const
{
    const char* TEMP_FILE = "DATA/patients.tmp";
    const char* BACKUP_FILE = "DATA/patients.bak";

    ofstream file(TEMP_FILE);

    if (!file.is_open())
        return false;

    PatientNode* current = head;

    while (current != NULL)
    {
        Patient* p = current->data;

        if (p == NULL)
        {
            file.close();
            remove(TEMP_FILE);
            return false;
        }

        if (!validatePatient(*p))
        {
            file.close();
            remove(TEMP_FILE);
            return false;
        }

        file << p->patientID << DELIMITER
             << p->name << DELIMITER
             << p->age << DELIMITER
             << p->gender << DELIMITER
             << p->phone << DELIMITER
             << p->address << DELIMITER
             << p->emergencyContactName << DELIMITER
             << p->emergencyContactPhone << DELIMITER
             << p->emergencyContactRelation << DELIMITER
             << p->bloodGroup << DELIMITER
             << p->allergies << DELIMITER
             << p->existingConditions << DELIMITER
             << p->currentMedicines << DELIMITER
             << p->initialSymptoms << DELIMITER
             << p->department << DELIMITER
             << p->doctorID << DELIMITER
             << p->doctorName << DELIMITER
             << p->status << DELIMITER
             << p->isEmergency << DELIMITER
             << p->emergencyID << DELIMITER
             << p->emergencySeverity << DELIMITER
             << p->isAdmitted << DELIMITER
             << p->admissionDate << DELIMITER
             << p->dischargeDate << DELIMITER
             << p->roomID << DELIMITER
             << p->bedID << '\n';

        if (!file.good())
        {
            file.close();
            remove(TEMP_FILE);
            return false;
        }

        current = current->next;
    }

    file.close();

    if (!file.good())
    {
        remove(TEMP_FILE);
        return false;
    }

    return safelyReplaceFile(
        TEMP_FILE,
        "DATA/patients.txt",
        BACKUP_FILE
    );
}

// ==================================================
// LOAD PATIENT BASIC INFORMATION
// ==================================================

bool PatientManager::loadPatients()
{
    ifstream file("DATA/patients.txt");

    // Missing file means there are currently no patients.
    if (!file.is_open())
        return true;

    string line;

    int maximumID = 0;

    while (getline(file, line))
    {
        // Handle Windows CRLF files: getline removes \n but leaves \r.
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);

        string fields[26];

        if (splitFields(line, fields, 26) != 26)
        {
            cout << "\nInvalid patient record found in patients.txt.\n";
            return false;
        }

        Patient* p = new Patient();

        p->patientID = fields[0];
        p->name = fields[1];

        if (!readInteger(fields[2], p->age))
        {
            delete p;
            return false;
        }

        p->gender = fields[3];
        p->phone = fields[4];
        p->address = fields[5];

        p->emergencyContactName = fields[6];
        p->emergencyContactPhone = fields[7];
        p->emergencyContactRelation = fields[8];

        p->bloodGroup = fields[9];
        p->allergies = fields[10];
        p->existingConditions = fields[11];
        p->currentMedicines = fields[12];
        p->initialSymptoms = fields[13];

        p->department = fields[14];
        p->doctorID = fields[15];
        p->doctorName = fields[16];

        p->status = fields[17];

        int emergencyValue;
        int admittedValue;

        if (!readInteger(fields[18], emergencyValue) ||
            !readInteger(fields[21], admittedValue))
        {
            delete p;
            return false;
        }

        p->isEmergency = (emergencyValue != 0);
        p->emergencyID = fields[19];
        p->emergencySeverity = fields[20];

        p->isAdmitted = (admittedValue != 0);

        p->admissionDate = fields[22];
        p->dischargeDate = fields[23];
        p->roomID = fields[24];
        p->bedID = fields[25];

        if (!validatePatient(*p))
        {
            delete p;
            return false;
        }

        if (searchByID(p->patientID) != NULL ||
            searchByPhone(p->phone) != NULL)
        {
            delete p;
            return false;
        }

        appendPatient(p);

        if (p->patientID.length() > 1 &&
            p->patientID[0] == 'P')
        {
            int idNumber;

            if (readInteger(p->patientID.substr(1), idNumber) &&
                idNumber > maximumID)
            {
                maximumID = idNumber;
            }
        }
    }

    if (file.bad())
        return false;

    nextPatientNumber = maximumID + 1;

    rebuildHashTables();

    return true;
}

// ==================================================
// SAVE VITAL HISTORY
// ==================================================

bool PatientManager::saveVitalHistory() const
{
    const char* TEMP_FILE = "DATA/vitals.tmp";
    const char* BACKUP_FILE = "DATA/vitals.bak";

    ofstream file(TEMP_FILE);

    if (!file.is_open())
        return false;

    PatientNode* current = head;

    while (current != NULL)
    {
        if (current->data == NULL)
        {
            file.close();
            remove(TEMP_FILE);
            return false;
        }

        const VitalNode* vital =
            current->data->getVitalHead();

        while (vital != NULL)
        {
            const VitalRecord& r = vital->data;

            if (containsInvalidCharacter(r.date) ||
                containsInvalidCharacter(r.abnormalFlags) ||
                containsInvalidCharacter(r.nurseID))
            {
                cout << "Invalid character in vital record for patient "
                     << current->data->patientID << ".\n";
                file.close();
                remove(TEMP_FILE);
                return false;
            }

            writeVital(file, current->data->patientID, r);

            if (!file.good())
            {
                file.close();
                remove(TEMP_FILE);
                return false;
            }

            vital = vital->next;
        }

        current = current->next;
    }

    file.close();

    if (!file.good())
    {
        cout << "Vital temporary file write/close failed.\n";
        remove(TEMP_FILE);
        return false;
    }

    bool replaced = safelyReplaceFile(
        TEMP_FILE,
        "DATA/vitals.txt",
        BACKUP_FILE
    );
    if (!replaced)
        cout << "Could not replace DATA/vitals.txt from temporary file.\n";
    return replaced;
}

// ==================================================
// LOAD VITAL HISTORY
// ==================================================

bool PatientManager::loadVitalHistory()
{
    ifstream file("DATA/vitals.txt");

    if (!file.is_open())
        return true;

    string line;

    while (getline(file, line))
    {
        // Remove the carriage return from Windows CRLF files.
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);

        string fields[13];

        if (splitFields(line, fields, 13) != 13)
            return false;

        Patient* patient = searchByID(fields[0]);

        if (patient == NULL)
            return false;

        VitalRecord record;

        record.date = fields[1];

        if (!readNumber(fields[2], record.systolicBP) ||
            !readNumber(fields[3], record.diastolicBP) ||
            !readNumber(fields[4], record.bloodSugar) ||
            !readNumber(fields[5], record.temperature) ||
            !readNumber(fields[6], record.heartRate) ||
            !readNumber(fields[7], record.oxygenLevel) ||
            !readNumber(fields[8], record.height) ||
            !readNumber(fields[9], record.weight) ||
            !readNumber(fields[10], record.BMI))
        {
            return false;
        }

        record.abnormalFlags = fields[11];
        record.nurseID = fields[12];

        patient->addVitalRecord(record);
    }

    return !file.bad();
}

// ==================================================
// SAVE MEDICAL HISTORY
// ==================================================

bool PatientManager::saveMedicalHistory() const
{
    const char* TEMP_FILE = "DATA/medical_records.tmp";
    const char* BACKUP_FILE = "DATA/medical_records.bak";

    ofstream file(TEMP_FILE);

    if (!file.is_open())
        return false;

    PatientNode* current = head;

    while (current != NULL)
    {
        if (current->data == NULL)
        {
            file.close();
            remove(TEMP_FILE);
            return false;
        }

        const MedicalNode* medical =
            current->data->getMedicalHead();

        while (medical != NULL)
        {
            const MedicalRecord& r = medical->data;

            if (containsInvalidCharacter(r.date) ||
                containsInvalidCharacter(r.symptoms) ||
                containsInvalidCharacter(r.diagnosis) ||
                containsInvalidCharacter(r.clinicalNotes) ||
                containsInvalidCharacter(r.treatment) ||
                containsInvalidCharacter(r.doctorID) ||
                containsInvalidCharacter(r.doctorName))
            {
                file.close();
                remove(TEMP_FILE);
                return false;
            }

            writeMedical(file, current->data->patientID, r);

            if (!file.good())
            {
                file.close();
                remove(TEMP_FILE);
                return false;
            }

            medical = medical->next;
        }

        current = current->next;
    }

    file.close();

    if (!file.good())
    {
        remove(TEMP_FILE);
        return false;
    }

    return safelyReplaceFile(
        TEMP_FILE,
        "DATA/medical_records.txt",
        BACKUP_FILE
    );
}

// ==================================================
// LOAD MEDICAL HISTORY
// ==================================================


bool PatientManager::loadMedicalHistory()
{
    ifstream file("DATA/medical_records.txt");

    if (!file.is_open())
        return true;

    string line;
    int loadedRecords = 0;

    while (getline(file, line))
    {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);

        string fields[10];
        int count = splitFields(line, fields, 10);

        // Current project format is 8 fields. Also accept the older
        // 10-field format so existing user data remains readable.
        if (count != 8 && count != 10)
        {
            cout << "\nInvalid medical record format found.\n";
            cout << "Record: " << line << '\n';
            return false;
        }

        Patient* patient = searchByID(fields[0]);
        if (patient == NULL)
        {
            cout << "\nMedical record references an unknown patient: "
                 << fields[0] << '\n';
            return false;
        }

        MedicalRecord record;
        if (count == 8)
        {
            // patientID|date|symptoms|diagnosis|clinicalNotes|treatment|doctorID|doctorName
            record.date = fields[1];
            record.symptoms = fields[2];
            record.diagnosis = fields[3];
            record.clinicalNotes = fields[4];
            record.treatment = fields[5];
            record.doctorID = fields[6];
            record.doctorName = fields[7];
        }
        else
        {
            // Legacy 10-field format.
            record.date = fields[2];
            record.symptoms = fields[4];
            record.diagnosis = fields[5];
            record.treatment = fields[6];
            record.doctorName = fields[7];
            record.doctorID = fields[8];
            record.clinicalNotes = fields[9];
        }

        patient->addMedicalRecord(record);

        loadedRecords++;
    }

    if (file.bad())
        return false;

    cout << "\nMedical history loaded successfully: "
         << loadedRecords << " records.\n";

    return true;
}

// ==================================================
// RELOAD ALL PATIENT DATA
// ==================================================

bool PatientManager::reloadAllData()
{
    storageReady = false;

    clearPatients();

    nextPatientNumber = 1;

    if (!loadPatients())
        return false;

    if (!loadVitalHistory())
        return false;

    if (!loadMedicalHistory())
        return false;

    rebuildHashTables();

    storageReady = true;

    return true;
}

// ==================================================
// DISPLAY HASH TABLE INFORMATION
// ==================================================

void PatientManager::displaySearchStructures() const
{
    cout << "\n========== SEARCH STRUCTURES ==========\n";

    cout << "\nPatient ID Hash Table:\n";
    patientIDTable.display();

    cout << "\nPhone Number Hash Table:\n";
    phoneTable.display();

    cout << "\nTotal patients: " << patientCount << '\n';
}

// ==================================================
// COUNT ACTIVE PATIENTS FOR A DOCTOR
// ==================================================

int PatientManager::countActivePatientsForDoctor(
    const string& doctorID) const
{
    if (doctorID.empty())
        return 0;

    int activeCount = 0;

    PatientNode* current = head;

    while (current != NULL)
    {
        Patient* patient = current->data;

        if (patient != NULL &&
            patient->doctorID == doctorID &&
            patient->dischargeDate.empty() &&
            patient->status != "Discharged")
        {
            activeCount++;
        }

        current = current->next;
    }

    return activeCount;
}

// ==================================================
// CHECK DOCTOR CAPACITY
// ==================================================

bool PatientManager::isDoctorAtCapacity(
    const string& doctorID) const
{
    const int MAX_PATIENTS_PER_DOCTOR = 5;

    return countActivePatientsForDoctor(doctorID)
           >= MAX_PATIENTS_PER_DOCTOR;
}
// Display patients assigned to a particular doctor
void PatientManager::displayPatientsForDoctor(
    const string& doctorID) const
{
    if (doctorID.empty())
    {
        cout << "\nInvalid doctor ID.\n";
        return;
    }

    PatientNode* current = head;
    bool found = false;

    cout << "\n========== ASSIGNED PATIENTS ==========\n";

    while (current != NULL)
    {
        Patient* patient = current->data;

        if (patient != NULL && patient->doctorID == doctorID)
        {
            patient->displayBasicInfo();
            cout << "----------------------------------\n";
            found = true;
        }

        current = current->next;
    }

    if (!found)
        cout << "\nNo patients assigned to this doctor.\n";
}
bool PatientManager::saveAllData() const
{
    if (!storageReady)
        return false;

    if (!savePatients())
    {
        cout << "\nPatient basic-data save failed.\n";
        return false;
    }

    if (!saveVitalHistory())
    {
        cout << "\nVital-history save failed.\n";
        return false;
    }

    if (!saveMedicalHistory())
    {
        cout << "\nMedical-history save failed.\n";
        return false;
    }

    return true;
}
