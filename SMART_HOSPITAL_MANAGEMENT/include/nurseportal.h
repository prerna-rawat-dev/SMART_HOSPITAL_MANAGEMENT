#ifndef NURSEPORTAL_H
#define NURSEPORTAL_H

#include "nurse.h"
#include "patientmanager.h"
#include "bed.h"
#include "resource.h"

#include <iostream>
#include <string>
using namespace std;

class NursePortal
{
private:
    string loggedInNurseID;
    string loggedInNurseName;
    string loggedInUsername;
    string nurseDepartment;
    string nursePhone;
    string nurseAvailability;

    string assignedDoctorID;
    string assignedDoctorName;

    bool hasPatientAccess(const Patient* patient) const;

public:
    NursePortal();

    bool login(const string& nurseID, NurseManager& nurseManager);
    void logout();

    bool isLoggedIn() const;

    void displayDashboard() const;
    void displayNurseProfile() const;

    void viewAssignedPatients(PatientManager& patientManager) const;

    void viewPatientDetails(
        PatientManager& patientManager,
        const string& patientID
    ) const;

    void recordVitals(
        PatientManager& patientManager,
        const string& patientID
    ) const;

    void viewMedicalInstructions(
        PatientManager& patientManager,
        const string& patientID
    ) const;

    void assignPatientBed(
        PatientManager& patientManager,
        BedManager& bedManager,
        const string& patientID
    ) const;

    void releasePatientBed(
        PatientManager& patientManager,
        BedManager& bedManager,
        const string& patientID
    ) const;

    void dischargePatient(
        PatientManager& patientManager,
        BedManager& bedManager,
        const string& patientID
    ) const;

    void assignEquipment(
        PatientManager& patientManager,
        ResourceManager& resourceManager,
        const string& patientID
    ) const;

    void returnEquipment(
        ResourceManager& resourceManager,
        const string& assignmentID
    ) const;
};

#endif