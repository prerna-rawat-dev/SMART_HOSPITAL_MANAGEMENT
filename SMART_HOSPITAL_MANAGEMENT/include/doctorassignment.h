#ifndef DOCTOR_ASSIGNMENT_H
#define DOCTOR_ASSIGNMENT_H

#include <iostream>
#include <string>

#include "patientmanager.h"
#include "doctor.h"

using namespace std;

class DoctorAssignmentManager
{
public:

    bool assignPatient(
        const string& patientID,
        const string& doctorID,
        PatientManager& patientManager,
        DoctorManager& doctorManager
    );

    bool dischargePatient(
        const string& patientID,
        PatientManager& patientManager
    );

    void displayDoctorCapacity(
        const string& doctorID,
        PatientManager& patientManager,
        DoctorManager& doctorManager
    ) const;
};

#endif