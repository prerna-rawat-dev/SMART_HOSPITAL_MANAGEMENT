#ifndef ADMIN_H
#define ADMIN_H

#include <iostream>
#include <string>

#include "login.h"
#include "patientmanager.h"
#include "department.h"
#include "doctor.h"
#include "nurse.h"
#include "resource.h"
#include "bed.h"
#include "auditlog.h"
#include "graph.h"

using namespace std;

class Admin
{
private:

    // Submenus
    void displayStaffMenu() const;
    void displayDepartmentMenu() const;
    void displayResourceMenu() const;
    void displayBedMenu() const;
    void displayPatientMenu() const;
    void displayAuditMenu() const;
    void displayGraphMenu() const;

    // Staff registration
    void registerDoctor(
        Login& loginManager,
        DoctorManager& doctorManager,
        DepartmentManager& departmentManager
    );

    void registerNurse(
        Login& loginManager,
        NurseManager& nurseManager,
        DoctorManager& doctorManager,
        DepartmentManager& departmentManager
    );

    // Existing emergency doctor designation
    void updateDoctorEmergencyDesignation(
        DoctorManager& doctorManager
    );

public:

    Admin();

    // Main Admin Dashboard
    void displayDashboard() const;

    // NEW: Register General Emergency Assessment Doctor
    void registerGeneralEmergencyAssessmentDoctor(
        Login& loginManager
    );

    // Staff Management
    void manageStaff(
        Login& loginManager,
        DoctorManager& doctorManager,
        NurseManager& nurseManager,
        DepartmentManager& departmentManager
    );

    // Department Management
    void manageDepartments(
        DepartmentManager& departmentManager
    ) const;

    // Resource Management
    void manageResources(
        ResourceManager& resourceManager
    );

    // Bed Management
    void manageBeds(
        BedManager& bedManager
    );

    // Patient Management
    void managePatients(
        PatientManager& patientManager
    ) const;

    // Audit Management
    void manageAuditLogs(
        AuditLogManager& auditManager
    ) const;

    // Hospital Navigation Graph
    void manageGraph(
        HospitalGraph& graphManager
    );

};

#endif
