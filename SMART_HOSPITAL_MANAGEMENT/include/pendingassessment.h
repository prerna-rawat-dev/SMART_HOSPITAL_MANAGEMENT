#ifndef PENDINGASSESSMENT_H
#define PENDINGASSESSMENT_H

#include <iostream>
#include <string>

using namespace std;

struct PendingAssessment
{
    // Basic visit information
    string visitID;
    string patientID;
    string arrivalDateTime;
    string symptoms;

    // Assessment status
    // Pending / Emergency / Non-Emergency /
    // Awaiting Doctor Assignment / Completed
    string status;

    // General Emergency Assessment Doctor
    string triageDoctorID;

    // Department selected during assessment
    string department;

    // Severity selected by assessment doctor
    // Critical / Severe / Moderate / Low
    string severity;

    // Automatic classification
    // Emergency / Non-Emergency
    string classification;

    // Assigned specialist doctor
    string specialistDoctorID;
    string specialistDoctorName;

    // Assessment date and time
    string assessmentDateTime;

    PendingAssessment* next;

    PendingAssessment();
};

class PendingAssessmentManager
{
private:

    PendingAssessment* front;
    PendingAssessment* rear;

    int assessmentCount;
    int nextVisitNumber;

    bool storageReady;

    // Linked list operations
    void clearQueue();

    // Visit ID generation
    string generateVisitID();

    // File handling
    bool loadAssessments();
    bool saveAssessments() const;

    // Record validation
    bool isValidRecord(
        const PendingAssessment* node
    ) const;

public:

    PendingAssessmentManager();

    ~PendingAssessmentManager();

    PendingAssessmentManager(
        const PendingAssessmentManager& other
    ) = delete;

    PendingAssessmentManager& operator=(
        const PendingAssessmentManager& other
    ) = delete;


    // ==========================================
    // PATIENT REGISTRATION
    // ==========================================

    bool addAssessment(
        const string& patientID,
        const string& symptoms
    );


    // ==========================================
    // PENDING ASSESSMENT QUEUE
    // ==========================================

    void displayPendingAssessments() const;

    PendingAssessment* getNextAssessment() const;

    PendingAssessment* searchByVisitID(
        const string& visitID
    ) const;


    // ==========================================
    // ASSESSMENT MANAGEMENT
    // ==========================================

    bool updateAssessmentStatus(
        const string& visitID,
        const string& newStatus
    );

    bool saveAssessmentDetails(
        const string& visitID,
        const string& triageDoctorID,
        const string& department,
        const string& severity,
        const string& classification,
        const string& specialistDoctorID,
        const string& specialistDoctorName,
        const string& assessmentDateTime,
        const string& status
    );


    // ==========================================
    // ASSESSMENT RECORDS
    // ==========================================

    void displayAssessedPatients() const;

    void displayEmergencyPatients() const;

    void displayNonEmergencyPatients() const;

    void displayAssessmentHistory() const;


    // ==========================================
    // INFORMATION
    // ==========================================

    int getAssessmentCount() const;

    bool isEmpty() const;

};

#endif