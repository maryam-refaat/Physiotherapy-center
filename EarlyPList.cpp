#include "EarlyPList.h"
#include <cstdlib>
#include <ctime>

Patient* EarlyPList::reschedule() 
{
    static bool seeded = false;
    if (!seeded) 
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }

    if (std::rand() % 100 >= Presc || isEmpty()) 
    {
        return nullptr;
    }

    int count = 0;
    PriQueue<Patient*> tempQueue;
    Patient* p;
    int pri;

    while (dequeue(p, pri)) 
    {
        tempQueue.enqueue(p, pri);
        count++;
    }

    int targetIndex = std::rand() % count;
    int currentIndex = 0;
    Patient* rescheduledPatient = nullptr;

    while (tempQueue.dequeue(p, pri)) 
    {
        if (currentIndex == targetIndex) 
        {
            rescheduledPatient = p;
        }
        else 
        {
            enqueue(p, pri);
        }
        currentIndex++;
    }
    ResceduleNumber++;
    
    rescheduledPatient->setPT(rescheduledPatient->getPT() + rand() % rescheduledPatient->getPT());
    return rescheduledPatient;
}

void EarlyPList:: enqueue(const T& data, int priority)
{
        EarlyNumber++;
        PriQueue::enqueue(const T& data, int priority);
}

flaot EarlyPList:: getReschedulePercent(int &LateNumber)
{
    return ((flaot)ResceduleNumber*100)/(EarlyNumber + LateNumber);
}

flaot EarlyPList::getEarlyPercent(int &LateNumber)
{
    return ((flaot)EarlyNumber*100)/(EarlyNumber + LateNumber);
}

int EarlyPList::GetEarlyNumber()
{
    return EarlyNumber;
}

