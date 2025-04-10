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

    return rescheduledPatient;
}
