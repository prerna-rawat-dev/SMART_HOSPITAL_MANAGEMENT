#include "../include/appointmentrequests.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

AppointmentRequest::AppointmentRequest() : next(NULL) {}

AppointmentRequestManager::AppointmentRequestManager()
    : head(NULL), tail(NULL), requestCount(0), nextRequestNumber(1) { load(); }
AppointmentRequestManager::~AppointmentRequestManager() { save(); clear(); }
void AppointmentRequestManager::append(AppointmentRequest* r)
{
    r->next = NULL;
    if (!head) head = tail = r; else { tail->next = r; tail = r; }
    ++requestCount;
}
void AppointmentRequestManager::clear()
{
    while (head) { AppointmentRequest* t=head; head=head->next; delete t; }
    tail=NULL; requestCount=0;
}
string AppointmentRequestManager::generateRequestID()
{
    ostringstream out; out << "REQ";
    out.width(5); out.fill('0'); out << nextRequestNumber++;
    return out.str();
}
bool AppointmentRequestManager::load()
{
    ifstream f("DATA/appointment_requests.txt"); if (!f) return true;
    string line;
    while (getline(f,line)) {
        if (line.empty()) continue;
        string a[10]; stringstream ss(line); bool ok=true;
        for(int i=0;i<9;i++) if(!getline(ss,a[i],'|')) {ok=false;break;}
        if(!ok || !getline(ss,a[9])) continue;
        AppointmentRequest* r=new AppointmentRequest;
        r->requestID=a[0]; r->patientID=a[1]; r->doctorID=a[2]; r->department=a[3];
        r->requestedDate=a[4]; r->requestedTime=a[5]; r->reason=a[6]; r->status=a[7];
        r->doctorRemarks=a[8]; r->receptionistRemarks=a[9];
        append(r);
        if(r->requestID.size()>3 && r->requestID.substr(0,3)=="REQ") {
            int n=atoi(r->requestID.substr(3).c_str()); if(n>=nextRequestNumber) nextRequestNumber=n+1;
        }
    }
    return true;
}
bool AppointmentRequestManager::save() const
{
    ofstream f("DATA/appointment_requests.txt"); if(!f) return false;
    for(AppointmentRequest* r=head;r;r=r->next)
        f<<r->requestID<<'|'<<r->patientID<<'|'<<r->doctorID<<'|'<<r->department<<'|'
         <<r->requestedDate<<'|'<<r->requestedTime<<'|'<<r->reason<<'|'<<r->status<<'|'
         <<r->doctorRemarks<<'|'<<r->receptionistRemarks<<'\n';
    return true;
}
bool AppointmentRequestManager::createRequest(const string& patientID,const string& doctorID,
 const string& department,const string& date,const string& time,const string& reason)
{
    if(patientID.empty()||doctorID.empty()||department.empty()||date.empty()||time.empty()||reason.empty()) return false;
    AppointmentRequest* r=new AppointmentRequest;
    r->requestID=generateRequestID(); r->patientID=patientID; r->doctorID=doctorID;
    r->department=department; r->requestedDate=date; r->requestedTime=time;
    r->reason=reason; r->status="Pending Doctor Approval";
    append(r);
    if(!save()) { // remove tail safely
        if(head==r) head=tail=NULL; else { AppointmentRequest* p=head; while(p&&p->next!=r)p=p->next; if(p){p->next=NULL;tail=p;} }
        --requestCount; delete r; return false;
    }
    cout<<"Appointment request submitted. Request ID: "<<r->requestID<<"\nStatus: "<<r->status<<endl;
    return true;
}
void AppointmentRequestManager::displayPatientRequests(const string& id) const
{
    bool found=false; cout<<"\n========== MY APPOINTMENT REQUESTS ==========\n";
    for(AppointmentRequest* r=head;r;r=r->next) if(r->patientID==id) {
        found=true; cout<<"Request ID: "<<r->requestID<<" | Doctor: "<<r->doctorID
        <<" | Department: "<<r->department<<" | Date: "<<r->requestedDate
        <<" | Time: "<<r->requestedTime<<" | Status: "<<r->status
        <<" | Doctor remarks: "<<r->doctorRemarks<<" | Receptionist remarks: "<<r->receptionistRemarks<<endl;
    }
    if(!found) cout<<"No appointment requests found.\n";
}
void AppointmentRequestManager::displayPendingForDoctor(const string& id) const
{
    bool found=false; cout<<"\n========== PENDING DOCTOR APPROVAL ==========\n";
    for(AppointmentRequest* r=head;r;r=r->next) if(r->doctorID==id&&r->status=="Pending Doctor Approval") {
        found=true; cout<<r->requestID<<" | Patient: "<<r->patientID<<" | Department: "<<r->department
        <<" | Date: "<<r->requestedDate<<" | Time: "<<r->requestedTime<<" | Reason: "<<r->reason<<endl;
    }
    if(!found) cout<<"No pending requests.\n";
}
void AppointmentRequestManager::displayPendingForReceptionist() const
{
    bool found=false; cout<<"\n====== PENDING RECEPTIONIST APPROVAL ======\n";
    for(AppointmentRequest* r=head;r;r=r->next) if(r->status=="Pending Receptionist Approval") {
        found=true; cout<<r->requestID<<" | Patient: "<<r->patientID<<" | Doctor: "<<r->doctorID
        <<" | Date: "<<r->requestedDate<<" | Time: "<<r->requestedTime<<" | Reason: "<<r->reason<<endl;
    }
    if(!found) cout<<"No doctor-approved requests waiting.\n";
}
bool AppointmentRequestManager::doctorDecision(const string& id,const string& doctorID,bool approve,const string& remarks)
{
    for(AppointmentRequest* r=head;r;r=r->next) if(r->requestID==id) {
        if(r->doctorID!=doctorID||r->status!="Pending Doctor Approval") return false;
        r->status=approve?"Pending Receptionist Approval":"Rejected by Doctor"; r->doctorRemarks=remarks;
        return save();
    }
    return false;
}
bool AppointmentRequestManager::receptionistDecision(const string& id,bool approve,const string& remarks,AppointmentManager& appointments)
{
    for(AppointmentRequest* r=head;r;r=r->next) if(r->requestID==id) {
        if(r->status!="Pending Receptionist Approval") return false;
        if(approve) {
            Appointment a; a.patientID=r->patientID; a.doctorID=r->doctorID; a.department=r->department;
            a.appointmentDate=r->requestedDate; a.appointmentTime=r->requestedTime; a.reason=r->reason; a.status="Confirmed";
            if(!appointments.createConfirmedAppointment(a)) return false;
            r->status="Confirmed";
        } else r->status="Rejected by Receptionist";
        r->receptionistRemarks=remarks;
        return save();
    }
    return false;
}
