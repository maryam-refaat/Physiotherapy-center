#pragma once

#include "PriQueue.h"
#include "Patient.h"

class EarlyPList : public PriQueue<Patient*> 
{
    int Presc; // Rescheduling probability (0-100)
public:
    EarlyPList() : Presc(0) {}
    EarlyPList(int rescheduleProb) : Presc(rescheduleProb) {}

    Patient* reschedule();
};



