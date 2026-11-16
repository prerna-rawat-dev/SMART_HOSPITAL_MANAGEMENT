#ifndef GENERAL_EMERGENCY_H
#define GENERAL_EMERGENCY_H

#include <iostream>
#include <string>

#include "login.h"
#include "account.h"
#include "patientmanager.h"
#include "pendingassessment.h"
#include "doctor.h"
#include "doctorassignment.h"
#include "emergency.h"

using namespace std;

class GeneralEmergencyManager
{
private:

    // Logged-in assessment doctor's information
    string loggedInDoctorID;
    string loggedInDoctorName;
    string loggedInUsername;

public:

    // Constructor
    GeneralEmergencyManager();

    // ==========================================
    // AUTHENTICATION
    // ==========================================

    // Login using Admin-created username/password
    bool login(Login& loginManager);

    // Logout
    void logout();

    // Check login status
    bool isLoggedIn() const;

    // Get logged-in doctor's details
    string getLoggedInDoctorID() const;
    string getLoggedInDoctorName() const;


    // ==========================================
    // GENERAL EMERGENCY DASHBOARD
    // ==========================================

    void displayWelcomeMessage() const;

    void displayDashboard() const;


    // ==========================================
    // PENDING ASSESSMENT
    // ==========================================

    // View patients waiting for assessment
    void viewPendingPatients(
        PendingAssessmentManager& assessmentManager
    ) const;

    // Assess the next pending patient
    bool assessNextPatient(
        PatientManager& patientManager,
        PendingAssessmentManager& assessmentManager,
        DoctorManager& doctorManager,
        DoctorAssignmentManager& assignmentManager,
        EmergencyManager& emergencyManager
    );

    // Assess a particular patient using Visit ID
    bool assessPatient(
        const string& visitID,
        PatientManager& patientManager,
        PendingAssessmentManager& assessmentManager,
        DoctorManager& doctorManager,
        DoctorAssignmentManager& assignmentManager,
        EmergencyManager& emergencyManager
    );


    // ==========================================
    // ASSESSMENT RECORDS
    // ==========================================

    // View all assessed patients
    void viewAssessedPatients(
        PendingAssessmentManager& assessmentManager
    ) const;

    // View Emergency patients
    void viewEmergencyPatients(
        EmergencyManager& emergencyManager
    ) const;

    // View Non-Emergency patients
    void viewNonEmergencyPatients(
        PendingAssessmentManager& assessmentManager
    ) const;

    // View assessment history
    void viewAssessmentHistory(
        PendingAssessmentManager& assessmentManager
    ) const;

};

#endif