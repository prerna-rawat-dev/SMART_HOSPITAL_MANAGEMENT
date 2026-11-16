#include "../include/pendingassessment.h"

#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cctype>

using namespace std;

// ==========================================
// FILE PATHS
// ==========================================

const char* ASSESSMENT_FILE =
    "DATA/pending_assessments.txt";

const char* TEMP_ASSESSMENT_FILE =
    "DATA/pending_assessments.tmp";

const char* BACKUP_ASSESSMENT_FILE =
    "DATA/pending_assessments.bak";


// ==========================================
// HELPER FUNCTIONS
// ==========================================

static string trimField(const string& value)
{
    size_t start = 0;
    size_t end = value.length();

    while (start < end &&
           isspace(static_cast<unsigned char>(value[start])))
        start++;

    while (end > start &&
           isspace(static_cast<unsigned char>(value[end - 1])))
        end--;

    return value.substr(start, end - start);
}


// Split records while preserving empty fields
static int splitFields(
    const string& line,
    string fields[],
    int maximumFields
)
{
    int count = 0;
    size_t start = 0;

    while (true)
    {
        if (count >= maximumFields)
            return maximumFields + 1;

        size_t position = line.find('|', start);

        if (position == string::npos)
        {
            fields[count++] =
                trimField(line.substr(start));

            break;
        }

        fields[count++] =
            trimField(line.substr(start, position - start));

        start = position + 1;
    }

    return count;
}


static bool containsInvalidCharacter(const string& value)
{
    return value.find('|') != string::npos ||
           value.find('\n') != string::npos ||
           value.find('\r') != string::npos;
}


// ==========================================
// NODE CONSTRUCTOR
// ==========================================

PendingAssessment::PendingAssessment()
{
    status = "Pending";
    next = NULL;
}


// ==========================================
// MANAGER CONSTRUCTOR
// ==========================================

PendingAssessmentManager::PendingAssessmentManager()
{
    front = NULL;
    rear = NULL;

    assessmentCount = 0;
    nextVisitNumber = 1;

    storageReady = false;

    // Create file if it does not exist
    ofstream createFile(
        ASSESSMENT_FILE,
        ios::app
    );

    if (!createFile.is_open())
    {
        cout << "\nUnable to create assessment file.\n";
        return;
    }

    createFile.close();

    storageReady = loadAssessments();

    if (!storageReady)
    {
        cout << "\nWARNING: Assessment records could not be loaded.\n";
        cout << "Existing data has been protected.\n";
        cout << "Assessment operations are disabled.\n";
    }
}


// ==========================================
// DESTRUCTOR
// ==========================================

PendingAssessmentManager::~PendingAssessmentManager()
{
    clearQueue();
}


// ==========================================
// CLEAR LINKED LIST
// ==========================================

void PendingAssessmentManager::clearQueue()
{
    PendingAssessment* current = front;

    while (current != NULL)
    {
        PendingAssessment* temp = current;

        current = current->next;

        delete temp;
    }

    front = NULL;
    rear = NULL;

    assessmentCount = 0;
}


// ==========================================
// GENERATE VISIT ID
// ==========================================

string PendingAssessmentManager::generateVisitID()
{
    string number = to_string(nextVisitNumber);

    while (number.length() < 5)
        number = "0" + number;

    return "VIS" + number;
}


// ==========================================
// VALIDATE RECORD
// ==========================================

bool PendingAssessmentManager::isValidRecord(
    const PendingAssessment* node
) const
{
    if (node == NULL)
        return false;

    if (node->visitID.empty() ||
        node->patientID.empty() ||
        node->arrivalDateTime.empty() ||
        node->symptoms.empty() ||
        node->status.empty())
    {
        return false;
    }

    if (containsInvalidCharacter(node->visitID) ||
        containsInvalidCharacter(node->patientID) ||
        containsInvalidCharacter(node->arrivalDateTime) ||
        containsInvalidCharacter(node->symptoms) ||
        containsInvalidCharacter(node->status) ||
        containsInvalidCharacter(node->triageDoctorID) ||
        containsInvalidCharacter(node->department) ||
        containsInvalidCharacter(node->severity) ||
        containsInvalidCharacter(node->classification) ||
        containsInvalidCharacter(node->specialistDoctorID) ||
        containsInvalidCharacter(node->specialistDoctorName) ||
        containsInvalidCharacter(node->assessmentDateTime))
    {
        return false;
    }

    return true;
}


// ==========================================
// LOAD ASSESSMENTS
// ==========================================

bool PendingAssessmentManager::loadAssessments()
{
    ifstream file(ASSESSMENT_FILE);

    if (!file.is_open())
        return false;

    string line;

    int maximumID = 0;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        string fields[13];

        int count = splitFields(
            line,
            fields,
            13
        );

        // Support old 5-field records and new 12-field records
        if (count != 5 && count != 12)
        {
            clearQueue();
            file.close();
            return false;
        }

        PendingAssessment* node =
            new PendingAssessment();

        node->visitID = fields[0];
        node->patientID = fields[1];
        node->arrivalDateTime = fields[2];
        node->symptoms = fields[3];
        node->status = fields[4];

        // New assessment fields
        if (count == 12)
        {
            node->triageDoctorID = fields[5];
            node->department = fields[6];
            node->severity = fields[7];
            node->classification = fields[8];
            node->specialistDoctorID = fields[9];
            node->specialistDoctorName = fields[10];
            node->assessmentDateTime = fields[11];
        }
        else
        {
            // Preserve old records
            if (node->status == "Emergency" ||
                node->status == "Non-Emergency")
            {
                node->classification = node->status;
            }
        }

        if (!isValidRecord(node))
        {
            delete node;
            clearQueue();
            file.close();
            return false;
        }

        // Validate Visit ID
        if (node->visitID.length() != 8 ||
            node->visitID.substr(0, 3) != "VIS")
        {
            delete node;
            clearQueue();
            file.close();
            return false;
        }

        string number =
            node->visitID.substr(3);

        for (int i = 0; i < 5; i++)
        {
            if (!isdigit(
                static_cast<unsigned char>(number[i])))
            {
                delete node;
                clearQueue();
                file.close();
                return false;
            }
        }

        int value = atoi(number.c_str());

        if (value <= 0)
        {
            delete node;
            clearQueue();
            file.close();
            return false;
        }

        // Prevent duplicate Visit IDs
        if (searchByVisitID(node->visitID) != NULL)
        {
            delete node;
            clearQueue();
            file.close();
            return false;
        }

        if (value > maximumID)
            maximumID = value;

        // Insert into linked list
        if (front == NULL)
        {
            front = rear = node;
        }
        else
        {
            rear->next = node;
            rear = node;
        }

        assessmentCount++;
    }

    bool successful = !file.bad();

    file.close();

    if (!successful)
    {
        clearQueue();
        return false;
    }

    nextVisitNumber = maximumID + 1;

    return true;
}


// ==========================================
// SAVE ASSESSMENTS
// ==========================================

bool PendingAssessmentManager::saveAssessments() const
{
    if (!storageReady)
        return false;

    ofstream file(
        TEMP_ASSESSMENT_FILE,
        ios::out | ios::trunc
    );

    if (!file.is_open())
        return false;

    PendingAssessment* current = front;

    while (current != NULL)
    {
        if (!isValidRecord(current))
        {
            file.close();
            remove(TEMP_ASSESSMENT_FILE);
            return false;
        }

        file << current->visitID << "|"
             << current->patientID << "|"
             << current->arrivalDateTime << "|"
             << current->symptoms << "|"
             << current->status << "|"
             << current->triageDoctorID << "|"
             << current->department << "|"
             << current->severity << "|"
             << current->classification << "|"
             << current->specialistDoctorID << "|"
             << current->specialistDoctorName << "|"
             << current->assessmentDateTime
             << "\n";

        current = current->next;
    }

    file.close();

    if (!file.good())
    {
        remove(TEMP_ASSESSMENT_FILE);
        return false;
    }

    // Preserve existing data
    remove(BACKUP_ASSESSMENT_FILE);

    if (rename(
        ASSESSMENT_FILE,
        BACKUP_ASSESSMENT_FILE
    ) != 0)
    {
        remove(TEMP_ASSESSMENT_FILE);
        return false;
    }

    // Replace original file
    if (rename(
        TEMP_ASSESSMENT_FILE,
        ASSESSMENT_FILE
    ) != 0)
    {
        rename(
            BACKUP_ASSESSMENT_FILE,
            ASSESSMENT_FILE
        );

        remove(TEMP_ASSESSMENT_FILE);

        return false;
    }

    remove(BACKUP_ASSESSMENT_FILE);

    return true;
}


// ==========================================
// ADD NEW PATIENT TO QUEUE
// ==========================================

bool PendingAssessmentManager::addAssessment(
    const string& patientID,
    const string& symptoms
)
{
    if (!storageReady)
        return false;

    if (patientID.empty() || symptoms.empty())
        return false;

    if (containsInvalidCharacter(patientID) ||
        containsInvalidCharacter(symptoms))
    {
        return false;
    }

    if (nextVisitNumber <= 0)
        return false;

    PendingAssessment* node =
        new PendingAssessment();

    node->visitID = generateVisitID();

    node->patientID = patientID;

    node->symptoms = symptoms;

    // Current date and time
    time_t now = time(NULL);

    tm* localTime = localtime(&now);

    if (localTime == NULL)
    {
        delete node;
        return false;
    }

    char buffer[30];

    strftime(
        buffer,
        sizeof(buffer),
        "%d-%m-%Y %H:%M:%S",
        localTime
    );

    node->arrivalDateTime = buffer;

    node->status = "Pending";

    // Insert at rear
    if (front == NULL)
    {
        front = rear = node;
    }
    else
    {
        rear->next = node;
        rear = node;
    }

    assessmentCount++;

    if (!saveAssessments())
    {
        // Rollback insertion
        if (front == node)
        {
            front = NULL;
            rear = NULL;
        }
        else
        {
            PendingAssessment* current = front;

            while (current != NULL &&
                   current->next != node)
            {
                current = current->next;
            }

            if (current != NULL)
            {
                current->next = NULL;
                rear = current;
            }
        }

        delete node;

        assessmentCount--;

        return false;
    }

    nextVisitNumber++;

    cout << "\n============================================\n";
    cout << "       PATIENT ADDED SUCCESSFULLY\n";
    cout << "============================================\n";

    cout << "Patient ID: " << node->patientID << '\n';
    cout << "Visit ID: " << node->visitID << '\n';

    cout << "\nYour registration is complete.\n";
    cout << "Please wait while the General Emergency\n";
    cout << "Assessment Doctor reviews your case.\n";

    cout << "\nStatus: PENDING ASSESSMENT\n";

    return true;
}


// ==========================================
// DISPLAY PENDING ASSESSMENTS
// ==========================================

void PendingAssessmentManager::displayPendingAssessments() const
{
    cout << "\n========== PENDING ASSESSMENT QUEUE ==========\n";

    PendingAssessment* current = front;

    int count = 0;

    while (current != NULL)
    {
        if (current->status == "Pending")
        {
            cout << "\nVisit ID: "
                 << current->visitID;

            cout << "\nPatient ID: "
                 << current->patientID;

            cout << "\nArrival: "
                 << current->arrivalDateTime;

            cout << "\nSymptoms: "
                 << current->symptoms;

            cout << "\nStatus: "
                 << current->status;

            cout << "\n---------------------------------------------\n";

            count++;
        }

        current = current->next;
    }

    if (count == 0)
        cout << "\nNo patients are waiting for assessment.\n";

    cout << "\nTotal Pending Patients: "
         << count << '\n';
}


// ==========================================
// GET NEXT PENDING ASSESSMENT
// ==========================================

PendingAssessment*
PendingAssessmentManager::getNextAssessment() const
{
    PendingAssessment* current = front;

    while (current != NULL)
    {
        if (current->status == "Pending")
            return current;

        current = current->next;
    }

    return NULL;
}


// ==========================================
// SEARCH BY VISIT ID
// ==========================================

PendingAssessment*
PendingAssessmentManager::searchByVisitID(
    const string& visitID
) const
{
    PendingAssessment* current = front;

    while (current != NULL)
    {
        if (current->visitID == visitID)
            return current;

        current = current->next;
    }

    return NULL;
}


// ==========================================
// UPDATE ASSESSMENT STATUS
// ==========================================

bool PendingAssessmentManager::updateAssessmentStatus(
    const string& visitID,
    const string& newStatus
)
{
    if (!storageReady)
        return false;

    if (newStatus != "Pending" &&
        newStatus != "Under Assessment" &&
        newStatus != "Emergency" &&
        newStatus != "Non-Emergency" &&
        newStatus != "Awaiting Doctor Assignment" &&
        newStatus != "Completed")
    {
        return false;
    }

    PendingAssessment* assessment =
        searchByVisitID(visitID);

    if (assessment == NULL)
        return false;

    string oldStatus = assessment->status;

    assessment->status = newStatus;

    if (!saveAssessments())
    {
        assessment->status = oldStatus;
        return false;
    }

    return true;
}


// ==========================================
// SAVE COMPLETE ASSESSMENT DETAILS
// ==========================================

bool PendingAssessmentManager::saveAssessmentDetails(
    const string& visitID,
    const string& triageDoctorID,
    const string& department,
    const string& severity,
    const string& classification,
    const string& specialistDoctorID,
    const string& specialistDoctorName,
    const string& assessmentDateTime,
    const string& status
)
{
    if (!storageReady)
        return false;

    if (triageDoctorID.empty() ||
        department.empty() ||
        severity.empty() ||
        assessmentDateTime.empty())
    {
        return false;
    }

    if (classification != "Emergency" &&
        classification != "Non-Emergency")
    {
        return false;
    }

    if (severity != "Critical" &&
        severity != "Severe" &&
        severity != "Moderate" &&
        severity != "Low")
    {
        return false;
    }

    if (status != "Emergency" &&
        status != "Non-Emergency" &&
        status != "Awaiting Doctor Assignment" &&
        status != "Completed")
    {
        return false;
    }

    PendingAssessment* assessment =
        searchByVisitID(visitID);

    if (assessment == NULL)
        return false;

    // Preserve previous information
    string oldTriageID = assessment->triageDoctorID;
    string oldDepartment = assessment->department;
    string oldSeverity = assessment->severity;
    string oldClassification = assessment->classification;
    string oldSpecialistID = assessment->specialistDoctorID;
    string oldSpecialistName = assessment->specialistDoctorName;
    string oldAssessmentDate = assessment->assessmentDateTime;
    string oldStatus = assessment->status;

    // Update assessment
    assessment->triageDoctorID = triageDoctorID;
    assessment->department = department;
    assessment->severity = severity;
    assessment->classification = classification;

    assessment->specialistDoctorID = specialistDoctorID;
    assessment->specialistDoctorName = specialistDoctorName;

    assessment->assessmentDateTime = assessmentDateTime;
    assessment->status = status;

    if (!saveAssessments())
    {
        // Restore previous information
        assessment->triageDoctorID = oldTriageID;
        assessment->department = oldDepartment;
        assessment->severity = oldSeverity;
        assessment->classification = oldClassification;

        assessment->specialistDoctorID = oldSpecialistID;
        assessment->specialistDoctorName = oldSpecialistName;

        assessment->assessmentDateTime = oldAssessmentDate;
        assessment->status = oldStatus;

        return false;
    }

    return true;
}


// ==========================================
// DISPLAY ALL ASSESSED PATIENTS
// ==========================================

void PendingAssessmentManager::displayAssessedPatients() const
{
    cout << "\n========== ASSESSED PATIENTS ==========\n";

    PendingAssessment* current = front;

    int count = 0;

    while (current != NULL)
    {
        if (!current->classification.empty())
        {
            cout << "\nVisit ID: " << current->visitID;
            cout << "\nPatient ID: " << current->patientID;
            cout << "\nDepartment: " << current->department;
            cout << "\nSeverity: " << current->severity;
            cout << "\nClassification: " << current->classification;
            cout << "\nAssigned Doctor: " << current->specialistDoctorName;
            cout << "\nStatus: " << current->status;

            cout << "\n---------------------------------------\n";

            count++;
        }

        current = current->next;
    }

    if (count == 0)
        cout << "\nNo assessed patients found.\n";

    cout << "\nTotal Assessed Patients: " << count << '\n';
}


// ==========================================
// DISPLAY EMERGENCY PATIENTS
// ==========================================

void PendingAssessmentManager::displayEmergencyPatients() const
{
    cout << "\n========== EMERGENCY PATIENTS ==========\n";

    PendingAssessment* current = front;

    int count = 0;

    while (current != NULL)
    {
        if (current->classification == "Emergency")
        {
            cout << "\nVisit ID: " << current->visitID;
            cout << "\nPatient ID: " << current->patientID;
            cout << "\nDepartment: " << current->department;
            cout << "\nSeverity: " << current->severity;
            cout << "\nSpecialist: " << current->specialistDoctorName;
            cout << "\nStatus: " << current->status;

            cout << "\n----------------------------------------\n";

            count++;
        }

        current = current->next;
    }

    if (count == 0)
        cout << "\nNo emergency patients found.\n";

    cout << "\nTotal Emergency Patients: " << count << '\n';
}


// ==========================================
// DISPLAY NON-EMERGENCY PATIENTS
// ==========================================

void PendingAssessmentManager::displayNonEmergencyPatients() const
{
    cout << "\n========== NON-EMERGENCY PATIENTS ==========\n";

    PendingAssessment* current = front;

    int count = 0;

    while (current != NULL)
    {
        if (current->classification == "Non-Emergency")
        {
            cout << "\nVisit ID: " << current->visitID;
            cout << "\nPatient ID: " << current->patientID;
            cout << "\nDepartment: " << current->department;
            cout << "\nSeverity: " << current->severity;
            cout << "\nSpecialist: " << current->specialistDoctorName;
            cout << "\nStatus: " << current->status;

            cout << "\n--------------------------------------------\n";

            count++;
        }

        current = current->next;
    }

    if (count == 0)
        cout << "\nNo non-emergency patients found.\n";

    cout << "\nTotal Non-Emergency Patients: " << count << '\n';
}


// ==========================================
// DISPLAY COMPLETE ASSESSMENT HISTORY
// ==========================================

void PendingAssessmentManager::displayAssessmentHistory() const
{
    cout << "\n========== COMPLETE ASSESSMENT HISTORY ==========\n";

    PendingAssessment* current = front;

    if (current == NULL)
    {
        cout << "\nNo assessment history available.\n";
        return;
    }

    while (current != NULL)
    {
        cout << "\nVisit ID: " << current->visitID;
        cout << "\nPatient ID: " << current->patientID;
        cout << "\nArrival: " << current->arrivalDateTime;
        cout << "\nSymptoms: " << current->symptoms;
        cout << "\nTriage Doctor ID: " << current->triageDoctorID;
        cout << "\nDepartment: " << current->department;
        cout << "\nSeverity: " << current->severity;
        cout << "\nClassification: " << current->classification;
        cout << "\nSpecialist Doctor: " << current->specialistDoctorName;
        cout << "\nAssessment Date: " << current->assessmentDateTime;
        cout << "\nStatus: " << current->status;

        cout << "\n-----------------------------------------------\n";

        current = current->next;
    }
}


// ==========================================
// GET TOTAL ASSESSMENT COUNT
// ==========================================

int PendingAssessmentManager::getAssessmentCount() const
{
    return assessmentCount;
}


// ==========================================
// CHECK EMPTY PENDING QUEUE
// ==========================================

bool PendingAssessmentManager::isEmpty() const
{
    return getNextAssessment() == NULL;
}