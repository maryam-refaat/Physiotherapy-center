#pragma once
#include "EU_WaitList.h"

class X_WaitList : public EU_WaitList 
{
    int Pcancel; // Cancellation probability (0-100)
public:
    X_WaitList():Pcancel(0) {};
    X_WaitList(int cancelProb) : Pcancel(cancelProb) {}
    Patient* cancel();
};

