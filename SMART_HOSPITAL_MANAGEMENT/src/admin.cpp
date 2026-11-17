#include "../include/admin.h"

// Constructor
Admin::Admin()
{
}

// ==================================================
// MAIN ADMIN DASHBOARD
// ==================================================

void Admin::displayDashboard() const
{
    cout << "\n========== ADMIN DASHBOARD ==========\n";
    cout << "1. Staff Management\n";
    cout << "2. Department Management\n";
    cout << "3. Resource Management\n";
    cout << "4. Bed Management\n";
    cout << "5. Patient Records\n";
    cout << "6. Audit Logs\n";
    cout << "7. Hospital Navigation Graph\n";
    cout << "8. Doctor Assignment & Capacity\n";
    cout << "9. Logout\n";
    cout << "=====================================\n";
}

// ==================================================
// STAFF MENU
// ==================================================

void Admin::displayStaffMenu() const
{
    cout << "\n========== STAFF MANAGEMENT ==========\n";
    cout << "1. View Staff Accounts\n";
    cout << "2. View All Doctors\n";
    cout << "3. View All Nurses\n";
    cout << "4. Register Doctor\n";
    cout << "5. Register Nurse\n";
    cout << "6. Manage Doctor Type & Availability\n";
    cout << "7. Register General Emergency Assessment Doctor\n";
    cout << "8. Back\n";
    cout << "======================================\n";
}

// ==================================================
// DEPARTMENT MENU
// ==================================================

void Admin::displayDepartmentMenu() const
{
    cout << "\n========== DEPARTMENT MANAGEMENT ==========\n";
    cout << "1. View All Departments\n";
    cout << "2. Back\n";
    cout << "===========================================\n";
}

// ==================================================
// RESOURCE MENU
// ==================================================

void Admin::displayResourceMenu() const
{
    cout << "\n========== RESOURCE MANAGEMENT ==========\n";
    cout << "1. Add Resource\n";
    cout << "2. View All Resources\n";
    cout << "3. View Available Resources\n";
    cout << "4. Back\n";
    cout << "=========================================\n";
}

// ==================================================
// BED MENU
// ==================================================

void Admin::displayBedMenu() const
{
    cout << "\n========== BED MANAGEMENT ==========\n";
    cout << "1. Add Bed\n";
    cout << "2. View All Beds\n";
    cout << "3. View Available Beds\n";
    cout << "4. Back\n";
    cout << "====================================\n";
}

// ==================================================
// PATIENT MENU
// ==================================================

void Admin::displayPatientMenu() const
{
    cout << "\n========== PATIENT RECORDS ==========\n";
    cout << "1. View All Patients\n";
    cout << "2. Back\n";
    cout << "=====================================\n";
}

// ==================================================
// AUDIT MENU
// ==================================================

void Admin::displayAuditMenu() const
{
    cout << "\n========== AUDIT LOGS ==========\n";
    cout << "1. View All Logs\n";
    cout << "2. Back\n";
    cout << "===============================\n";
}

// ==================================================
// GRAPH MENU
// ==================================================

void Admin::displayGraphMenu() const
{
    cout << "\n========== HOSPITAL NAVIGATION GRAPH ==========\n";
    cout << "1. Add Location\n";
    cout << "2. Add Route Between Locations\n";
    cout << "3. Display All Locations\n";
    cout << "4. Display All Routes\n";
    cout << "5. Find Route Using BFS (Minimum Number of Routes)\n";
    cout << "6. Find Shortest Route Using Dijkstra (Minimum Distance)\n";
    cout << "7. Back\n";
    cout << "================================================\n";
}

// ==================================================
// REGISTER DOCTOR
// ==================================================

void Admin::registerDoctor(
    Login& loginManager,
    DoctorManager& doctorManager,
    DepartmentManager& departmentManager
)
{
    Doctor doctor;
    string password;

    int typeChoice = 0;
    int availabilityChoice = 0;

    cout << "\n========== DOCTOR REGISTRATION ==========\n";

    cout << "Enter Doctor Name: ";
    getline(cin >> ws, doctor.name);

    cout << "Enter Username: ";
    getline(cin, doctor.username);

    cout << "Enter Password: ";
    getline(cin, password);

    cout << "\nAvailable Departments:\n";
    departmentManager.displayDepartments();

    cout << "\nEnter Department Name: ";
    getline(cin, doctor.department);

    if (!departmentManager.departmentExists(doctor.department))
    {
        cout << "\nInvalid department.\n";
        return;
    }

    cout << "Enter Specialization: ";
    getline(cin, doctor.specialization);

    cout << "Enter Phone Number: ";
    getline(cin, doctor.phone);

    cout << "\nSelect Doctor Type:\n";
    cout << "1. Emergency Doctor\n";
    cout << "2. Non-Emergency Doctor\n";
    cout << "Enter choice: ";

    if (!(cin >> typeChoice) ||
        (typeChoice != 1 && typeChoice != 2))
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\nInvalid doctor type.\n";
        return;
    }

    doctor.doctorType =
        (typeChoice == 1) ? "Emergency" : "Non-Emergency";

    cout << "\nSelect Availability:\n";
    cout << "1. Available\n";
    cout << "2. Unavailable\n";
    cout << "Enter choice: ";

    if (!(cin >> availabilityChoice) ||
        (availabilityChoice != 1 && availabilityChoice != 2))
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\nInvalid availability.\n";
        return;
    }

    doctor.availability =
        (availabilityChoice == 1) ? "Available" : "Unavailable";

    if (!doctorManager.registerDoctor(doctor))
    {
        cout << "\nDoctor registration failed.\n";
        return;
    }

    Doctor* savedDoctor =
        doctorManager.searchByName(doctor.name);

    if (savedDoctor == NULL)
    {
        cout << "\nDoctor registered, but doctor ID could not be retrieved.\n";
        return;
    }

    Account account(
        savedDoctor->doctorID,
        savedDoctor->name,
        savedDoctor->username,
        password,
        "Doctor",
        savedDoctor->department
    );

    if (!loginManager.createAccount(account))
    {
        cout << "\nDoctor record was saved, but login account creation failed.\n";
        cout << "Please contact the administrator.\n";
        return;
    }

    cout << "\nDoctor registered successfully!\n";
    cout << "Doctor ID: " << savedDoctor->doctorID << '\n';
    cout << "Username: " << savedDoctor->username << '\n';
    cout << "Doctor Type: " << savedDoctor->doctorType << '\n';
    cout << "Availability: " << savedDoctor->availability << '\n';
}

// ==================================================
// REGISTER NURSE
// ==================================================

void Admin::registerNurse(
    Login& loginManager,
    NurseManager& nurseManager,
    DoctorManager& doctorManager,
    DepartmentManager& departmentManager
)
{
    Nurse nurse;
    string password;
    string assignedDoctorID;

    cout << "\n========== NURSE REGISTRATION ==========\n";

    cout << "Enter Nurse Name: ";
    getline(cin >> ws, nurse.name);

    cout << "Enter Username: ";
    getline(cin, nurse.username);

    cout << "Enter Password: ";
    getline(cin, password);

    cout << "\nAvailable Departments:\n";
    departmentManager.displayDepartments();

    cout << "\nEnter Department Name: ";
    getline(cin, nurse.department);

    if (!departmentManager.departmentExists(nurse.department))
    {
        cout << "\nInvalid department.\n";
        return;
    }

    cout << "\nEnter Assigned Doctor ID: ";
    getline(cin, assignedDoctorID);

    Doctor* assignedDoctor =
        doctorManager.searchByID(assignedDoctorID);

    if (assignedDoctor == NULL)
    {
        cout << "\nDoctor ID not found. Register the doctor first.\n";
        return;
    }

    if (assignedDoctor->department != nurse.department)
    {
        cout << "\nThe nurse and assigned doctor must belong to the same department.\n";
        return;
    }

    nurse.assignedDoctorID = assignedDoctor->doctorID;
    nurse.assignedDoctorName = assignedDoctor->name;

    cout << "Enter Phone Number: ";
    getline(cin, nurse.phone);

    cout << "Enter Availability: ";
    getline(cin, nurse.availability);

    if (!nurseManager.registerNurse(nurse))
    {
        cout << "\nNurse registration failed.\n";
        return;
    }

    Nurse* savedNurse =
        nurseManager.searchByName(nurse.name);

    if (savedNurse == NULL)
    {
        cout << "\nNurse registered, but nurse ID could not be retrieved.\n";
        return;
    }

    Account account(
        savedNurse->nurseID,
        savedNurse->name,
        savedNurse->username,
        password,
        "Nurse",
        savedNurse->department
    );

    if (!loginManager.createAccount(account))
    {
        cout << "\nNurse record was saved, but login account creation failed.\n";
        cout << "Please contact the administrator.\n";
        return;
    }

    cout << "\nNurse registered successfully!\n";
    cout << "Nurse ID: " << savedNurse->nurseID << '\n';
    cout << "Assigned Doctor: " << savedNurse->assignedDoctorName << '\n';
    cout << "Username: " << savedNurse->username << '\n';
}

// ==================================================
// MANAGE DOCTOR TYPE AND AVAILABILITY
// ==================================================

void Admin::updateDoctorEmergencyDesignation(
    DoctorManager& doctorManager
)
{
    string doctorID;
    string doctorType;
    string availability;

    int typeChoice = 0;
    int availabilityChoice = 0;

    cout << "\n===== DOCTOR TYPE & AVAILABILITY MANAGEMENT =====\n";

    doctorManager.displayAllDoctors();

    cout << "\nEnter Doctor ID: ";
    getline(cin >> ws, doctorID);

    Doctor* doctor =
        doctorManager.searchByID(doctorID);

    if (doctor == NULL)
    {
        cout << "\nDoctor not found.\n";
        return;
    }

    cout << "\nDoctor: " << doctor->name;
    cout << "\nDepartment: " << doctor->department << '\n';

    cout << "\nSelect Doctor Type:\n";
    cout << "1. Emergency\n";
    cout << "2. Non-Emergency\n";
    cout << "Enter choice: ";

    if (!(cin >> typeChoice) ||
        (typeChoice != 1 && typeChoice != 2))
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\nInvalid doctor type.\n";
        return;
    }

    doctorType =
        (typeChoice == 1) ? "Emergency" : "Non-Emergency";

    cout << "\nSelect Manual Availability:\n";
    cout << "1. Available\n";
    cout << "2. Unavailable\n";
    cout << "Enter choice: ";

    if (!(cin >> availabilityChoice) ||
        (availabilityChoice != 1 && availabilityChoice != 2))
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\nInvalid availability.\n";
        return;
    }

    availability =
        (availabilityChoice == 1) ? "Available" : "Unavailable";

    if (doctorManager.updateDoctorTypeAndAvailability(
            doctorID,
            doctorType,
            availability))
    {
        cout << "\nDoctor details updated successfully!\n";
        cout << "Doctor Type: " << doctorType << '\n';
        cout << "Manual Availability: " << availability << '\n';
    }
    else
    {
        cout << "\nUnable to update doctor details.\n";
    }
}

// ==================================================
// REGISTER GENERAL EMERGENCY ASSESSMENT DOCTOR
// ==================================================

void Admin::registerGeneralEmergencyAssessmentDoctor(
    Login& loginManager
)
{
    string doctorName;
    string username;
    string password;
    string confirmPassword;

    cout << "\n===== GENERAL EMERGENCY ASSESSMENT DOCTOR REGISTRATION =====\n";

    cout << "Enter Doctor Name: ";
    getline(cin >> ws, doctorName);

    cout << "Enter Username: ";
    getline(cin, username);

    cout << "Enter Password: ";
    getline(cin, password);

    cout << "Confirm Password: ";
    getline(cin, confirmPassword);

    if (doctorName.empty() || username.empty() || password.empty())
    {
        cout << "\nAll fields are required.\n";
        return;
    }

    if (password != confirmPassword)
    {
        cout << "\nPasswords do not match.\n";
        return;
    }

    const string fields[3] =
    {
        doctorName,
        username,
        password
    };

    for (int i = 0; i < 3; ++i)
    {
        if (fields[i].find('|') != string::npos ||
            fields[i].find('\n') != string::npos ||
            fields[i].find('\r') != string::npos)
        {
            cout << "\nInvalid character. | and line breaks are not allowed.\n";
            return;
        }
    }

    if (loginManager.usernameExists(username))
    {
        cout << "\nUsername already exists.\n";
        return;
    }

    string accountID = "GEA_" + username;

    Account assessmentDoctorAccount(
        accountID,
        doctorName,
        username,
        password,
        "General Emergency Assessment",
        "General Emergency Assessment"
    );

    if (!loginManager.createAccount(assessmentDoctorAccount))
    {
        cout << "\nGeneral Emergency Assessment Doctor registration failed.\n";
        return;
    }

    cout << "\n===============================================\n";
    cout << "GENERAL EMERGENCY ASSESSMENT DOCTOR REGISTERED\n";
    cout << "===============================================\n";

    cout << "Account ID: " << accountID << '\n';
    cout << "Doctor Name: " << doctorName << '\n';
    cout << "Username: " << username << '\n';
    cout << "Role: General Emergency Assessment\n";

    cout << "\nThis account is separate from specialist doctors.\n";
}

// ==================================================
// MANAGE STAFF
// ==================================================

void Admin::manageStaff(
    Login& loginManager,
    DoctorManager& doctorManager,
    NurseManager& nurseManager,
    DepartmentManager& departmentManager
)
{
    int choice = 0;

    do
    {
        displayStaffMenu();

        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\nInvalid input.\n";
            continue;
        }

        switch (choice)
        {
            case 1:
                loginManager.displayAccounts();
                break;

            case 2:
                doctorManager.displayAllDoctors();
                break;

            case 3:
                nurseManager.displayAllNurses();
                break;

            case 4:
                registerDoctor(
                    loginManager,
                    doctorManager,
                    departmentManager
                );
                break;

            case 5:
                registerNurse(
                    loginManager,
                    nurseManager,
                    doctorManager,
                    departmentManager
                );
                break;

            case 6:
                updateDoctorEmergencyDesignation(doctorManager);
                break;

            case 7:
                registerGeneralEmergencyAssessmentDoctor(loginManager);
                break;

            case 8:
                cout << "\nReturning to Admin Dashboard.\n";
                break;

            default:
                cout << "\nInvalid choice. Try again.\n";
        }

    } while (choice != 8);
}

// ==================================================
// MANAGE DEPARTMENTS
// ==================================================

void Admin::manageDepartments(
    DepartmentManager& departmentManager
) const
{
    int choice = 0;

    do
    {
        displayDepartmentMenu();

        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\nInvalid input.\n";
            continue;
        }

        switch (choice)
        {
            case 1:
                departmentManager.displayDepartments();
                break;

            case 2:
                cout << "\nReturning to Admin Dashboard.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 2);
}

// ==================================================
// MANAGE RESOURCES
// ==================================================

void Admin::manageResources(
    ResourceManager& resourceManager
)
{
    int choice = 0;

    do
    {
        displayResourceMenu();

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
            {
                Resource resource;

                cout << "\nEnter Resource Name: ";
                getline(cin >> ws, resource.resourceName);

                cout << "Enter Category: ";
                getline(cin, resource.category);

                cout << "Enter Total Quantity: ";
                cin >> resource.totalQuantity;

                cout << "Enter Location: ";
                getline(cin >> ws, resource.location);

                resource.availableQuantity = resource.totalQuantity;
                resource.status = "Available";

                if (resourceManager.addResource(resource))
                    cout << "\nResource added successfully.\n";
                else
                    cout << "\nUnable to add resource.\n";

                break;
            }

            case 2:
                resourceManager.displayAllResources();
                break;

            case 3:
                resourceManager.displayAvailableResources();
                break;

            case 4:
                cout << "\nReturning to Admin Dashboard.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 4);
}

// ==================================================
// MANAGE BEDS
// ==================================================

void Admin::manageBeds(BedManager& bedManager)
{
    int choice = 0;

    do
    {
        displayBedMenu();

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
            {
                Bed bed;

                cout << "\nEnter Room ID: ";
                getline(cin >> ws, bed.roomID);

                cout << "Enter Department: ";
                getline(cin, bed.department);

                cout << "Enter Bed Category (General/ICU/Emergency/OT): ";
                getline(cin, bed.category);

                bed.status = "Available";
                bed.patientID = "";

                if (bedManager.addBed(bed))
                    cout << "\nBed added successfully.\n";
                else
                    cout << "\nUnable to add bed.\n";

                break;
            }

            case 2:
                bedManager.displayAllBeds();
                break;

            case 3:
                bedManager.displayAvailableBeds();
                break;

            case 4:
                cout << "\nReturning to Admin Dashboard.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 4);
}

// ==================================================
// MANAGE PATIENTS
// ==================================================

void Admin::managePatients(
    PatientManager& patientManager
) const
{
    int choice = 0;

    do
    {
        displayPatientMenu();

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                patientManager.displayAllPatients();
                break;

            case 2:
                cout << "\nReturning to Admin Dashboard.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 2);
}

// ==================================================
// MANAGE AUDIT LOGS
// ==================================================

void Admin::manageAuditLogs(
    AuditLogManager& auditManager
) const
{
    int choice = 0;

    do
    {
        displayAuditMenu();

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                auditManager.displayAllLogs();
                break;

            case 2:
                cout << "\nReturning to Admin Dashboard.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 2);
}

// ==================================================
// MANAGE HOSPITAL NAVIGATION GRAPH
// ==================================================

void Admin::manageGraph(HospitalGraph& graphManager)
{
    int choice = 0;

    do
    {
        displayGraphMenu();

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
            {
                string locationID;
                string locationName;

                cout << "\nEnter Location ID (example: LOC001): ";
                getline(cin >> ws, locationID);

                cout << "Enter Location Name: ";
                getline(cin, locationName);

                if (graphManager.addLocation(locationID, locationName))
                    cout << "\nLocation added successfully.\n";
                else
                    cout << "\nUnable to add location.\n";

                break;
            }

            case 2:
            {
                string sourceID;
                string destinationID;
                int distance = 0;

                cout << "\nEnter Source Location ID: ";
                getline(cin >> ws, sourceID);

                cout << "Enter Destination Location ID: ";
                getline(cin, destinationID);

                cout << "Enter Route Distance in metres: ";
                cin >> distance;

                if (graphManager.addRoute(
                        sourceID,
                        destinationID,
                        distance))
                {
                    cout << "\nRoute added successfully.\n";
                }
                else
                {
                    cout << "\nUnable to add route.\n";
                }

                break;
            }

            case 3:
                graphManager.displayLocations();
                break;

            case 4:
                graphManager.displayRoutes();
                break;

            case 5:
            {
                string sourceID;
                string destinationID;

                cout << "\nEnter Starting Location ID: ";
                getline(cin >> ws, sourceID);

                cout << "Enter Destination Location ID: ";
                getline(cin, destinationID);

                graphManager.findRouteBFS(sourceID, destinationID);

                break;
            }

            case 6:
            {
                string sourceID;
                string destinationID;

                cout << "\nEnter Starting Location ID: ";
                getline(cin >> ws, sourceID);

                cout << "Enter Destination Location ID: ";
                getline(cin, destinationID);

                graphManager.findShortestRoute(sourceID, destinationID);

                break;
            }

            case 7:
                cout << "\nReturning to Admin Dashboard.\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 7);
}