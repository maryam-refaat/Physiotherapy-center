#pragma once

#include "PriQueue.h"
#include "Patient.h"

class EarlyPList : public PriQueue<Patient*> 
{
public:
    Patient* reschedule();
};

