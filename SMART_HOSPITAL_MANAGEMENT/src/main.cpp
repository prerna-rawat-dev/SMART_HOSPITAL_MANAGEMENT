#include <iostream>
#include <string>

#include "../include/login.h"
#include "../include/admin.h"
#include "../include/graph.h"
#include "../include/receptionist.h"
#include "../include/doctorportal.h"
#include "../include/nurseportal.h"
#include "../include/patientportal.h"
#include "../include/emergency.h"
#include "../include/lab.h"
#include "../include/pharmacist.h"
#include "../include/resource.h"
#include "../include/bed.h"
#include "../include/appointment.h"
#include "../include/billing.h"
#include "../include/auditlog.h"
#include "../include/department.h"
#include "../include/doctor.h"
#include "../include/nurse.h"
#include "../include/patientmanager.h"
#include "../include/pendingassessment.h"
#include "../include/doctorassignment.h"
#include "../include/generalemergency.h"

using namespace std;

// Clear input buffer
void clearInput()
{
    cin.clear();
    cin.ignore(10000, '\n');
}

// ADMIN DASHBOARD
void runAdmin(
    Admin& admin,
    Login& loginManager,
    DoctorManager& doctorManager,
    NurseManager& nurseManager,
    DepartmentManager& departmentManager,
    ResourceManager& resourceManager,
    BedManager& bedManager,
    PatientManager& patientManager,
    AuditLogManager& auditManager,
    HospitalGraph& graphManager,
    DoctorAssignmentManager& assignmentManager
)
{
    int choice = 0;

    do
    {
        admin.displayDashboard();

        cout << "Enter choice: ";
        if (!(cin >> choice))
        {
            clearInput();
            cout << "\nInvalid input. Enter a number.\n";
            continue;
        }
        clearInput();

        switch (choice)
        {
            case 1:
                admin.manageStaff(
                    loginManager,
                    doctorManager,
                    nurseManager,
                    departmentManager
                );
                break;

            case 2:
                admin.manageDepartments(departmentManager);
                break;

            case 3:
                admin.manageResources(resourceManager);
                break;

            case 4:
                admin.manageBeds(bedManager);
                break;

            case 5:
                admin.managePatients(patientManager);
                break;

            case 6:
                admin.manageAuditLogs(auditManager);
                break;

            case 7:
                admin.manageGraph(graphManager);
                break;

            case 8:
            {
                int assignmentChoice = 0;

                do
                {
                    cout << "\n===== DOCTOR ASSIGNMENT & CAPACITY =====\n";
                    cout << "1. Assign Patient to Doctor\n";
                    cout << "2. Discharge Patient / Release Capacity\n";
                    cout << "3. View Doctor Capacity\n";
                    cout << "4. Back to Admin Dashboard\n";
                    cout << "Enter choice: ";

                    if (!(cin >> assignmentChoice))
                    {
                        clearInput();
                        cout << "Invalid input. Enter a number.\n";
                        assignmentChoice = 0;
                        continue;
                    }
                    clearInput();

                    if (assignmentChoice == 1)
                    {
                        string patientID, doctorID;
                        cout << "Enter Patient ID: ";
                        getline(cin, patientID);
                        cout << "Enter Doctor ID: ";
                        getline(cin, doctorID);

                        assignmentManager.assignPatient(
                            patientID, doctorID,
                            patientManager, doctorManager
                        );
                    }
                    else if (assignmentChoice == 2)
                    {
                        string patientID;
                        cout << "Enter Patient ID to discharge: ";
                        getline(cin, patientID);

                        assignmentManager.dischargePatient(
                            patientID, patientManager
                        );
                    }
                    else if (assignmentChoice == 3)
                    {
                        string doctorID;
                        cout << "Enter Doctor ID: ";
                        getline(cin, doctorID);

                        assignmentManager.displayDoctorCapacity(
                            doctorID, patientManager, doctorManager
                        );
                    }
                    else if (assignmentChoice != 4)
                    {
                        cout << "Invalid choice.\n";
                    }
                } while (assignmentChoice != 4);

                break;
            }

            case 9:
                cout << "\nAdmin logged out.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 9);
}

// RECEPTIONIST DASHBOARD
void runReceptionist(
    Receptionist& receptionist,
    PatientManager& patientManager,
    DoctorManager& doctorManager,
    AppointmentManager& appointmentManager,
    AppointmentRequestManager& appointmentRequestManager,
    EmergencyManager& emergencyManager,
    BedManager& bedManager,
    PendingAssessmentManager& assessmentManager,
    BillingManager& billingManager
)
{
    int choice = 0;

    do
    {
        receptionist.displayDashboard();

        cout << "Enter choice: ";
        if (!(cin >> choice))
        {
            clearInput();
            cout << "\nInvalid input. Enter a number.\n";
            continue;
        }

        switch (choice)
        {
            case 1:
            {
                int patientChoice = 0;

                do
                {
                    cout << "\n========== PATIENT MANAGEMENT ==========\n";
                    cout << "1. Register New Patient\n";
                    cout << "2. Search Patient\n";
                    cout << "3. View All Patients\n";
                    cout << "4. Update Patient Details\n";
                    cout << "5. Back\n";
                    cout << "Enter choice: ";

                    if (!(cin >> patientChoice))
                    {
                        clearInput();
                        cout << "\nInvalid input. Enter a number.\n";
                        continue;
                    }

                    switch (patientChoice)
                    {
                        case 1:
                            receptionist.registerNewPatient(
                                patientManager, doctorManager, assessmentManager
                            );
                            break;

                        case 2:
                            receptionist.searchPatient(patientManager);
                            break;

                        case 3:
                            receptionist.viewAllPatients(patientManager);
                            break;

                        case 4:
                            receptionist.updatePatientDetails(patientManager);
                            break;

                        case 5:
                            cout << "\nReturning to Receptionist Dashboard.\n";
                            break;

                        default:
                            cout << "\nInvalid choice. Try again.\n";
                    }

                } while (patientChoice != 5);

                break;
            }

            case 2:
                receptionist.viewAppointments(appointmentManager);
                break;

            case 3:
                receptionist.viewEmergencyCases(emergencyManager);
                break;

            case 4:
                receptionist.viewAvailableBeds(bedManager);
                break;

            case 5:
                receptionist.viewDoctorApprovedRequests(appointmentRequestManager);
                break;
            case 6:
                receptionist.reviewAppointmentRequests(appointmentRequestManager, appointmentManager, true);
                break;
            case 7:
                receptionist.reviewAppointmentRequests(appointmentRequestManager, appointmentManager, false);
                break;
            case 9:
    cout << "\nReceptionist logged out successfully.\n";
    break;
case 8:
{
    int billingChoice = 0;

    do
    {
        cout << "\n========== BILLING MANAGEMENT ==========\n";
        cout << "1. Create New Bill\n";
        cout << "2. Add Item to Existing Bill\n";
        cout << "3. View All Bills\n";
        cout << "4. Search Patient Bill\n";
        cout << "5. Record Payment\n";
        cout << "6. View Bill Breakdown\n";
        cout << "7. View Payment History\n";
        cout << "8. Back\n";
        cout << "Enter choice: ";

        if (!(cin >> billingChoice))
        {
            clearInput();
            cout << "Invalid input.\n";
            continue;
        }

        string patientID, billID;
        string itemName, category, paymentMode;
        double consultation, emergency, laboratory;
        double medicine, other, amount;
        int quantity;

        switch (billingChoice)
        {
            case 1:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                if (patientManager.searchByID(patientID) == NULL)
                {
                    cout << "Patient not found.\n";
                    break;
                }

                cout << "Consultation Charges: ";
                cin >> consultation;

                cout << "Emergency Charges: ";
                cin >> emergency;

                cout << "Laboratory Charges: ";
                cin >> laboratory;

                cout << "Medicine Charges: ";
                cin >> medicine;

                cout << "Other Charges: ";
                cin >> other;

                billingManager.createBill(
                    patientID,
                    consultation,
                    emergency,
                    laboratory,
                    medicine,
                    other
                );
                break;

            case 2:
                cout << "Enter Bill ID: ";
                cin >> billID;

                cout << "Enter Item Name: ";
                cin >> itemName;

                cout << "Enter Category: ";
                cin >> category;

                cout << "Enter Quantity: ";
                cin >> quantity;

                cout << "Enter Total Item Amount: ";
                cin >> amount;

                billingManager.addBillItem(
                    billID,
                    itemName,
                    category,
                    quantity,
                    amount
                );
                break;

            case 3:
                billingManager.displayAllBills();
                break;

            case 4:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                billingManager.displayPatientBills(patientID);
                break;

            case 5:
                cout << "Enter Bill ID: ";
                cin >> billID;

                cout << "Enter Payment Amount: ";
                cin >> amount;

                cout << "Payment Mode (Cash/UPI/Card): ";
                cin >> paymentMode;

                billingManager.makePayment(
                    billID,
                    amount,
                    paymentMode
                );
                break;

            case 6:
                cout << "Enter Bill ID: ";
                cin >> billID;

                billingManager.displayBillBreakdown(billID);
                break;

            case 7:
                cout << "Enter Bill ID: ";
                cin >> billID;

                billingManager.displayPaymentHistory(billID);
                break;

            case 8:
                cout << "Returning to Receptionist Dashboard.\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (billingChoice != 8);

    break;
}


            default:
                cout << "\nInvalid choice. Try again.\n";
        }

   } while (choice != 9);
}

// DOCTOR DASHBOARD
void runDoctor(
    DoctorPortal& doctorPortal,
    PatientManager& patientManager,
    AppointmentManager& appointmentManager,
    AppointmentRequestManager& appointmentRequestManager,
    LabManager& labManager,
    PharmacistManager& pharmacistManager,
    EmergencyManager& emergencyManager,
    BillingManager& billingManager
)
{
    int choice;
    string patientID;

    do
    {
        doctorPortal.displayDashboard();

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                doctorPortal.viewAssignedPatients(patientManager);
                break;

            case 2:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                doctorPortal.viewPatientRecords(
                    patientManager, patientID
                );
                break;

            case 3:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                doctorPortal.addMedicalRecord(
                    patientManager, patientID
                );
                break;

            case 4:
                doctorPortal.viewAppointments(appointmentManager);
                break;

            case 5:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                doctorPortal.orderLabTest(
                    patientManager, labManager, patientID
                );
                break;

            case 6:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                doctorPortal.viewLabReports(
                    labManager, patientID
                );
                break;

            case 7:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                doctorPortal.createPrescription(
                    patientManager, pharmacistManager, patientID
                );
                break;

            case 8:
                doctorPortal.viewEmergencyCases(emergencyManager);
                break;

            case 9:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                doctorPortal.recommendAdmission(
                    patientManager, patientID
                );
                break;

            case 10:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                doctorPortal.recommendDischarge(
                    patientManager, patientID
                );
                break;

            case 11:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                doctorPortal.viewPatientBill(
                    billingManager, patientID
                );
                break;

            case 13:
                doctorPortal.viewPendingAppointmentRequests(appointmentRequestManager);
                break;
            case 14:
                doctorPortal.decideAppointmentRequest(appointmentRequestManager, true);
                break;
            case 15:
                doctorPortal.decideAppointmentRequest(appointmentRequestManager, false);
                break;
            case 12:
                doctorPortal.logout();
                cout << "\nDoctor logged out.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 12);
}

// NURSE DASHBOARD
void runNurse(
    NursePortal& nursePortal,
    PatientManager& patientManager,
    NurseManager& nurseManager,
    BedManager& bedManager,
    ResourceManager& resourceManager
)
{
    (void)nurseManager;
    int choice;
    string patientID;
    string assignmentID;

    do
    {
        nursePortal.displayDashboard();

        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                nursePortal.viewAssignedPatients(patientManager);
                break;

            case 2:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                nursePortal.viewPatientDetails(
                    patientManager, patientID
                );
                break;

            case 3:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                nursePortal.recordVitals(
                    patientManager, patientID
                );
                break;

            case 4:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                nursePortal.viewMedicalInstructions(
                    patientManager, patientID
                );
                break;

            case 5:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                nursePortal.assignPatientBed(
                    patientManager, bedManager, patientID
                );
                break;

            case 6:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                nursePortal.releasePatientBed(
                    patientManager, bedManager, patientID
                );
                break;

            case 7:
                cout << "Enter Patient ID: ";
                cin >> patientID;

                nursePortal.assignEquipment(
                    patientManager, resourceManager, patientID
                );
                break;

            case 8:
                cout << "Enter Assignment ID: ";
                cin >> assignmentID;

                nursePortal.returnEquipment(
                    resourceManager, assignmentID
                );
                break;

            case 9:
                cout << "Enter Patient ID to discharge: ";
                cin >> patientID;

                nursePortal.dischargePatient(
                    patientManager, bedManager, patientID
                );
                break;

            case 10:
                nursePortal.displayNurseProfile();
                break;

            case 11:
                nursePortal.logout();
                
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 11);
}

// PATIENT PORTAL
void runPatientPortal(
    PatientPortal& portal,
    PatientManager& patientManager,
    AppointmentManager& appointmentManager,
    AppointmentRequestManager& appointmentRequestManager,
    LabManager& labManager,
    PharmacistManager& pharmacistManager,
    BillingManager& billingManager,
    HospitalGraph& graphManager
)
{
    string patientID, phone;
    int choice;

    cout << "\n========== PATIENT LOGIN ==========\n";

    cout << "Enter Patient ID: ";
    cin >> patientID;

    cout << "Enter Registered Phone: ";
    cin >> phone;

    if (!portal.authenticate(patientID, phone, patientManager))
    {
        cout << "\nPatient login failed.\n";
        return;
    }

    do
    {
        portal.displayDashboard();

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                portal.viewProfile();
                break;

            case 2:
                portal.viewVitalHistory();
                break;

            case 3:
                portal.viewMedicalHistory();
                break;

            case 4:
                portal.viewAppointments(appointmentManager);
                break;

            case 5:
                portal.bookAppointmentRequest(appointmentRequestManager);
                break;
            case 6:
                portal.viewAppointmentRequests(appointmentRequestManager);
                break;
            case 7:
                portal.viewLabReports(labManager);
                break;

            case 8:
                portal.viewPrescriptions(pharmacistManager);
                break;

            case 9:
                portal.viewBills(billingManager);
                break;

            case 10:
                portal.viewOutstandingAmount(billingManager);
                break;

            case 11:
            {
                int navigationChoice;

                do
                {
                    cout << "\n========== HOSPITAL NAVIGATION ==========\n";
                    cout << "1. View Hospital Locations\n";
                    cout << "2. Find Route Using BFS\n";
                    cout << "3. Find Shortest Route Using Dijkstra\n";
                    cout << "4. Back to Patient Dashboard\n";

                    cout << "Enter choice: ";
                    cin >> navigationChoice;

                    switch (navigationChoice)
                    {
                        case 1:
                            portal.viewHospitalLocations(graphManager);
                            break;

                        case 2:
                            portal.findHospitalRouteBFS(graphManager);
                            break;

                        case 3:
                            portal.findShortestHospitalRoute(graphManager);
                            break;

                        case 4:
                            cout << "\nReturning to Patient Dashboard.\n";
                            break;

                        default:
                            cout << "\nInvalid choice.\n";
                    }

                } while (navigationChoice != 4);

                break;
            }

            case 12:
                portal.logout();
                cout << "\nPatient logged out.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 12);
}

// GENERAL EMERGENCY ASSESSMENT DASHBOARD
void runGeneralEmergencyAssessment(
    GeneralEmergencyManager& generalEmergencyManager,
    PatientManager& patientManager,
    PendingAssessmentManager& assessmentManager,
    DoctorManager& doctorManager,
    DoctorAssignmentManager& assignmentManager,
    EmergencyManager& emergencyManager
)
{
    int choice = 0;

    do
    {
        generalEmergencyManager.displayDashboard();

        cout << "\nEnter choice: ";
        if (!(cin >> choice))
        {
            clearInput();
            cout << "\nInvalid input. Enter a number.\n";
            continue;
        }
        clearInput();

        switch (choice)
        {
            case 1:
                generalEmergencyManager.viewPendingPatients(assessmentManager);
                break;

            case 2:
                generalEmergencyManager.assessNextPatient(
                    patientManager,
                    assessmentManager,
                    doctorManager,
                    assignmentManager,
                    emergencyManager
                );
                break;

            case 3:
            {
                string visitID;
                cout << "Enter Visit ID: ";
                getline(cin, visitID);

                generalEmergencyManager.assessPatient(
                    visitID,
                    patientManager,
                    assessmentManager,
                    doctorManager,
                    assignmentManager,
                    emergencyManager
                );
                break;
            }

            case 4:
                generalEmergencyManager.viewAssessedPatients(assessmentManager);
                break;

            case 5:
                generalEmergencyManager.viewEmergencyPatients(emergencyManager);
                break;

            case 6:
                generalEmergencyManager.viewNonEmergencyPatients(assessmentManager);
                break;

            case 7:
                generalEmergencyManager.viewAssessmentHistory(assessmentManager);
                break;

            case 8:
                generalEmergencyManager.logout();
                cout << "\nGeneral Emergency Assessment Doctor logged out.\n";
                break;

            default:
                cout << "\nInvalid choice. Please select 1-8.\n";
        }

    } while (choice != 8 && generalEmergencyManager.isLoggedIn());
}

// EMERGENCY MANAGEMENT
void runEmergency(EmergencyManager& emergencyManager)
{
    int choice;
    string emergencyID, status;

    do
    {
        cout << "\n====== EMERGENCY MANAGEMENT ======\n";
        cout << "1. View Emergency Queue\n";
        cout << "2. View All Emergency Cases\n";
        cout << "3. Search Emergency Case\n";
        cout << "4. Update Emergency Status\n";
        cout << "5. Logout\n";

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                emergencyManager.displayEmergencyQueue();
                break;

            case 2:
                emergencyManager.displayAllEmergencies();
                break;

            case 3:
                cout << "Enter Emergency ID: ";
                cin >> emergencyID;

                emergencyManager.displayEmergency(emergencyID);
                break;

            case 4:
                cout << "Enter Emergency ID: ";
                cin >> emergencyID;

                cout << "Enter New Status: ";
                cin.ignore();
                getline(cin, status);

                if (emergencyManager.updateEmergencyStatus(
                    emergencyID, status))
                    cout << "Status updated.\n";
                else
                    cout << "Unable to update status.\n";

                break;

            case 5:
                cout << "Emergency staff logged out.\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 5);
}

// LABORATORY DASHBOARD
void runLaboratory(LabManager& labManager)
{
    int choice;
    string orderID, result, range, date, staffID, flag, status;

    do
    {
        cout << "\n========== LABORATORY DASHBOARD ==========\n";
        cout << "1. View All Test Records\n";
        cout << "2. View Pending Samples\n";
        cout << "3. Update Sample Status\n";
        cout << "4. Enter Test Result\n";
        cout << "5. Logout\n";

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                labManager.displayAllRecords();
                break;

            case 2:
                labManager.displayPendingSamples();
                break;

            case 3:
                cout << "Enter Order ID: ";
                cin >> orderID;

                cout << "Enter Status (Collected/Rejected): ";
                cin >> status;

                if (labManager.updateSampleStatus(orderID, status))
                    cout << "Sample status updated.\n";
                else
                    cout << "Unable to update sample status.\n";

                break;

            case 4:
                cout << "Enter Order ID: ";
                cin >> orderID;

                cout << "Enter Result: ";
                cin.ignore();
                getline(cin, result);

                cout << "Enter Reference Range: ";
                getline(cin, range);

                cout << "Enter Report Date: ";
                getline(cin, date);

                cout << "Enter Lab Staff ID: ";
                getline(cin, staffID);

                cout << "Enter Flag (Normal/Abnormal): ";
                getline(cin, flag);

                if (labManager.enterTestResult(
                    orderID, result, range, date, staffID, flag))
                    cout << "Test result saved.\n";
                else
                    cout << "Unable to save test result.\n";

                break;

            case 5:
                cout << "Laboratory staff logged out.\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 5);
}

// PHARMACIST DASHBOARD
void runPharmacist(
    PharmacistManager& pharmacistManager,
    const string& pharmacistID
)
{
    int choice;

    do
    {
        cout << "\n========== PHARMACIST DASHBOARD ==========\n";
        cout << "1. View All Medicines\n";
        cout << "2. Add New Medicine\n";
        cout << "3. Update Medicine Stock\n";
        cout << "4. View Low Stock Medicines\n";
        cout << "5. View Expired Medicines\n";
        cout << "6. Search Medicine\n";
        cout << "7. View Pending Prescriptions\n";
        cout << "8. Issue Pending Prescription\n";
        cout << "9. Logout\n";

        cout << "Enter choice: ";
        if (!(cin >> choice))
        {
            clearInput();
            cout << "Invalid input. Enter a number.\n";
            choice = 0;
            continue;
        }

        switch (choice)
        {
            case 1:
                pharmacistManager.displayAllMedicines();
                break;

            case 2:
            {
                Medicine medicine;

                cout << "\n========== ADD NEW MEDICINE ==========\n";
                cout << "Medicine Name: ";
                getline(cin >> ws, medicine.medicineName);

                cout << "Category: ";
                getline(cin, medicine.category);

                cout << "Quantity: ";
                if (!(cin >> medicine.quantity))
                {
                    clearInput();
                    cout << "Invalid quantity.\n";
                    break;
                }

                cout << "Minimum Stock: ";
                if (!(cin >> medicine.minimumStock))
                {
                    clearInput();
                    cout << "Invalid minimum stock.\n";
                    break;
                }

                cout << "Price: ";
                if (!(cin >> medicine.price))
                {
                    clearInput();
                    cout << "Invalid price.\n";
                    break;
                }

                cout << "Expiry Date (YYYY-MM-DD): ";
                cin >> medicine.expiryDate;

                pharmacistManager.addMedicine(medicine);
                break;
            }

            case 3:
            {
                string medicineID;
                int newQuantity;

                cout << "Enter Medicine ID: ";
                cin >> medicineID;

                cout << "Enter new stock quantity: ";
                if (!(cin >> newQuantity))
                {
                    clearInput();
                    cout << "Invalid quantity.\n";
                    break;
                }

                pharmacistManager.updateMedicineStock(
                    medicineID, newQuantity
                );
                break;
            }

            case 4:
                pharmacistManager.displayLowStockMedicines();
                break;

            case 5:
                pharmacistManager.displayExpiredMedicines();
                break;

            case 6:
            {
                string medicineID;
                cout << "Enter Medicine ID: ";
                cin >> medicineID;
                pharmacistManager.displayMedicine(medicineID);
                break;
            }

            case 7:
                pharmacistManager.displayPendingPrescriptions();
                break;

            case 8:
            {
                string prescriptionID;
                cout << "Enter Pending Prescription ID: ";
                cin >> prescriptionID;

                pharmacistManager.issuePendingPrescription(
                    prescriptionID, pharmacistID
                );
                break;
            }

            case 9:
                cout << "Pharmacist logged out.\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 9);
}

// MAIN FUNCTION
int main()
{
    // Create all managers only once
    Login loginManager;

    PatientManager patientManager;
    DoctorManager doctorManager;
    NurseManager nurseManager;

    DepartmentManager departmentManager;
    Admin admin;
    Receptionist receptionist;

    DoctorPortal doctorPortal;
    NursePortal nursePortal;
    PatientPortal patientPortal;

    EmergencyManager emergencyManager;
    PendingAssessmentManager assessmentManager;
    GeneralEmergencyManager generalEmergencyManager;
    LabManager labManager;
    PharmacistManager pharmacistManager;

    ResourceManager resourceManager;
    BedManager bedManager;
    AppointmentManager appointmentManager;
    AppointmentRequestManager appointmentRequestManager;
    BillingManager billingManager;
    AuditLogManager auditManager;

    // Hospital Navigation Graph
    HospitalGraph graphManager;

    // Doctor assignment and active-patient capacity manager
    DoctorAssignmentManager assignmentManager;

    // Create initial Admin if there are no accounts.
    // If accounts already exist, the function will not create another.
    loginManager.createInitialAdmin();

    int choice;
    Account loggedInAccount;

    do
    {
        cout << "\n";
        cout << "============================================\n";
        cout << "       SMART HOSPITAL MANAGEMENT SYSTEM\n";
        cout << "============================================\n";

        cout << "1. Admin / HR\n";
        cout << "2. Receptionist\n";
        cout << "3. Doctor\n";
        cout << "4. Patient Portal\n";
        cout << "5. Nurse\n";
        cout << "6. Emergency Management\n";
        cout << "7. Laboratory Staff\n";
        cout << "8. Pharmacist\n";
        cout << "9. General Emergency Assessment\n";
        cout << "10. Exit\n";

        cout << "============================================\n";
        cout << "Select your role: ";

        if (!(cin >> choice))
        {
            clearInput();
            cout << "\nInvalid input. Enter a number.\n";
            continue;
        }

        clearInput();

        switch (choice)
        {
            case 1:
                if (loginManager.authenticate("Admin", loggedInAccount))
                {
                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Admin logged in"
                    );

                    runAdmin(
                        admin, loginManager,
                        doctorManager, nurseManager,
                        departmentManager, resourceManager,
                        bedManager, patientManager, auditManager,
                        graphManager, assignmentManager
                    );

                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Admin logged out"
                    );
                }
                break;

            case 2:
                if (loginManager.authenticate(
                    "Receptionist", loggedInAccount))
                {
                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Receptionist logged in"
                    );

                    runReceptionist(
                        receptionist,
                        patientManager,
                        doctorManager,
                        appointmentManager,
                        appointmentRequestManager,
                        emergencyManager,
                        bedManager,
                        assessmentManager,
                        billingManager
                    );

                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Receptionist logged out"
                    );
                }
                break;

            case 3:
                if (loginManager.authenticate("Doctor", loggedInAccount))
                {
                    Doctor* doctor =
                        doctorManager.searchByID(loggedInAccount.id);

                    if (doctor == NULL)
                    {
                        cout << "\nNo doctor record matches this account ID.\n";
                        break;
                    }

                    doctorPortal.login(loggedInAccount.id);

                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Doctor logged in"
                    );

                    runDoctor(
                        doctorPortal, patientManager,
                        appointmentManager, appointmentRequestManager, labManager,
                        pharmacistManager, emergencyManager,
                        billingManager
                    );

                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Doctor logged out"
                    );
                }
                break;

            case 4:
                runPatientPortal(
                    patientPortal, patientManager,
                    appointmentManager, appointmentRequestManager, labManager,
                    pharmacistManager, billingManager,
                    graphManager
                );
                break;

            case 5:
                if (loginManager.authenticate("Nurse", loggedInAccount))
                {
                    if (nursePortal.login(
                        loggedInAccount.id, nurseManager))
                    {
                        auditManager.addLog(
                            loggedInAccount.id,
                            loggedInAccount.role,
                            "Nurse logged in"
                        );

                        runNurse(
                            nursePortal, patientManager,
                            nurseManager, bedManager,
                            resourceManager
                        );

                        auditManager.addLog(
                            loggedInAccount.id,
                            loggedInAccount.role,
                            "Nurse logged out"
                        );
                    }
                    else
                    {
                        cout << "\nNo nurse record matches this account ID.\n";
                    }
                }
                break;

            case 6:
                if (loginManager.authenticate(
                    "Emergency Management", loggedInAccount))
                {
                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Emergency staff logged in"
                    );

                    runEmergency(emergencyManager);

                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Emergency staff logged out"
                    );
                }
                break;

            case 7:
                if (loginManager.authenticate(
                    "Laboratory Staff", loggedInAccount))
                {
                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Laboratory staff logged in"
                    );

                    runLaboratory(labManager);

                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Laboratory staff logged out"
                    );
                }
                break;

            case 8:
                if (loginManager.authenticate(
                    "Pharmacist", loggedInAccount))
                {
                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Pharmacist logged in"
                    );

                    runPharmacist(pharmacistManager, loggedInAccount.id);

                    auditManager.addLog(
                        loggedInAccount.id,
                        loggedInAccount.role,
                        "Pharmacist logged out"
                    );
                }
                break;

            case 9:
                if (generalEmergencyManager.login(loginManager))
                {
                    // Preserve the ID because logout() clears the manager's session.
                    const string assessmentDoctorID =
                        generalEmergencyManager.getLoggedInDoctorID();

                    auditManager.addLog(
                        assessmentDoctorID,
                        "General Emergency Assessment",
                        "General Emergency Assessment Doctor logged in"
                    );

                    runGeneralEmergencyAssessment(
                        generalEmergencyManager,
                        patientManager,
                        assessmentManager,
                        doctorManager,
                        assignmentManager,
                        emergencyManager
                    );

                    auditManager.addLog(
                        assessmentDoctorID,
                        "General Emergency Assessment",
                        "General Emergency Assessment Doctor logged out"
                    );
                }
                break;

            case 10:
                cout << "\nThank you for using Smart Hospital Management System.\n";
                break;

            default:
                cout << "\nInvalid role selection.\n";
        }

    } while (choice != 10);

    return 0;
}