#ifndef RECEPTIONIST_H
#define RECEPTIONIST_H

#include <iostream>
#include <string>

#include "patientmanager.h"
#include "appointment.h"
#include "doctor.h"
#include "emergency.h"
#include "bed.h"
#include "pendingassessment.h"

using namespace std;

class Receptionist
{
private:
    void displayPatientMenu() const;
    void displayAppointmentMenu() const;
    void displayEmergencyMenu() const;

public:
    Receptionist();

    void displayDashboard() const;

    // Patient registration and assessment queue
    void registerNewPatient(
        PatientManager& patientManager,
        DoctorManager& doctorManager,
        PendingAssessmentManager& assessmentManager
    );

    void searchPatient(
        PatientManager& patientManager
    ) const;

    void viewAllPatients(
        PatientManager& patientManager
    ) const;

    void updatePatientDetails(
        PatientManager& patientManager
    );

    // Appointment management
    void manageAppointmentRequests(
        AppointmentManager& appointmentManager
    ) const;

    void viewAppointments(
        AppointmentManager& appointmentManager
    ) const;

    // Emergency management
    void viewEmergencyCases(
        EmergencyManager& emergencyManager
    ) const;

    // Bed management
    void viewAvailableBeds(
        BedManager& bedManager
    ) const;
};

#endif