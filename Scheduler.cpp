#include "Scheduler.h"
#include <fstream>
#include <sstream>
#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand()
#include <cctype>

Scheduler::Scheduler() {
    srand(time(nullptr)); // Seed for random events like cancel/reschedule
}

void Scheduler::LoadPatientsFromFile(string fileName) {
    ifstream file(fileName);
    if (!file.is_open()) return;

    int NE, NU, NG;
    file >> NE >> NU >> NG;

    int* gymCapacities = new int[NG];
    for (int i = 0; i < NG; ++i)
        file >> gymCapacities[i];

    InitializeResources(NE, NU, NG, gymCapacities);

    int Pcancel, Presc;
    file >> Pcancel >> Presc; // Store these in member variables if needed

    int Patients;
    file >> Patients;
    
    for (int i = 0; i < Patients; ++i) {
        char typeChar;
        int PT, VT, NT;
        file >> typeChar >> PT >> VT >> NT;

        PatientType type = (typeChar == 'N') ? PatientType::NORMAL : PatientType::RECOVERING;

        LinkedQueue<Treatment*> treatments;
        for (int j = 0; j < NT; ++j) {
            char treatTypeChar;
            int duration;
            file >> treatTypeChar >> duration;
            treatTypeChar=toupper(treatTypeChar);

            TreatmentType tType = (treatTypeChar == 'E') ? TreatmentType::E :
                                  (treatTypeChar == 'U') ? TreatmentType::U : TreatmentType::X;
            if(treatTypeChar == 'E')
                treatments.enqueue(new ElectroTreatment(duration));
            else if(treatTypeChar == 'U')
                treatments.enqueue(new UltrasoundTreatment(duration));
            else
                treatments.enqueue(new ExerciseTreatment(duration));
        }

        Patient* newPatient = new Patient(i + 1, type, PT, VT, treatments);
        ALLPatients.enqueue(newPatient);
    }
}

void Scheduler::InitializeResources(int NE, int NU, int NG, const int *gymCapacities ) {
    for (int i = 0; i < NE; ++i)
        E_Devices.enqueue(new Resource(TreatmentType::ELECTRO));

    for (int i = 0; i < NU; ++i)
        U_Devices.enqueue(new Resource(TreatmentType::ULTRASOUND));

    for (int i = 0; i < NG; ++i)
        X_Rooms.enqueue(new Resource(TreatmentType::EXERCISE, gymCapacities[i]));
}

void Scheduler::RunSimulation() {
    int currentTime = 1;
    while (!ALLPatients.isEmpty() || !InTreatment.isEmpty()) {
        ProcessTimeStep(currentTime);
        currentTime++;
    }
    PrintStatistics();
}

void Scheduler::ProcessTimeStep(int currentTime) {
    HandleArrivals(currentTime);
    AssignTreatments(currentTime);
    UpdateInTreatment(currentTime);
}

void Scheduler::HandleArrivals(int currentTime) {
    int size = ALLPatients.getCount();
    for (int i = 0; i < size; ++i) {
        Patient* p;
        ALLPatients.dequeue(p);
        if (p->getVT() > currentTime) {
            ALLPatients.enqueue(p);
        } else if (p->getVT() < p->getPT()) {
            EarlyPatients.enqueue(p,- (p->getPT()));
        } else if (p->getVT() > p->getPT()) {
            p->calculateLatePenalty();
            int prio= -(p->getLatePenalty()+p->getVT()); //get priority
            LatePatients.enqueue(p,prio);
        } else {
            p->updateStatus(currentTime); // Becomes WAIT
            switch (p->getCurrentTreatmentType()) {
                case TreatmentType::ELECTRO: E_Waiting.insertSorted(p); break;
                case TreatmentType::ULTRASOUND: U_Waiting.insertSorted(p); break;
                case TreatmentType::EXERCISE: X_Waiting.insertSorted(p); break;
            }
        }
    }
}

void Scheduler::AssignTreatments(int currentTime) {
    // For brevity, implement assignment to resources one by one
    // Example: Assign from E_Waiting to E_Devices if any device is free
    // Similar logic would be done for U and X
    int size = ALLPatients.getCount();
    
    for(int i=0; i<size; i++)
    {
        Patient* TempPatient;
        ALLPatients.dequeue(TempPatient);
        PatientType patientType=TempPatient->getType();
        if(patientType== PatientType::RECOVERING)
             TempPatient->optimizeTreatmentOrder();
        
        Treatment* t=TempPatient->getCurrentTreatment();
            if(t->canAssign(*this))
            {
                Resource* ToAssign;
                if(dynamic_cast<UltrasoundTreatment*>(t))
                {  
                    U_Devices.dequeue(ToAssign);
                    t->assign(ToAssign,t->getDuration());
                }
                else if(dynamic_cast<ElectroTreatment*>(t))
                {  
                    E_Devices.dequeue(ToAssign);
                    t->assign(ToAssign,t->getDuration());
                }
                else
                {  
                    X_Rooms.dequeue(ToAssign);
                    t->assign(ToAssign,t->getDuration());
                }
            }
            else
                t->moveToWait(*this,TempPatient);
    }
}

int Scheduler:: getLatency(Treatment* t)
{
    if(dynamic_cast<UltrasoundTreatment*>(t))
        {  
            return U_Waiting.calcTreatmentLatency();
        }
    else if(dynamic_cast<ElectroTreatment*>(t))
        {  
            return E_Waiting.calcTreatmentLatency();
        }
    else
        {  
            return X_Waiting.calcTreatmentLatency()
        }
}


void Scheduler::UpdateInTreatment(int currentTime) {
    int size = InTreatment.getCount();
    for (int i = 0; i < size; ++i) {
        Patient* p;
        int pri;
        InTreatment.dequeue(p,pri);
        if (currentTime - p->getTreatmentStartTime() >= p->getCurrentTreatmentDuration()) {
            if (p->completeTreatment()) 
                if(!p->hasMoreTreatments())
                    MoveToFinished(p);
             else {
                switch (p->getCurrentTreatmentType()) {
                    case TreatmentType::ELECTRO: E_Waiting.insertSorted(p); break;
                    case TreatmentType::ULTRASOUND: U_Waiting.insertSorted(p); break;
                    case TreatmentType::EXERCISE: X_Waiting.insertSorted(p); break;
                }
            }
        } else {
            InTreatment.enqueue(p,pri);
        }
    }
}

void Scheduler::MoveToFinished(Patient* p) {
    p->updateStatus(-1); // Set FNSH status internally
    FinishedPatients.push(p);
}

void Scheduler::PrintStatistics() {
    // Output relevant stats here: number of patients, avg wait/treatment times, etc.
}
