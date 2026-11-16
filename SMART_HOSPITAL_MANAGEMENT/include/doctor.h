#ifndef DOCTOR_H
#define DOCTOR_H

#include <iostream>
#include <string>

using namespace std;

const int MAX_PATIENTS_PER_DOCTOR = 5;

struct Doctor
{
    string doctorID;
    string name;
    string username;
    string department;
    string specialization;
    string phone;

    // Emergency / Non-Emergency
    string doctorType;

    // Manually controlled: Available / Unavailable
    string availability;

    Doctor* next;

    Doctor();
};

class DoctorManager
{
private:
    Doctor* head;
    Doctor* tail;

    int doctorCount;
    int nextDoctorNumber;

    void appendDoctor(Doctor* doctor);
    void clearDoctors();

    bool loadDoctors();
    bool saveDoctors() const;

    string generateDoctorID();

public:
    DoctorManager();
    ~DoctorManager();

    DoctorManager(const DoctorManager&) = delete;
    DoctorManager& operator=(const DoctorManager&) = delete;

    bool registerDoctor(const Doctor& doctor);

    Doctor* searchByID(const string& doctorID) const;
    Doctor* searchByName(const string& name) const;

    void displayDoctor(const string& doctorID) const;
    void displayAllDoctors() const;
    void displayDoctorsByDepartment(const string& department) const;

    bool updateDoctorTypeAndAvailability(
        const string& doctorID,
        const string& doctorType,
        const string& availability
    );

    void displayEmergencyDoctorsByDepartment(
        const string& department
    ) const;

    void displayAllEmergencyDoctors() const;

    bool isEligibleEmergencyDoctor(
        const string& doctorID,
        const string& department,
        int activePatientCount
    ) const;

    int getDoctorCount() const;

    string getEffectiveAvailability(
        const string& doctorID,
        int activePatientCount
    ) const;

    bool canAcceptPatient(
        const string& doctorID,
        int activePatientCount
    ) const;
};

#endif