#ifndef PATIENT_H
#define PATIENT_H

#include "LinkedQueue.h"
#include "Treatment.h"

class Treatment;

enum class PatientType { NORMAL, RECOVERING };
enum class PatientStatus { IDLE, ERLV, LATE, WATI, SERV, FNSH };

class Patient {
protected:
    int PID;            // Patient ID
    PatientType type;
    int PT;             // Appointment Time
    int VT;             // Arrival Time
    PatientStatus status;
    LinkedQueue<Treatment*> requiredTreatments;
    int latePenalty;
    int currentTreatmentDuration;
    int treatmentStartTime;

    public:
        Patient(int id, PatientType t, int appointmentTime, int arrivalTime,
            LinkedQueue<Treatment*>& treatments);
    
        int getPID() const { return PID; }
        PatientType getType() const { return type; }
        int getPT() const { return PT; }
        int getVT() const { return VT; }
        PatientStatus getStatus() const { return status; }
        int getLatePenalty() const { return latePenalty; }
        int getCurrentTreatmentDuration() const { return currentTreatmentDuration; }
        int getTreatmentStartTime() const { return treatmentStartTime; }
        int getEffectivePriority() const { return PT + (isLate() ? latePenalty : 0); }
    
        bool isIdle() const { return status == PatientStatus::IDLE; }
        bool isEarly() const { return status == PatientStatus::ERLV; }
        bool isLate() const { return status == PatientStatus::LATE; }
        bool isWaiting() const { return status == PatientStatus::WATI; }
        bool inTreatment() const { return status == PatientStatus::SERV; }
        bool isFinished() const { return status == PatientStatus::FNSH; }
    
       Treatment* getCurrentTreatment() const;
        TreatmentType getCurrentTreatmentType() const;
        bool hasMoreTreatments() const;
        bool isLastTreatmentXTherapy() const;
        void startTreatment(int currentTime);
        bool completeTreatment();
        void moveToNextTreatment();
    
       // Special functions for recovering patients
        void optimizeTreatmentOrder();
    
        void updateStatus(int currentTime);
        void calculateLatePenalty();
};

#endif


//#pragma once
//class Patient {
//protected:
//    int PT;          // Appointment time
//    int penalty;     // Late penalty (0 if not late)
//    int treatmentDuration; // Current treatment duration
//
//public:
//    int getEffectivePriority() const;
//
//    int getCurrentTreatmentDuration() const;
//
//};