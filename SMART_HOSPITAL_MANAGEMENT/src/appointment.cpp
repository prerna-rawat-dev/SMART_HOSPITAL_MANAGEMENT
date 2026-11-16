
#include "../include/appointment.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

// Split a line using |
int splitAppointmentLine(
    const string& line,
    string fields[],
    int maxFields)
{
    stringstream ss(line);
    string field;
    int count = 0;

    while (getline(ss, field, '|') && count < maxFields)
    {
        fields[count++] = field;
    }

    return count;
}

// Appointment constructor
Appointment::Appointment()
{
    next = NULL;
}

// AppointmentManager constructor
AppointmentManager::AppointmentManager()
{
    appointmentHead = NULL;
    appointmentTail = NULL;

    
    

    appointmentCount = 0;
    

    nextAppointmentNumber = 1;
    

    loadAppointments();
    
}

// Destructor
AppointmentManager::~AppointmentManager()
{
    saveAppointments();
    

    clearAppointments();
    
}

// Append confirmed appointment
void AppointmentManager::appendAppointment(Appointment* appointment)
{
    appointment->next = NULL;

    if (appointmentHead == NULL)
    {
        appointmentHead = appointmentTail = appointment;
    }
    else
    {
        appointmentTail->next = appointment;
        appointmentTail = appointment;
    }

    appointmentCount++;
}

// Clear confirmed appointments
void AppointmentManager::clearAppointments()
{
    Appointment* current = appointmentHead;

    while (current != NULL)
    {
        Appointment* temp = current;
        current = current->next;
        delete temp;
    }

    appointmentHead = NULL;
    appointmentTail = NULL;
    appointmentCount = 0;
}

// Generate appointment ID
string AppointmentManager::generateAppointmentID()
{
    string id = "APPT00000" + to_string(nextAppointmentNumber);
    nextAppointmentNumber++;

    return id;
}

// Check if doctor already has an appointment at this time
bool AppointmentManager::hasDoctorConflict(
    const string& doctorID,
    const string& date,
    const string& time,
    const string& excludedAppointmentID) const
{
    Appointment* current = appointmentHead;

    while (current != NULL)
    {
        if (current->doctorID == doctorID &&
            current->appointmentDate == date &&
            current->appointmentTime == time &&
            current->status == "Confirmed" &&
            current->appointmentID != excludedAppointmentID)
        {
            return true;
        }

        current = current->next;
    }

    return false;
}

// Save confirmed appointments
bool AppointmentManager::saveAppointments() const
{
    ofstream file("DATA/appointments.txt");

    if (!file)
        return false;

    Appointment* current = appointmentHead;

    while (current != NULL)
    {
        file << current->appointmentID << "|"
             << current->patientID << "|"
             << current->doctorID << "|"
             << current->department << "|"
             << current->appointmentDate << "|"
             << current->appointmentTime << "|"
             << current->reason << "|"
             << current->status << "\n";

        current = current->next;
    }

    file.close();
    return true;
}

// Load confirmed appointments
bool AppointmentManager::loadAppointments()
{
    ifstream file("DATA/appointments.txt");

    if (!file)
        return true;

    string line;

    while (getline(file, line))
    {
        string fields[8];

        if (splitAppointmentLine(line, fields, 8) != 8)
            continue;

        Appointment* appointment = new Appointment();

        appointment->appointmentID = fields[0];
        appointment->patientID = fields[1];
        appointment->doctorID = fields[2];
        appointment->department = fields[3];
        appointment->appointmentDate = fields[4];
        appointment->appointmentTime = fields[5];
        appointment->reason = fields[6];
        appointment->status = fields[7];

        if (appointment->appointmentID.length() > 4 &&
            appointment->appointmentID.substr(0, 4) == "APPT")
        {
            int number = atoi(
                appointment->appointmentID.substr(4).c_str()
            );

            if (number >= nextAppointmentNumber)
                nextAppointmentNumber = number + 1;
        }

        appendAppointment(appointment);
    }

    file.close();
    return true;
}

// Create a confirmed appointment after both approvals.
bool AppointmentManager::createConfirmedAppointment(const Appointment& appointment)
{
    if (appointment.patientID.empty() || appointment.doctorID.empty() ||
        appointment.appointmentDate.empty() || appointment.appointmentTime.empty())
        return false;

    if (hasDoctorConflict(appointment.doctorID, appointment.appointmentDate, appointment.appointmentTime))
    {
        cout << "Doctor already has a confirmed appointment in this time slot.\n";
        return false;
    }

    Appointment* item = new Appointment;
    item->appointmentID = generateAppointmentID();
    item->patientID = appointment.patientID;
    item->doctorID = appointment.doctorID;
    item->department = appointment.department;
    item->appointmentDate = appointment.appointmentDate;
    item->appointmentTime = appointment.appointmentTime;
    item->reason = appointment.reason;
    item->status = "Confirmed";
    appendAppointment(item);

    if (!saveAppointments())
    {
        // Remove the just-appended item if persistence fails.
        if (appointmentHead == item) appointmentHead = appointmentTail = NULL;
        else { Appointment* prev = appointmentHead; while (prev && prev->next != item) prev = prev->next;
               if (prev) { prev->next = NULL; appointmentTail = prev; } }
        appointmentCount--; delete item; return false;
    }
    cout << "Appointment confirmed. Appointment ID: " << item->appointmentID << endl;
    return true;
}

// Reschedule a confirmed appointment
bool AppointmentManager::rescheduleAppointment(
    const string& appointmentID,
    const string& newDate,
    const string& newTime)
{
    Appointment* appointment = searchAppointment(appointmentID);

    if (appointment == NULL ||
        appointment->status != "Confirmed" ||
        newDate.empty() ||
        newTime.empty())
    {
        return false;
    }

    if (hasDoctorConflict(
        appointment->doctorID,
        newDate,
        newTime,
        appointmentID))
    {
        cout << "\nDoctor is already booked at the new time.\n";
        return false;
    }

    string oldDate = appointment->appointmentDate;
    string oldTime = appointment->appointmentTime;

    appointment->appointmentDate = newDate;
    appointment->appointmentTime = newTime;

    if (!saveAppointments())
    {
        appointment->appointmentDate = oldDate;
        appointment->appointmentTime = oldTime;
        return false;
    }

    return true;
}

// Cancel a confirmed appointment
bool AppointmentManager::cancelAppointment(
    const string& appointmentID)
{
    Appointment* appointment = searchAppointment(appointmentID);

    if (appointment == NULL || appointment->status != "Confirmed")
        return false;

    appointment->status = "Cancelled";

    return saveAppointments();
}

// Search confirmed appointment
Appointment* AppointmentManager::searchAppointment(
    const string& appointmentID) const
{
    Appointment* current = appointmentHead;

    while (current != NULL)
    {
        if (current->appointmentID == appointmentID)
            return current;

        current = current->next;
    }

    return NULL;
}

// Display one appointment
void AppointmentManager::displayAppointment(
    const string& appointmentID) const
{
    Appointment* appointment = searchAppointment(appointmentID);

    if (appointment == NULL)
    {
        cout << "\nAppointment not found.\n";
        return;
    }

    cout << "\nAppointment ID: " << appointment->appointmentID;
    cout << "\nPatient ID: " << appointment->patientID;
    cout << "\nDoctor ID: " << appointment->doctorID;
    cout << "\nDepartment: " << appointment->department;
    cout << "\nDate: " << appointment->appointmentDate;
    cout << "\nTime: " << appointment->appointmentTime;
    cout << "\nReason: " << appointment->reason;
    cout << "\nStatus: " << appointment->status << "\n";
}

// Display all confirmed appointments and their history
void AppointmentManager::displayAllAppointments() const
{
    if (appointmentHead == NULL)
    {
        cout << "\nNo appointments found.\n";
        return;
    }

    Appointment* current = appointmentHead;

    while (current != NULL)
    {
        displayAppointment(current->appointmentID);
        cout << "-------------------------\n";

        current = current->next;
    }
}

// Display appointments of a patient
void AppointmentManager::displayPatientAppointments(
    const string& patientID) const
{
    Appointment* current = appointmentHead;
    bool found = false;

    while (current != NULL)
    {
        if (current->patientID == patientID)
        {
            displayAppointment(current->appointmentID);
            found = true;
        }

        current = current->next;
    }

    if (!found)
        cout << "\nNo appointments found for this patient.\n";
}

// Display appointments of a doctor
void AppointmentManager::displayDoctorAppointments(
    const string& doctorID) const
{
    Appointment* current = appointmentHead;
    bool found = false;

    while (current != NULL)
    {
        if (current->doctorID == doctorID &&
            current->status == "Confirmed")
        {
            displayAppointment(current->appointmentID);
            found = true;
        }

        current = current->next;
    }

    if (!found)
        cout << "\nNo confirmed appointments for this doctor.\n";
}

// Get appointment count
int AppointmentManager::getAppointmentCount() const
{
    return appointmentCount;
}

// Check doctor availability
bool AppointmentManager::isDoctorAvailable(
    const string& doctorID,
    const string& date,
    const string& time) const
{
    if (doctorID.empty() || date.empty() || time.empty())
        return false;

    return !hasDoctorConflict(doctorID, date, time);
}
