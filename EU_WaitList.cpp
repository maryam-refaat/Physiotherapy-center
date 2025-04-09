#include "EU_WaitList.h"

// Inserts patient in priority order while maintaining FIFO for same priorities
void EU_WaitList::insertSorted(Patient* patient) 
{
    if (isEmpty()) 
    {
        enqueue(patient);
        return;
    }

    LinkedQueue<Patient*> tempQueue;
    bool inserted = false;
    int newPriority = patient->getEffectivePriority();

    Patient* currentPatient;
    while (dequeue(currentPatient)) 
    {
        if (!inserted && newPriority < currentPatient->getEffectivePriority()) 
        {
            tempQueue.enqueue(patient);
            inserted = true;
        }
        tempQueue.enqueue(currentPatient);
    }

    if (!inserted) 
    {
        tempQueue.enqueue(patient);
    }

    while (tempQueue.dequeue(currentPatient)) 
    {
        enqueue(currentPatient);
    }
}

int EU_WaitList::calcTreatmentLatency() const 
{
    int totalLatency = 0;
    //copy of the queue to not effect our queue
    LinkedQueue<Patient*> tempQueue(*this);

    Patient* currentPatient;
    while (tempQueue.dequeue(currentPatient)) 
    {
        totalLatency += currentPatient->getCurrentTreatmentDuration();
    }

    return totalLatency;
}