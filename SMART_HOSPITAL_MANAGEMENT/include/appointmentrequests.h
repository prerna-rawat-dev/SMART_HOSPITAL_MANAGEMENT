#ifndef APPOINTMENTREQUESTS_H
#define APPOINTMENTREQUESTS_H

#include <iostream>
#include <string>
#include "appointment.h"
using namespace std;

struct AppointmentRequest
{
    string requestID, patientID, doctorID, department;
    string requestedDate, requestedTime, reason, status;
    string doctorRemarks, receptionistRemarks;
    AppointmentRequest* next;
    AppointmentRequest();
};

class AppointmentRequestManager
{
private:
    AppointmentRequest* head;
    AppointmentRequest* tail;
    int requestCount;
    int nextRequestNumber;
    void append(AppointmentRequest* request);
    void clear();
    string generateRequestID();
    bool load();
    bool save() const;
public:
    AppointmentRequestManager();
    ~AppointmentRequestManager();
    AppointmentRequestManager(const AppointmentRequestManager&) = delete;
    AppointmentRequestManager& operator=(const AppointmentRequestManager&) = delete;
    bool createRequest(const string& patientID, const string& doctorID,
                       const string& department, const string& date,
                       const string& time, const string& reason);
    void displayPatientRequests(const string& patientID) const;
    void displayPendingForDoctor(const string& doctorID) const;
    void displayPendingForReceptionist() const;
    bool doctorDecision(const string& requestID, const string& doctorID,
                        bool approve, const string& remarks);
    bool receptionistDecision(const string& requestID, bool approve,
                              const string& remarks, AppointmentManager& appointments);
};
#endif
