#include "../include/receptionist.h"

// ==========================================
// CONSTRUCTOR
// ==========================================

Receptionist::Receptionist()
{
}


// ==========================================
// RECEPTIONIST DASHBOARD
// ==========================================

void Receptionist::displayDashboard() const
{
    cout << "\n========== RECEPTIONIST DASHBOARD ==========\n";
    cout << "1. Patient Management\n";
    cout << "2. Appointment Management\n";
    cout << "3. View Emergency Cases\n";
    cout << "4. View Available Beds\n";
    cout << "5. Logout\n";
    cout << "============================================\n";
}


// ==========================================
// PATIENT MANAGEMENT MENU
// ==========================================

void Receptionist::displayPatientMenu() const
{
    cout << "\n========== PATIENT MANAGEMENT ==========\n";
    cout << "1. Register New Patient\n";
    cout << "2. Search Patient\n";
    cout << "3. View All Patients\n";
    cout << "4. Update Patient Details\n";
    cout << "5. Back\n";
    cout << "========================================\n";
}


// ==========================================
// APPOINTMENT MANAGEMENT MENU
// ==========================================

void Receptionist::displayAppointmentMenu() const
{
    cout << "\n========== APPOINTMENT MANAGEMENT ==========\n";
    cout << "1. View Appointment Requests\n";
    cout << "2. Confirm Appointment Request\n";
    cout << "3. Reject Appointment Request\n";
    cout << "4. View All Appointments\n";
    cout << "5. Back\n";
    cout << "============================================\n";
}


// ==========================================
// EMERGENCY MENU
// ==========================================

void Receptionist::displayEmergencyMenu() const
{
    cout << "\n========== EMERGENCY CASES ==========\n";
    cout << "1. View All Emergency Cases\n";
    cout << "2. View Emergency Priority Queue\n";
    cout << "3. Back\n";
    cout << "=====================================\n";
}


// ==========================================
// REGISTER NEW PATIENT
// AUTOMATIC PENDING ASSESSMENT QUEUE
// ==========================================

void Receptionist::registerNewPatient(
    PatientManager& patientManager,
    DoctorManager& doctorManager,
    PendingAssessmentManager& assessmentManager
)
{
    (void)doctorManager;
    Patient patient;

    cout << "\n============================================\n";
    cout << "          NEW PATIENT REGISTRATION\n";
    cout << "============================================\n";

    // --------------------------------------
    // BASIC INFORMATION
    // --------------------------------------

    cout << "\nEnter Patient Name: ";
    getline(cin >> ws, patient.name);

    cout << "Enter Age: ";
    cin >> patient.age;

    if (cin.fail() || patient.age < 0 || patient.age > 120)
    {
        cin.clear();
        cin.ignore(10000, '\n');

        cout << "\nInvalid age. Registration cancelled.\n";
        return;
    }

    cin.ignore(10000, '\n');

    cout << "Enter Gender: ";
    getline(cin, patient.gender);

    cout << "Enter Phone Number: ";
    getline(cin, patient.phone);

    cout << "Enter Address: ";
    getline(cin, patient.address);


    // --------------------------------------
    // EMERGENCY CONTACT
    // --------------------------------------

    cout << "\n========== EMERGENCY CONTACT ==========\n";

    cout << "Emergency Contact Name: ";
    getline(cin, patient.emergencyContactName);

    cout << "Emergency Contact Phone: ";
    getline(cin, patient.emergencyContactPhone);

    cout << "Emergency Contact Relation: ";
    getline(cin, patient.emergencyContactRelation);


    // --------------------------------------
    // MEDICAL INFORMATION
    // --------------------------------------

    cout << "\n========== MEDICAL INFORMATION ==========\n";

    cout << "Blood Group: ";
    getline(cin, patient.bloodGroup);

    cout << "Known Allergies (enter None if not applicable): ";
    getline(cin, patient.allergies);

    cout << "Existing Medical Conditions: ";
    getline(cin, patient.existingConditions);

    cout << "Current Medicines: ";
    getline(cin, patient.currentMedicines);

    cout << "Symptoms Reported by Patient: ";
    getline(cin, patient.initialSymptoms);


    // --------------------------------------
    // INITIAL PATIENT STATUS
    // --------------------------------------

    patient.status = "Registered";

    // Receptionist cannot decide classification
    patient.isEmergency = false;
    patient.emergencyID = "";
    patient.emergencySeverity = "";

    // Admission details
    patient.isAdmitted = false;
    patient.admissionDate = "";
    patient.dischargeDate = "";
    patient.roomID = "";
    patient.bedID = "";

    // Department and specialist doctor
    // will be selected after assessment

    patient.department = "";
    patient.doctorID = "";
    patient.doctorName = "";


    // --------------------------------------
    // REGISTER PATIENT PERMANENTLY
    // --------------------------------------

    if (!patientManager.registerPatient(patient))
    {
        cout << "\n============================================\n";
        cout << "         PATIENT REGISTRATION FAILED\n";
        cout << "============================================\n";

        cout << "Please check patient details and try again.\n";

        return;
    }


    // --------------------------------------
    // PATIENT REGISTRATION SUCCESSFUL
    // --------------------------------------

    cout << "\n============================================\n";
    cout << "       PATIENT REGISTERED SUCCESSFULLY\n";
    cout << "============================================\n";

    cout << "\nPatient ID: "
         << patient.patientID << '\n';


    // --------------------------------------
    // AUTOMATICALLY ADD TO PENDING QUEUE
    // --------------------------------------

    bool added = assessmentManager.addAssessment(
        patient.patientID,
        patient.initialSymptoms
    );

    if (added)
    {
        cout << "\n============================================\n";
        cout << "          REGISTRATION COMPLETE\n";
        cout << "============================================\n";

        cout << "\nYour patient registration is successful.\n";

        cout << "\nPlease wait while the General Emergency\n";
        cout << "Assessment Doctor reviews the patient's case.\n";

        cout << "\nNo department or specialist doctor has been\n";
        cout << "assigned yet.\n";

        cout << "\n============================================\n";
        cout << "             WAITING STATUS\n";
        cout << "============================================\n";

        cout << "Status: PENDING ASSESSMENT\n";

        cout << "\nKindly wait. The doctor will assess the patient.\n";

        cout << "\n============================================\n";
    }
    else
    {
        cout << "\n============================================\n";
        cout << "             QUEUE ERROR\n";
        cout << "============================================\n";

        cout << "Patient ID: " << patient.patientID << '\n';

        cout << "\nWARNING: Patient information was saved,\n";
        cout << "but Pending Assessment Queue insertion failed.\n";

        cout << "\nPlease inform the administrator.\n";

        cout << "Do not register the same patient again.\n";

        cout << "The patient must be added to the assessment\n";
        cout << "queue before routine department assignment.\n";

        cout << "\n============================================\n";
    }
}


// ==========================================
// SEARCH PATIENT
// ==========================================

void Receptionist::searchPatient(
    PatientManager& patientManager
) const
{
    int choice;

    cout << "\n========== SEARCH PATIENT ==========\n";
    cout << "1. Search by Patient ID\n";
    cout << "2. Search by Name\n";
    cout << "3. Search by Phone Number\n";

    cout << "Enter choice: ";
    cin >> choice;

    cin.ignore(10000, '\n');

    Patient* patient = NULL;

    string searchValue;

    switch (choice)
    {
        case 1:

            cout << "Enter Patient ID: ";
            getline(cin, searchValue);

            patient = patientManager.searchByID(searchValue);

            break;

        case 2:

            cout << "Enter Patient Name: ";
            getline(cin, searchValue);

            patient = patientManager.searchByName(searchValue);

            break;

        case 3:

            cout << "Enter Phone Number: ";
            getline(cin, searchValue);

            patient = patientManager.searchByPhone(searchValue);

            break;

        default:

            cout << "\nInvalid choice.\n";

            return;
    }

    if (patient != NULL)
    {
        cout << "\nPatient Found!\n";

        patient->displayBasicInfo();
    }
    else
    {
        cout << "\nPatient not found.\n";
    }
}


// ==========================================
// VIEW ALL PATIENTS
// ==========================================

void Receptionist::viewAllPatients(
    PatientManager& patientManager
) const
{
    cout << "\n========== ALL REGISTERED PATIENTS ==========\n";

    patientManager.displayAllPatients();
}


// ==========================================
// UPDATE PATIENT DETAILS
// ==========================================

void Receptionist::updatePatientDetails(
    PatientManager& patientManager
)
{
    string patientID;

    cout << "\nEnter Patient ID to update: ";

    getline(cin >> ws, patientID);

    if (patientManager.searchByID(patientID) == NULL)
    {
        cout << "\nPatient not found.\n";

        return;
    }

    patientManager.updatePatient(patientID);
}


// ==========================================
// MANAGE APPOINTMENT REQUESTS
// ==========================================

void Receptionist::manageAppointmentRequests(
    AppointmentManager& appointmentManager
) const
{
    int choice;

    do
    {
        displayAppointmentMenu();

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:

                appointmentManager.displayAppointmentRequests();

                break;

            case 2:
            {
                string requestID;

                cout << "Enter Request ID to confirm: ";
                cin >> requestID;

                if (appointmentManager.confirmAppointment(requestID))
                {
                    cout << "\nAppointment confirmed successfully.\n";
                }
                else
                {
                    cout << "\nUnable to confirm appointment.\n";
                }

                break;
            }

            case 3:
            {
                string requestID;

                cout << "Enter Request ID to reject: ";
                cin >> requestID;

                if (appointmentManager.rejectAppointment(requestID))
                {
                    cout << "\nAppointment request rejected.\n";
                }
                else
                {
                    cout << "\nUnable to reject request.\n";
                }

                break;
            }

            case 4:

                appointmentManager.displayAllAppointments();

                break;

            case 5:

                cout << "\nReturning to Receptionist Dashboard.\n";

                break;

            default:

                cout << "\nInvalid choice. Try again.\n";
        }

    } while (choice != 5);
}


// ==========================================
// VIEW ALL APPOINTMENTS
// ==========================================

void Receptionist::viewAppointments(
    AppointmentManager& appointmentManager
) const
{
    appointmentManager.displayAllAppointments();
}


// ==========================================
// VIEW EMERGENCY CASES
// ==========================================

void Receptionist::viewEmergencyCases(
    EmergencyManager& emergencyManager
) const
{
    int choice;

    do
    {
        displayEmergencyMenu();

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:

                emergencyManager.displayAllEmergencies();

                break;

            case 2:

                emergencyManager.displayEmergencyQueue();

                break;

            case 3:

                cout << "\nReturning to Receptionist Dashboard.\n";

                break;

            default:

                cout << "\nInvalid choice. Try again.\n";
        }

    } while (choice != 3);
}


// ==========================================
// VIEW AVAILABLE BEDS
// ==========================================

void Receptionist::viewAvailableBeds(
    BedManager& bedManager
) const
{
    cout << "\n========== AVAILABLE BEDS ==========\n";

    bedManager.displayAvailableBeds();
}