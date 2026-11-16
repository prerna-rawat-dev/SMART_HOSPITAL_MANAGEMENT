
#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iostream>
#include <string>

using namespace std;

struct Appointment
{
    string appointmentID;
    string patientID;
    string doctorID;
    string department;

    string appointmentDate;
    string appointmentTime;

    string reason;
    string status;

    Appointment* next;

    Appointment();
};

class AppointmentManager
{
private:
    Appointment* appointmentHead;
    Appointment* appointmentTail;

   
    int appointmentCount;
   

    int nextAppointmentNumber;
    

    void appendAppointment(Appointment* appointment);
    

    void clearAppointments();
    

    string generateAppointmentID();
   

    bool loadAppointments();
    bool saveAppointments() const;

    
   

    bool hasDoctorConflict(
        const string& doctorID,
        const string& date,
        const string& time,
        const string& excludedAppointmentID = ""
    ) const;

public:
    AppointmentManager();
    ~AppointmentManager();

    AppointmentManager(const AppointmentManager& other) = delete;
    AppointmentManager& operator=(const AppointmentManager& other) = delete;

    bool rescheduleAppointment(
        const string& appointmentID,
        const string& newDate,
        const string& newTime
    );

    bool cancelAppointment(const string& appointmentID);

    // Appointment search and display

    Appointment* searchAppointment(const string& appointmentID) const;

    void displayAppointment(const string& appointmentID) const;

    void displayAllAppointments() const;

    void displayPatientAppointments(const string& patientID) const;

    void displayDoctorAppointments(const string& doctorID) const;

    // Information

    int getAppointmentCount() const;


    bool createConfirmedAppointment(const Appointment& appointment);

    bool isDoctorAvailable(
        const string& doctorID,
        const string& date,
        const string& time
    ) const;
};

#endif
