#include "Patient.h"

int Patient::getEffectivePriority() const 
{
    return PT + penalty;
}

int Patient::getCurrentTreatmentDuration() const 
{
    return treatmentDuration;
}