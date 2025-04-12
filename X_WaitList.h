#pragma once
#include "EU_WaitList.h"

class X_WaitList : public EU_WaitList 
{
    int CancelNumber;
    int Pcancel; // Cancellation probability (0-100)
public:
    X_WaitList(int cancelProb) : Pcancel(cancelProb),CancelNumber(0) {}

    //Patient* cancel();

      bool cancel(Patient* &pcancelled);


      float getCancellationPercent(int& EarlyNumber, int& LateNumber);
};

