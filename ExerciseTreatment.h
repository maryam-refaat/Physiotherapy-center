#ifndef EXERCISE_TREATMENT_H
#define EXERCISE_TREATMENT_H

#include "Treatment.h"

class ExerciseTreatment : public Treatment {
public:
    ExerciseTreatment(/*TreatmentType t,*/ int d) : Treatment(d) {}

    bool canAssign(Scheduler& scheduler) const override;
    void moveToWait(Scheduler& scheduler, Patient* patient) override;
};

#endif // EXERCISE_TREATMENT_H
