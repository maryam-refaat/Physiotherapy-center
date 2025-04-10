#include "ElectroTreatment.h"
#include "Scheduler.h"

bool ElectroTreatment::canAssign(Scheduler& scheduler) const 
{
    return scheduler.hasAvailableElectroDevice();
}

void ElectroTreatment::moveToWait(Scheduler& scheduler, Patient* patient) 
{
    scheduler.addToEWait(patient);
}