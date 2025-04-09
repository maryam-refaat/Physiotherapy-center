#pragma once
#include"LinkedQueue.h"
#include "Patient.h"

class EU_WaitList : public LinkedQueue<Patient*>
{
public:
    void insertSorted(Patient* patient);

    int calcTreatmentLatency() const;
};

