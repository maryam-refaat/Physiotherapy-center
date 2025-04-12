#pragma once

#include "Resource.h"
#include "Patient.h"
#include"Scheduler.h"

class Treatment {
protected:
    int duration;
    int assignmentTime;
    Resource* assignedResource;
public:
    Treatment(int d);

    virtual ~Treatment();

    virtual bool canAssign(Scheduler& scheduler) const = 0;
    virtual void moveToWait(Scheduler& scheduler, Patient* patient) = 0;

    int getDuration() const;
    int getAssignmentTime() const;
    Resource* getAssignedResource() const;
    void assign(Resource* resource, int time);
    void unassign();
};

