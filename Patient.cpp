#include "Patient.h"
#include "Treatment.h" 

#include <climits>

Patient::Patient(int id, PatientType patientType, int appointmentTime, int arrivalTime,LinkedQueue<Treatment*>& treatments)
   : PID(id), type(patientType), PT(appointmentTime), VT(arrivalTime),
     status(PatientStatus::IDLE), latePenalty(0), currentTreatmentDuration(0),
     treatmentStartTime(-1) 
{

    Treatment* treatment;
    while (treatments.dequeue(treatment)) 
    {
        requiredTreatments.enqueue(treatment);
    }

    if (type == PatientType::RECOVERING) 
    {
        optimizeTreatmentOrder();
    }
    updateStatus(0);
}

void Patient::setTreatmentStartTime(int time) {
    treatmentStartTime = time;
}



void Patient::optimizeTreatmentOrder() 
{
    if (requiredTreatments.getCount() <= 1) return;

    // Find treatment with minimum duration (for recovering patients)
    LinkedQueue<Treatment*> tempQueue;
    Treatment* minTreatment = nullptr;
    int minDuration = INT_MAX;
    Treatment* t;

    while (requiredTreatments.dequeue(t)) 
    {
        if (t->getDuration() < minDuration) 
        {
            minDuration = t->getDuration();
            if (minTreatment) tempQueue.enqueue(minTreatment);
            minTreatment = t;
        }
        else 
        {
            tempQueue.enqueue(t);
        }
    }

    if (minTreatment) 
    {
        requiredTreatments.enqueue(minTreatment);
        while (tempQueue.dequeue(t)) 
        {
            requiredTreatments.enqueue(t);
        }
    }
}

Treatment* Patient::getCurrentTreatment() const 
{
    Treatment* t;
    if (requiredTreatments.peek(t)) 
    {
        return t;
    }
    return nullptr;
}

TreatmentType Patient::getCurrentTreatmentType() const 
{
    Treatment* t = getCurrentTreatment();
    if (t) return t->getTreatmentType();
    return TreatmentType::EXERCISE; // Default
}

bool Patient::hasMoreTreatments() const 
{
    return !requiredTreatments.isEmpty();
}

bool Patient::isLastTreatmentXTherapy() const 
{
    if (requiredTreatments.getCount() != 1) return false;
    Treatment* t;
    requiredTreatments.peek(t);
    return t->getTreatmentType() == TreatmentType::EXERCISE;
}

void Patient::startTreatment(int currentTime) 
{
    treatmentStartTime = currentTime;
    Treatment* t = getCurrentTreatment();
    if (t) currentTreatmentDuration = t->getDuration();
    status = PatientStatus::SERV;
}

bool Patient::completeTreatment() 
{
    Treatment* t;
    if (requiredTreatments.dequeue(t)) 
    {
        delete t;
        currentTreatmentDuration = 0;
        treatmentStartTime = -1;
        return true;
    }
    return false;
}

void Patient::moveToNextTreatment() 
{
    status = PatientStatus::WAIT;
}

void Patient::updateStatus(int currentTime) 
{
    if (status == PatientStatus::FNSH) return;

    if (VT > currentTime) 
    {
        status = PatientStatus::IDLE;
    }
    else if (VT < PT) 
    {
        status = PatientStatus::ERLV;
    }
    else if (VT > PT) 
    {
        status = PatientStatus::LATE;
        calculateLatePenalty();
    }
    else 
    {
        status = PatientStatus::WAIT;
    }
}

void Patient::calculateLatePenalty() 
{
    if (VT > PT) 
    {
        latePenalty = (VT - PT) / 2;
    }
}
