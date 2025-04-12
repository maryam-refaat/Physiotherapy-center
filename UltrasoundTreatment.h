#ifndef ULTRASOUND_TREATMENT_H
#define ULTRASOUND_TREATMENT_H

#include "Treatment.h"

class UltrasoundTreatment : public Treatment {
public:
    UltrasoundTreatment(/*TreatmentType t,*/ int d) : Treatment(d) {}

    bool canAssign(Scheduler& scheduler) const;
    void moveToWait(Scheduler& scheduler, Patient* patient);
};

#endif // ULTRASOUND_TREATMENT_H
