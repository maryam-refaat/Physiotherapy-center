#include "X_WaitList.h"
#include <cstdlib> // For rand()
#include <ctime>   // For time()


#include "X_WaitList.h"

Patient* X_WaitList::cancel() 
{
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }

    if (std::rand() % 100 >= Pcancel || isEmpty()) 
    {
        return nullptr;
    }

    LinkedQueue<Patient*> eligiblePatients;
    LinkedQueue<Patient*> tempQueue;
    Patient* current;

    while (dequeue(current)) 
    {
        if (current->isLastTreatmentXTherapy()) 
        {
            eligiblePatients.enqueue(current);
        }
        tempQueue.enqueue(current);
    }

    while (tempQueue.dequeue(current)) 
    {
        enqueue(current);
    }

    if (eligiblePatients.isEmpty()) 
    {
        return nullptr;
    }

   int targetIndex = std::rand() % eligiblePatients.getCount();
    Patient* cancelledPatient = nullptr;
    int currentIndex = 0;

    while (dequeue(current)) 
    {
        if (current->isLastTreatmentXTherapy() && currentIndex++ == targetIndex) 
        {
            cancelledPatient = current;
        }
        else 
        {
            tempQueue.enqueue(current);
        }
    }

    while (tempQueue.dequeue(current)) 
    {
        enqueue(current);
    }

    return cancelledPatient;
}
