#include "../include/generalemergency.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace
{
    const string GENERAL_EMERGENCY_ROLE = "General Emergency Assessment";

    string currentDateTime()
    {
        time_t now = time(NULL);
        tm* local = localtime(&now);
        char buffer[32] = "";

        if (local != NULL)
            strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", local);

        return string(buffer);
    }

    string makeDoctorID(int number)
    {
        ostringstream output;
        output << "DOC" << setw(5) << setfill('0') << number;
        return output.str();
    }

    Doctor* findEligibleSpecialist(
        const string& department,
        const string& doctorType,
        PatientManager& patientManager,
        DoctorManager& doctorManager)
    {
        // Doctor IDs are generated sequentially as DOC00001, DOC00002, etc.
        // Search through the ID range so the linked-list implementation
        // remains encapsulated inside DoctorManager.
        for (int number = 1; number <= 99999; ++number)
        {
            Doctor* doctor = doctorManager.searchByID(makeDoctorID(number));
            if (doctor == NULL)
                continue;

           if (doctor->department != department ||
    doctor->doctorType != doctorType ||
    doctor->availability != "Available")
    continue;

int activePatients =
    patientManager.countActivePatientsForDoctor(doctor->doctorID);

if (doctorManager.canAcceptPatient(
        doctor->doctorID, activePatients))
{
    return doctor;
}
}

    return NULL;
}

} // End of anonymous namespace


GeneralEmergencyManager::GeneralEmergencyManager()
    : loggedInDoctorID(""),
      loggedInDoctorName(""),
      loggedInUsername("")
{
}

bool GeneralEmergencyManager::login(Login& loginManager)
{
    Account account;

    if (!loginManager.authenticate(GENERAL_EMERGENCY_ROLE, account))
        return false;

    loggedInDoctorID = account.id;
    loggedInDoctorName = account.name;
    loggedInUsername = account.username;

    displayWelcomeMessage();
    return true;
}

void GeneralEmergencyManager::logout()
{
    loggedInDoctorID.clear();
    loggedInDoctorName.clear();
    loggedInUsername.clear();
    cout << "\nGeneral Emergency Assessment Doctor logged out successfully.\n";
}

bool GeneralEmergencyManager::isLoggedIn() const
{
    return !loggedInDoctorID.empty();
}

string GeneralEmergencyManager::getLoggedInDoctorID() const
{
    return loggedInDoctorID;
}

string GeneralEmergencyManager::getLoggedInDoctorName() const
{
    return loggedInDoctorName;
}

void GeneralEmergencyManager::displayWelcomeMessage() const
{
    if (!isLoggedIn())
        return;

    cout << "\n============================================================\n";
    cout << "HELLO " << loggedInDoctorName
         << "! WELCOME TO GENERAL EMERGENCY ASSESSMENT.\n";
    cout << "KINDLY CLASSIFY THE PATIENTS.\n";
    cout << "============================================================\n";
}

void GeneralEmergencyManager::displayDashboard() const
{
    cout << "\n========== GENERAL EMERGENCY ASSESSMENT DASHBOARD ==========";
    cout << "\nDoctor: " << loggedInDoctorName;
    cout << "\n1. View Pending Assessment Queue";
    cout << "\n2. Assess Next Patient";
    cout << "\n3. Assess Patient by Visit ID";
    cout << "\n4. View All Assessed Patients";
    cout << "\n5. View Emergency Patients";
    cout << "\n6. View Non-Emergency Patients";
    cout << "\n7. View Assessment History";
    cout << "\n8. Logout";
    cout << "\n============================================================\n";
}

void GeneralEmergencyManager::viewPendingPatients(
    PendingAssessmentManager& assessmentManager) const
{
    if (!isLoggedIn())
    {
        cout << "\nPlease login first.\n";
        return;
    }

    assessmentManager.displayPendingAssessments();
}

bool GeneralEmergencyManager::assessNextPatient(
    PatientManager& patientManager,
    PendingAssessmentManager& assessmentManager,
    DoctorManager& doctorManager,
    DoctorAssignmentManager& assignmentManager,
    EmergencyManager& emergencyManager)
{
    if (!isLoggedIn())
    {
        cout << "\nPlease login first.\n";
        return false;
    }

    PendingAssessment* assessment = assessmentManager.getNextAssessment();
    if (assessment == NULL)
    {
        cout << "\nThere are no patients pending assessment.\n";
        return false;
    }

    cout << "\nNext patient in queue";
    cout << "\nVisit ID: " << assessment->visitID;
    cout << "\nPatient ID: " << assessment->patientID;
    cout << "\nSymptoms: " << assessment->symptoms << '\n';

    return assessPatient(
        assessment->visitID,
        patientManager,
        assessmentManager,
        doctorManager,
        assignmentManager,
        emergencyManager);
}

bool GeneralEmergencyManager::assessPatient(
    const string& visitID,
    PatientManager& patientManager,
    PendingAssessmentManager& assessmentManager,
    DoctorManager& doctorManager,
    DoctorAssignmentManager& assignmentManager,
    EmergencyManager& emergencyManager)
{
    if (!isLoggedIn())
    {
        cout << "\nPlease login first.\n";
        return false;
    }

    PendingAssessment* assessment =
        assessmentManager.searchByVisitID(visitID);

    if (assessment == NULL)
    {
        cout << "\nVisit ID not found.\n";
        return false;
    }

    bool firstAssessment = assessment->status == "Pending";
    bool awaitingAssignment =
        assessment->status == "Awaiting Doctor Assignment";

    if (!firstAssessment && !awaitingAssignment)
    {
        cout << "\nThis visit has already been assessed. Current status: "
             << assessment->status << '\n';
        return false;
    }

    Patient* patient = patientManager.searchByID(assessment->patientID);
    if (patient == NULL)
    {
        cout << "\nPatient record not found.\n";
        return false;
    }

    string department;
    string severity;
    string classification;

    cout << "\n========== PATIENT ASSESSMENT ==========";
    cout << "\nVisit ID: " << assessment->visitID;
    cout << "\nPatient ID: " << patient->patientID;
    cout << "\nPatient Name: " << patient->name;
    cout << "\nAge: " << patient->age;
    cout << "\nGender: " << patient->gender;
    cout << "\nSymptoms: " << assessment->symptoms << '\n';

    if (firstAssessment)
    {
        int severityChoice = 0;
        cout << "\nSelect severity after examination:";
        cout << "\n1. Critical";
        cout << "\n2. Severe";
        cout << "\n3. Moderate";
        cout << "\n4. Low";
        cout << "\nEnter severity: ";

        if (!(cin >> severityChoice) || severityChoice < 1 || severityChoice > 4)
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\nInvalid severity selection.\n";
            return false;
        }
        cin.ignore(10000, '\n');

        const char* severityNames[] = {
            "Critical", "Severe", "Moderate", "Low"
        };
        severity = severityNames[severityChoice - 1];

        classification = (severity == "Critical" || severity == "Severe")
            ? "Emergency" : "Non-Emergency";

        cout << "Enter department for referral: ";
        getline(cin, department);
        if (department.empty())
        {
            cout << "\nDepartment cannot be empty.\n";
            return false;
        }
    }
    else
    {
        // Retain the original clinical assessment and retry only assignment.
        department = assessment->department;
        severity = assessment->severity;
        classification = assessment->classification;
        cout << "\nPrevious assessment retained.";
        cout << "\nDepartment: " << department;
        cout << "\nSeverity: " << severity;
        cout << "\nClassification: " << classification << '\n';
    }

    string requiredDoctorType =
        classification == "Emergency" ? "Emergency" : "Non-Emergency";

    Doctor* specialist = findEligibleSpecialist(
        department,
        requiredDoctorType,
        patientManager,
        doctorManager);

    string assignedDoctorID = "";
    string assignedDoctorName = "";
    string finalStatus = "Awaiting Doctor Assignment";

    // Save the clinical assessment even when no specialist is available.
    if (specialist != NULL)
    {
        patient->department = department;

        if (assignmentManager.assignPatient(
                patient->patientID,
                specialist->doctorID,
                patientManager,
                doctorManager))
        {
            assignedDoctorID = specialist->doctorID;
            assignedDoctorName = specialist->name;
            finalStatus = classification;
        }
        else
        {
            // The assessment remains saved and can be retried later.
            patient->department = department;
        }
    }

    string assessmentTime = firstAssessment
        ? currentDateTime() : assessment->assessmentDateTime;

    if (firstAssessment)
    {
        patient->isEmergency = (classification == "Emergency");
        patient->emergencySeverity = severity;
        patient->department = department;
    }

    if (finalStatus == "Emergency" && assignedDoctorID.empty())
        finalStatus = "Awaiting Doctor Assignment";

    if (!assessmentManager.saveAssessmentDetails(
            visitID,
            loggedInDoctorID,
            department,
            severity,
            classification,
            assignedDoctorID,
            assignedDoctorName,
            assessmentTime,
            finalStatus))
    {
        cout << "\nError: assessment could not be saved. Please check the data file.\n";
        return false;
    }

    if (classification == "Emergency" && !firstAssessment &&
        !assignedDoctorID.empty())
    {
        if (!emergencyManager.updateEmergencyAssignmentByVisitID(
                visitID, assignedDoctorID, "Waiting"))
        {
            cout << "\nWarning: specialist assigned, but emergency queue update failed.\n";
        }
    }

    if (firstAssessment && classification == "Emergency")
    {
        EmergencyCase emergency;
        emergency.visitID = visitID;
        emergency.patientID = patient->patientID;
        emergency.reason = assessment->symptoms;
        emergency.triageDoctorID = loggedInDoctorID;
        emergency.assessmentDateTime = assessmentTime;
        emergency.severity = severity;
        emergency.specialistDepartment = department;
        emergency.specialistDoctorID = assignedDoctorID;
        emergency.status = assignedDoctorID.empty()
            ? "Awaiting Doctor Assignment" : "Waiting";
        emergency.bedStatus = "Not Assigned";

        if (!emergencyManager.addEmergency(emergency))
        {
            cout << "\nWarning: assessment saved, but emergency queue insertion failed.\n";
        }
        else
        {
            EmergencyCase* savedEmergency =
                emergencyManager.searchByVisitID(visitID);
            if (savedEmergency != NULL)
                patient->emergencyID = savedEmergency->emergencyID;
        }
    }

    if (!patientManager.saveAllData())
        cout << "\nWarning: patient assessment fields could not be persisted.\n";

    if (finalStatus == "Awaiting Doctor Assignment")
    {
        cout << "\nPLEASE WAIT! All eligible doctors in the selected department are currently busy or unavailable.";
        cout << "\nYour assessment has been saved. You will be assigned a doctor as soon as one becomes available.";
        cout << "\nStatus: AWAITING DOCTOR ASSIGNMENT.\n";
        return true;
    }

    cout << "\nASSESSMENT COMPLETED SUCCESSFULLY!";
    cout << "\nSeverity: " << severity;
    cout << "\nClassification: " << classification;
    cout << "\nDepartment: " << department;
    cout << "\nAssigned Specialist: " << assignedDoctorName
         << " (" << assignedDoctorID << ")\n";

    if (classification == "Emergency")
        cout << "Emergency case added to the priority queue.\n";

    return true;
}

void GeneralEmergencyManager::viewAssessedPatients(
    PendingAssessmentManager& assessmentManager) const
{
    if (!isLoggedIn())
    {
        cout << "\nPlease login first.\n";
        return;
    }
    assessmentManager.displayAssessedPatients();
}

void GeneralEmergencyManager::viewEmergencyPatients(
    EmergencyManager& emergencyManager) const
{
    if (!isLoggedIn())
    {
        cout << "\nPlease login first.\n";
        return;
    }
    emergencyManager.displayAllEmergencies();
}

void GeneralEmergencyManager::viewNonEmergencyPatients(
    PendingAssessmentManager& assessmentManager) const
{
    if (!isLoggedIn())
    {
        cout << "\nPlease login first.\n";
        return;
    }
    assessmentManager.displayNonEmergencyPatients();
}

void GeneralEmergencyManager::viewAssessmentHistory(
    PendingAssessmentManager& assessmentManager) const
{
    if (!isLoggedIn())
    {
        cout << "\nPlease login first.\n";
        return;
    }
    assessmentManager.displayAssessmentHistory();
}
