#pragma once
#include "LinkedQueue.h"
#include "PriQueue.h"
#include "ArrayStack.h"
#include "EarlyPList.h"
#include "EU_WaitList.h"
#include "X_WaitList.h"
#include "patient.h"
#include "Resource.h"

using namespace std;

class Scheduler {
private:
    // Core Patient Lists
    LinkedQueue<Patient*> ALLPatients;
    EarlyPList EarlyPatients;
    PriQueue<Patient*> LatePatients;

    // Waiting Lists for Treatments
    EU_WaitList U_Waiting;
    EU_WaitList E_Waiting;
    X_WaitList X_Waiting;

    // Treatment Resources
    LinkedQueue<Resource*> E_Devices;
    LinkedQueue<Resource*> U_Devices;
    LinkedQueue<Resource*> X_Rooms;

    // Currently in treatment
    PriQueue<Patient*> InTreatment;

    // Finished
    ArrayStack<Patient*> FinishedPatients;

public:
    // Constructor
    Scheduler();

    // Input/Initialization
    void LoadPatientsFromFile(string fileName);
    void InitializeResources(int NE, int NU, int NG, const int * gymCapacities);

    // Simulation core
    void RunSimulation();
    void ProcessTimeStep(int currentTime);
    void HandleArrivals(int currentTime);
    void AssignTreatments(int currentTime);
    void UpdateInTreatment(int currentTime);
    void MoveToFinished(Patient* p);

    int getLatency(Treatment* t)const ;
    // Utilities
    void PrintStatistics();
};
