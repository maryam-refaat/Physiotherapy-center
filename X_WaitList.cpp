#include "X_WaitList.h"
#include <cstdlib> // For rand()
#include <ctime>   // For time()


#include "X_WaitList.h"

Patient* X_WaitList::cancel() 
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

    LinkedQueue<Patient*> tempQueue;
    int count = 0;
    Patient* currentPatient;

    while (dequeue(currentPatient)) 
    {
        tempQueue.enqueue(currentPatient);
        count++;
    }

    while (tempQueue.dequeue(currentPatient)) 
    {
        enqueue(currentPatient);
    }

    int targetIndex = rand() % count;
    Patient* cancelledPatient = nullptr;
    int currentIndex = 0;

    // Remove the selected patient
    while (dequeue(currentPatient)) 
    {
        if (currentIndex == targetIndex) 
        {
            cancelledPatient = currentPatient;
        }
        else 
        {
            tempQueue.enqueue(currentPatient);
        }
        currentIndex++;
    }

    while (tempQueue.dequeue(currentPatient)) 
    {
        enqueue(currentPatient);
    }

    return cancelledPatient;
}
