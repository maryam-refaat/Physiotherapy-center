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
    Patient* p;
    int pri;

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

void EarlyPList:: enqueue(Patient* &data, int priority)
{
        EarlyNumber++;
        PriQueue::enqueue(data,priority);
}

float EarlyPList:: getReschedulePercent(int &LateNumber)
{
    return ((float)ResceduleNumber*100)/(EarlyNumber + LateNumber);
}

float EarlyPList::getEarlyPercent(int &LateNumber)
{
    return ((float)EarlyNumber*100)/(EarlyNumber + LateNumber);
}

int EarlyPList::GetEarlyNumber()
{
    return EarlyNumber;
}

