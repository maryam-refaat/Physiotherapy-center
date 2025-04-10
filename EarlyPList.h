#pragma once

#include "PriQueue.h"
#include "Patient.h"

class EarlyPList : public PriQueue<Patient*> 
{
    int Presc; // Rescheduling probability (0-100)
public:
    EarlyPList(int rescheduleProb) : Presc(rescheduleProb) {}

    Patient* reschedule();
};



