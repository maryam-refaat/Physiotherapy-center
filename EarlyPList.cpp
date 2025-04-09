#include "EarlyPList.h"
#include <cstdlib>
#include <ctime>

Patient* EarlyPList::reschedule() 
{
    static bool seeded = false;
    if (!seeded) 
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    if (isEmpty()) 
    {
        return nullptr;
    }

    int count = 0;
    PriQueue<Patient*> tempQueue;
    Patient* currentPatient;
    int currentPri;

    while (dequeue(currentPatient, currentPri)) 
    {
        tempQueue.enqueue(currentPatient, currentPri);
        count++;
    }

    while (tempQueue.dequeue(currentPatient, currentPri)) 
    {
        enqueue(currentPatient, currentPri);
    }

    if (count == 0) 
    {
        return nullptr;
    }

    int targetIndex = rand() % count;
    Patient* rescheduledPatient = nullptr;
    int currentIndex = 0;

    while (dequeue(currentPatient, currentPri)) 
    {
        if (currentIndex == targetIndex) 
        {
            rescheduledPatient = currentPatient;
        }
        else 
        {
            tempQueue.enqueue(currentPatient, currentPri);
        }
        currentIndex++;
    }

    while (tempQueue.dequeue(currentPatient, currentPri)) 
    {
        enqueue(currentPatient, currentPri);
    }

    return rescheduledPatient;
}