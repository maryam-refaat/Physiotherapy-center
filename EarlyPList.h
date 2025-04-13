#pragma once

#include "PriQueue.h"
#include "Patient.h"

class EarlyPList : public PriQueue<Patient*> 
{
    int Presc; // Rescheduling probability (0-100)
public:
<<<<<<< HEAD
    EarlyPList() : Presc(0) {}
    EarlyPList(int rescheduleProb) : Presc(rescheduleProb) {}
=======
    EarlyPList(int rescheduleProb=0) : Presc(rescheduleProb),EarlyNumber(0),ResceduleNumber(0) {}
>>>>>>> 5c7257d3bd145d8168ced912365aad9ad01cd9c5

    Patient* reschedule();
};



