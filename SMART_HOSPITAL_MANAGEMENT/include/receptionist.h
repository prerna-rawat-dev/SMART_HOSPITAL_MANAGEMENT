#ifndef RECEPTIONIST_H
#define RECEPTIONIST_H

#include <iostream>
#include <string>

#include "patientmanager.h"
#include "appointment.h"
#include "appointmentrequests.h"
#include "doctor.h"
#include "emergency.h"
#include "bed.h"
#include "pendingassessment.h"

using namespace std;

class Receptionist
{
private:
    void displayPatientMenu() const;
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

    void viewAppointments(
        AppointmentManager& appointmentManager
    ) const;

    void reviewAppointmentRequests(AppointmentRequestManager& requestManager, AppointmentManager& appointmentManager, bool approve);
    void viewDoctorApprovedRequests(AppointmentRequestManager& requestManager) const;

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