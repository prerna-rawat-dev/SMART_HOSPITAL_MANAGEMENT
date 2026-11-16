#include "../include/emergency.h"

#include <fstream>
#include <sstream>
#include <cstdio>

using namespace std;

EmergencyCase::EmergencyCase()
{
    emergencyID = "";
    visitID = "";
    patientID = "";
    reason = "";
    triageDoctorID = "";
    assessmentDateTime = "";
    severity = "";
    specialistDepartment = "";
    specialistDoctorID = "";
    bedStatus = "Not Assigned";
    status = "Waiting";
    arrivalOrder = 0;
}

EmergencyManager::EmergencyManager()
{
    capacity = 10;
    emergencyCount = 0;
    nextEmergencyNumber = 1;
    nextArrivalOrder = 1;

    heap = new EmergencyCase[capacity];

    loadEmergencies();
}

EmergencyManager::~EmergencyManager()
{
    saveEmergencies();
    delete[] heap;
}

void EmergencyManager::resizeHeap()
{
    capacity *= 2;

    EmergencyCase* newHeap = new EmergencyCase[capacity];

    for (int i = 0; i < emergencyCount; i++)
        newHeap[i] = heap[i];

    delete[] heap;
    heap = newHeap;
}

void EmergencyManager::swapCases(int first, int second)
{
    EmergencyCase temp = heap[first];
    heap[first] = heap[second];
    heap[second] = temp;
}

int EmergencyManager::getPriority(const string& severity) const
{
    if (severity == "Critical") return 4;
    if (severity == "Severe") return 3;
    if (severity == "Moderate") return 2;
    if (severity == "Low") return 1;

    return 0;
}

bool EmergencyManager::hasHigherPriority(
    const EmergencyCase& first,
    const EmergencyCase& second
) const
{
    int p1 = getPriority(first.severity);
    int p2 = getPriority(second.severity);

    if (p1 > p2) return true;

    if (p1 == p2 && first.arrivalOrder < second.arrivalOrder)
        return true;

    return false;
}

void EmergencyManager::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;

        if (hasHigherPriority(heap[index], heap[parent]))
        {
            swapCases(index, parent);
            index = parent;
        }
        else
            break;
    }
}

void EmergencyManager::heapifyDown(int index)
{
    while (true)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int highest = index;

        if (left < emergencyCount &&
            hasHigherPriority(heap[left], heap[highest]))
            highest = left;

        if (right < emergencyCount &&
            hasHigherPriority(heap[right], heap[highest]))
            highest = right;

        if (highest == index)
            break;

        swapCases(index, highest);
        index = highest;
    }
}

string EmergencyManager::generateEmergencyID()
{
    string number = to_string(nextEmergencyNumber++);

    while (number.length() < 5)
        number = "0" + number;

    return "EMG" + number;
}

bool EmergencyManager::addEmergency(const EmergencyCase& emergency)
{
    if (emergency.patientID.empty() ||
        emergency.reason.empty() ||
        getPriority(emergency.severity) == 0)
    {
        cout << "\nInvalid emergency information.\n";
        return false;
    }

    if (!emergency.visitID.empty() &&
        searchByVisitID(emergency.visitID) != NULL)
    {
        cout << "\nAn emergency record already exists for this visit.\n";
        return false;
    }

    if (emergencyCount == capacity)
        resizeHeap();

    EmergencyCase newCase = emergency;

    if (newCase.emergencyID.empty())
        newCase.emergencyID = generateEmergencyID();
    else
    {
        if (searchEmergency(newCase.emergencyID) != NULL)
            return false;

        string numberPart = newCase.emergencyID.substr(3);
        stringstream ss(numberPart);
        int number = 0;

        if (ss >> number)
        {
            if (number >= nextEmergencyNumber)
                nextEmergencyNumber = number + 1;
        }
    }

    newCase.arrivalOrder = nextArrivalOrder++;

    if (newCase.status.empty())
        newCase.status = "Waiting";

    if (newCase.bedStatus.empty())
        newCase.bedStatus = "Not Assigned";

    heap[emergencyCount] = newCase;
    heapifyUp(emergencyCount);
    emergencyCount++;

    if (!saveEmergencies())
    {
        emergencyCount--;

        for (int i = 0; i < emergencyCount; i++)
            heapifyDown(i);

        return false;
    }

    cout << "\nEmergency registered successfully.\n";
    cout << "Emergency ID: " << newCase.emergencyID << endl;

    return true;
}

EmergencyCase* EmergencyManager::getHighestPriorityEmergency()
{
    if (isEmpty())
        return NULL;

    return &heap[0];
}

EmergencyCase* EmergencyManager::searchEmergency(
    const string& emergencyID
) const
{
    for (int i = 0; i < emergencyCount; i++)
    {
        if (heap[i].emergencyID == emergencyID)
            return &heap[i];
    }

    return NULL;
}

EmergencyCase* EmergencyManager::searchByVisitID(
    const string& visitID
) const
{
    for (int i = 0; i < emergencyCount; i++)
    {
        if (heap[i].visitID == visitID)
            return &heap[i];
    }

    return NULL;
}

bool EmergencyManager::removeEmergency(const string& emergencyID)
{
    int index = -1;

    for (int i = 0; i < emergencyCount; i++)
    {
        if (heap[i].emergencyID == emergencyID)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
        return false;

    EmergencyCase removed = heap[index];

    emergencyCount--;

    if (index != emergencyCount)
    {
        heap[index] = heap[emergencyCount];

        if (index > 0 &&
            hasHigherPriority(heap[index], heap[(index - 1) / 2]))
            heapifyUp(index);
        else
            heapifyDown(index);
    }

    if (!saveEmergencies())
    {
        if (emergencyCount == capacity)
            resizeHeap();

        heap[emergencyCount] = removed;
        emergencyCount++;
        heapifyUp(emergencyCount - 1);

        return false;
    }

    return true;
}

void EmergencyManager::displayEmergency(
    const string& emergencyID
) const
{
    EmergencyCase* e = searchEmergency(emergencyID);

    if (e == NULL)
    {
        cout << "\nEmergency record not found.\n";
        return;
    }

    cout << "\n========== EMERGENCY DETAILS ==========\n";
    cout << "Emergency ID: " << e->emergencyID << endl;
    cout << "Visit ID: " << e->visitID << endl;
    cout << "Patient ID: " << e->patientID << endl;
    cout << "Reason: " << e->reason << endl;
    cout << "Triage Doctor: " << e->triageDoctorID << endl;
    cout << "Assessment Date: " << e->assessmentDateTime << endl;
    cout << "Severity: " << e->severity << endl;
    cout << "Department: " << e->specialistDepartment << endl;
    cout << "Specialist Doctor: " << e->specialistDoctorID << endl;
    cout << "Bed Status: " << e->bedStatus << endl;
    cout << "Status: " << e->status << endl;
}

void EmergencyManager::displayAllEmergencies() const
{
    if (isEmpty())
    {
        cout << "\nNo emergency records available.\n";
        return;
    }

    cout << "\n========== ALL EMERGENCY RECORDS ==========\n";

    for (int i = 0; i < emergencyCount; i++)
    {
        cout << "\nEmergency ID: " << heap[i].emergencyID;
        cout << "\nVisit ID: " << heap[i].visitID;
        cout << "\nPatient ID: " << heap[i].patientID;
        cout << "\nSeverity: " << heap[i].severity;
        cout << "\nStatus: " << heap[i].status << endl;
    }
}

void EmergencyManager::displayEmergencyQueue() const
{
    if (isEmpty())
    {
        cout << "\nEmergency queue is empty.\n";
        return;
    }

    EmergencyCase* temp = new EmergencyCase[emergencyCount];

    for (int i = 0; i < emergencyCount; i++)
        temp[i] = heap[i];

    for (int i = 0; i < emergencyCount; i++)
    {
        int highest = i;

        for (int j = i + 1; j < emergencyCount; j++)
        {
            if (hasHigherPriority(temp[j], temp[highest]))
                highest = j;
        }

        EmergencyCase swapTemp = temp[i];
        temp[i] = temp[highest];
        temp[highest] = swapTemp;

        cout << "\nEmergency ID: " << temp[i].emergencyID;
        cout << "\nVisit ID: " << temp[i].visitID;
        cout << "\nPatient ID: " << temp[i].patientID;
        cout << "\nSeverity: " << temp[i].severity;
        cout << "\nStatus: " << temp[i].status;
        cout << "\n-----------------------------\n";
    }

    delete[] temp;
}

bool EmergencyManager::updateEmergencyStatus(
    const string& emergencyID,
    const string& newStatus
)
{
    EmergencyCase* e = searchEmergency(emergencyID);

    if (e == NULL || newStatus.empty())
        return false;

    string oldStatus = e->status;
    e->status = newStatus;

    if (!saveEmergencies())
    {
        e->status = oldStatus;
        return false;
    }

    return true;
}

bool EmergencyManager::updateEmergencyAssignmentByVisitID(
    const string& visitID,
    const string& specialistDoctorID,
    const string& newStatus)
{
    EmergencyCase* emergency = searchByVisitID(visitID);
    if (emergency == NULL || newStatus.empty())
        return false;

    string oldDoctorID = emergency->specialistDoctorID;
    string oldStatus = emergency->status;
    emergency->specialistDoctorID = specialistDoctorID;
    emergency->status = newStatus;

    if (!saveEmergencies())
    {
        emergency->specialistDoctorID = oldDoctorID;
        emergency->status = oldStatus;
        return false;
    }
    return true;
}

int EmergencyManager::getEmergencyCount() const
{
    return emergencyCount;
}

bool EmergencyManager::isEmpty() const
{
    return emergencyCount == 0;
}

bool EmergencyManager::loadEmergencies()
{
    ifstream file("DATA/emergencies.txt");

    if (!file.is_open())
        return true;

    string line;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        string fields[12];
        stringstream ss(line);
        int count = 0;

        while (count < 12 && getline(ss, fields[count], '|'))
            count++;

        // Old records contain 11 fields.
        // New records contain 12 fields.
        if (count != 11 && count != 12)
            continue;

        EmergencyCase e;

        if (count == 12)
        {
            e.emergencyID = fields[0];
            e.visitID = fields[1];
            e.patientID = fields[2];
            e.reason = fields[3];
            e.triageDoctorID = fields[4];
            e.assessmentDateTime = fields[5];
            e.severity = fields[6];
            e.specialistDepartment = fields[7];
            e.specialistDoctorID = fields[8];
            e.bedStatus = fields[9];
            e.status = fields[10];

            stringstream numberStream(fields[11]);
            if (!(numberStream >> e.arrivalOrder))
                continue;
        }
        else
        {
            e.emergencyID = fields[0];
            e.patientID = fields[1];
            e.reason = fields[2];
            e.triageDoctorID = fields[3];
            e.assessmentDateTime = fields[4];
            e.severity = fields[5];
            e.specialistDepartment = fields[6];
            e.specialistDoctorID = fields[7];
            e.bedStatus = fields[8];
            e.status = fields[9];

            stringstream numberStream(fields[10]);
            if (!(numberStream >> e.arrivalOrder))
                continue;
        }

        if (e.emergencyID.empty() ||
            e.patientID.empty() ||
            getPriority(e.severity) == 0 ||
            searchEmergency(e.emergencyID) != NULL)
            continue;

        if (emergencyCount == capacity)
            resizeHeap();

        heap[emergencyCount] = e;
        heapifyUp(emergencyCount);
        emergencyCount++;

        if (e.arrivalOrder >= nextArrivalOrder)
            nextArrivalOrder = e.arrivalOrder + 1;

        string idPart = e.emergencyID.substr(3);
        stringstream idStream(idPart);
        int idNumber = 0;

        if (idStream >> idNumber)
        {
            if (idNumber >= nextEmergencyNumber)
                nextEmergencyNumber = idNumber + 1;
        }
    }

    file.close();
    return true;
}

bool EmergencyManager::saveEmergencies() const
{
    const string tempFile = "DATA/emergencies.tmp";
    const string dataFile = "DATA/emergencies.txt";

    ofstream file(tempFile.c_str());

    if (!file.is_open())
        return false;

    for (int i = 0; i < emergencyCount; i++)
    {
        file << heap[i].emergencyID << "|"
             << heap[i].visitID << "|"
             << heap[i].patientID << "|"
             << heap[i].reason << "|"
             << heap[i].triageDoctorID << "|"
             << heap[i].assessmentDateTime << "|"
             << heap[i].severity << "|"
             << heap[i].specialistDepartment << "|"
             << heap[i].specialistDoctorID << "|"
             << heap[i].bedStatus << "|"
             << heap[i].status << "|"
             << heap[i].arrivalOrder << "\n";
    }

    file.close();

    if (!file)
        return false;

    if (remove(dataFile.c_str()) != 0)
    {
        ifstream check(dataFile.c_str());

        if (check.good())
        {
            check.close();
            remove(tempFile.c_str());
            return false;
        }
    }

    if (rename(tempFile.c_str(), dataFile.c_str()) != 0)
        return false;

    return true;
}