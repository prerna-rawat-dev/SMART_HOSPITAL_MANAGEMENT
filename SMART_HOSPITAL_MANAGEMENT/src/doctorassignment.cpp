#include "../include/doctorassignment.h"

#include <iostream>
#include <ctime>

using namespace std;

// ==================================================
// ASSIGN PATIENT TO DOCTOR
// ==================================================

bool DoctorAssignmentManager::assignPatient(
    const string& patientID,
    const string& doctorID,
    PatientManager& patientManager,
    DoctorManager& doctorManager
)
{
    Patient* patient = patientManager.searchByID(patientID);

    if (patient == NULL)
    {
        cout << "\nPatient not found.\n";
        return false;
    }

    Doctor* doctor = doctorManager.searchByID(doctorID);

    if (doctor == NULL)
    {
        cout << "\nDoctor not found.\n";
        return false;
    }

    // Idempotent assignment: an already assigned patient may be assessed
    // again without rejecting the same specialist assignment.
    if (!patient->doctorID.empty() && patient->doctorID == doctorID)
    {
        if (patient->department != doctor->department)
        {
            cout << "\nExisting assignment department does not match.\n";
            return false;
        }
        return true;
    }

    // Never silently replace another active specialist assignment.
    if (!patient->doctorID.empty() &&
        patient->status != "Registered" &&
        patient->status != "Awaiting Doctor Assignment" &&
        patient->status != "Pending Assessment")
    {
        cout << "\nPatient is already assigned to another doctor.\n";
        return false;
    }

    if (patient->department != doctor->department)
    {
        cout << "\nDoctor belongs to a different department.\n";
        return false;
    }

    int activeCount =
        patientManager.countActivePatientsForDoctor(doctorID);

    if (!doctorManager.canAcceptPatient(doctorID, activeCount))
    {
        cout << "\nDoctor is unavailable or has reached maximum capacity.\n";
        return false;
    }

    string oldDoctorID = patient->doctorID;
    string oldDoctorName = patient->doctorName;
    string oldStatus = patient->status;

    patient->doctorID = doctor->doctorID;
    patient->doctorName = doctor->name;
    patient->status = "Assigned";

    if (!patientManager.saveAllData())
    {
        patient->doctorID = oldDoctorID;
        patient->doctorName = oldDoctorName;
        patient->status = oldStatus;

        cout << "\nAssignment could not be saved.\n";
        return false;
    }

    cout << "\nPATIENT ASSIGNED SUCCESSFULLY!\n";
    cout << "Patient ID: " << patientID << '\n';
    cout << "Doctor Name: " << doctor->name << '\n';
    cout << "Doctor ID: " << doctor->doctorID << '\n';
    cout << "Department: " << doctor->department << '\n';
    cout << "Active Patients: " << activeCount + 1 << "/5\n";

    if (activeCount + 1 >= MAX_PATIENTS_PER_DOCTOR)
    {
        cout << "\nDoctor has reached maximum patient capacity.\n";
        cout << "Effective Availability: UNAVAILABLE\n";
    }

    return true;
}

// ==================================================
// DISCHARGE PATIENT
// ==================================================

bool DoctorAssignmentManager::dischargePatient(
    const string& patientID,
    PatientManager& patientManager
)
{
    Patient* patient = patientManager.searchByID(patientID);

    if (patient == NULL)
    {
        cout << "\nPatient not found.\n";
        return false;
    }

    if (patient->doctorID.empty())
    {
        cout << "\nPatient does not have an assigned doctor.\n";
        return false;
    }

    if (patient->status == "Discharged" ||
        !patient->dischargeDate.empty())
    {
        cout << "\nPatient is already discharged.\n";
        return false;
    }

    string oldStatus = patient->status;
    string oldDischargeDate = patient->dischargeDate;
    bool oldAdmissionStatus = patient->isAdmitted;

    time_t now = time(NULL);
    tm* currentTime = localtime(&now);

    if (currentTime == NULL)
        return false;

    char dateBuffer[20];

    strftime(
        dateBuffer,
        sizeof(dateBuffer),
        "%d-%m-%Y",
        currentTime
    );

    patient->dischargeDate = dateBuffer;
    patient->status = "Discharged";
    patient->isAdmitted = false;

    if (!patientManager.saveAllData())
    {
        patient->status = oldStatus;
        patient->dischargeDate = oldDischargeDate;
        patient->isAdmitted = oldAdmissionStatus;

        cout << "\nDischarge could not be saved.\n";
        return false;
    }

    cout << "\nPATIENT DISCHARGED SUCCESSFULLY!\n";
    cout << "Patient ID: " << patientID << '\n';
    cout << "Doctor capacity has been released.\n";

    return true;
}

// ==================================================
// DISPLAY DOCTOR CAPACITY
// ==================================================

void DoctorAssignmentManager::displayDoctorCapacity(
    const string& doctorID,
    PatientManager& patientManager,
    DoctorManager& doctorManager
) const
{
    Doctor* doctor = doctorManager.searchByID(doctorID);

    if (doctor == NULL)
    {
        cout << "\nDoctor not found.\n";
        return;
    }

    int activeCount =
        patientManager.countActivePatientsForDoctor(doctorID);

    cout << "\n========== DOCTOR CAPACITY ==========\n";

    cout << "Doctor ID: " << doctor->doctorID << '\n';
    cout << "Doctor Name: " << doctor->name << '\n';
    cout << "Department: " << doctor->department << '\n';

    cout << "Active Patients: "
         << activeCount << "/"
         << MAX_PATIENTS_PER_DOCTOR << '\n';

    cout << "Manual Availability: "
         << doctor->availability << '\n';

    cout << "Effective Availability: "
         << doctorManager.getEffectiveAvailability(
                doctorID,
                activeCount
            ) << '\n';

    cout << "=====================================\n";
}