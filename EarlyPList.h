#pragma once

#include "PriQueue.h"
#include "Patient.h"

class EarlyPList : public PriQueue<Patient*> 
{
    int Presc; // Rescheduling probability (0-100)
    int EarlyNumber;
    int ResceduleNumber;
public:
    EarlyPList(int rescheduleProb) : Presc(rescheduleProb),EarlyNumber(0),ResceduleNumber(0) {}

    Patient* reschedule();

    void enqueue(Patient* &data, int priority);
    
    float getReschedulePercent(int &LateNumber);
    float getEarlyPercent(int &LateNumber);

    int GetEarlyNumber();
};



