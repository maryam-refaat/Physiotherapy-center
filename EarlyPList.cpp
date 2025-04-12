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

    
    // PriQueue<Patient*> tempQueue;
    // Patient* p;
    // int pri;

    // while (dequeue(p, pri)) 
    // {
    //     tempQueue.enqueue(p, pri);
    //     count++;
    // }
    int count = getCount();
    int targetIndex = std::rand() % count;
    int currentIndex = 0;
    Patient* rescheduledPatient = nullptr;

    while (dequeue(p, pri) && count--) 
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

float EarlyPList:: getReschedulePercent(int &LateNumber)
{
    return ((flaot)ResceduleNumber*100)/(EarlyNumber + LateNumber);
}

float EarlyPList::getEarlyPercent(int &LateNumber)
{
    return ((flaot)EarlyNumber*100)/(EarlyNumber + LateNumber);
}

int EarlyPList::GetEarlyNumber()
{
    return EarlyNumber;
}

