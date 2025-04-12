#include "X_WaitList.h"
#include <cstdlib> // For rand()
#include <ctime>   // For time()


// #include "X_WaitList.h"

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

    
    // LinkedQueue<Patient*> eligiblePatients;
    // LinkedQueue<Patient*> tempQueue;
    Patient* current;

    int count=getCount();
    int EligiblePatients_Count=0;
    while (dequeue(current) && count-- ) 
    {
        if (current->isLastTreatmentXTherapy()) 
        {
            // eligiblePatients.enqueue(current);
            EligiblePatients_Count++;
        }
        enqueue(current);
    }

    // while (tempQueue.dequeue(current)) 
    // {
    //     enqueue(current);
    // }

    if (!EligiblePatients_Count) 
    {
        return nullptr;
    }

    CancelNumber++;

    int targetIndex = std::rand() % EligiblePatients_Count;
    Patient* cancelledPatient = nullptr;
    int currentIndex = 0;
    count=getCount();

    while (dequeue(current) && count--) 
    {
        if (current->isLastTreatmentXTherapy() && currentIndex++ == targetIndex) 
        {
            cancelledPatient = current;
        }
        else 
        {
            enqueue(current);
        }
    }

    // while (tempQueue.dequeue(current)) 
    // {
    //     enqueue(current);
    // }

    return cancelledPatient;
}

float X_WaitList::getCancellationPercent(int &EarlyNumber,int &LateNumber)
{
    return ((float)CancelNumber*100)/(EarlyNumber + LateNumber);
}
