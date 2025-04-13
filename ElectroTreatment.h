#pragma once

#include "Treatment.h"
#include"Schedule.h"
class Scheduler;

class ElectroTreatment : public Treatment {
public:
    ElectroTreatment( int d) : Treatment(d) {}

    bool canAssign(Scheduler& scheduler) const;
    void moveToWait(Scheduler& scheduler, Patient* patient);
};
