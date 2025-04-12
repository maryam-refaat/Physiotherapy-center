#include "Scheduler.h"
#include <fstream>
#include <sstream>
#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand()
#include <cctype>
#include"iostream"
using namespace std;

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
    file >> Pcancel >> Presc;

    int Patients;
    file >> Patients;
    
    for (int i = 0; i < Patients; ++i) {
        char typeChar;
        int PT, VT, NT;
        file >> typeChar >> PT >> VT >> NT;

        // Validate number of treatments
        if (NT > 3) 
        {
            //cout << "Patient " << (i + 1) << ": Too many treatments (" << NT << "). Max is 3. Skipping." << endl;
            // Skip this patient's treatment data
            for (int j = 0; j < NT; ++j) 
            {
                char dummyType;
                int dummyDur;
                file >> dummyType >> dummyDur;
            }
            continue;
        }

        PatientType type = (typeChar == 'N') ? PatientType::NORMAL : PatientType::RECOVERING;

        LinkedQueue<Treatment*> treatments;

        bool hasElectro = false, hasUltrasound = false, hasExercise = false;
        bool valid = true;

        for (int j = 0; j < NT; ++j) 
        {
            char treatTypeChar;
            int duration;
            file >> treatTypeChar >> duration;
            treatTypeChar=toupper(treatTypeChar);

            TreatmentType tType = (treatTypeChar == 'E') ? TreatmentType::ELECTRO :
                                  (treatTypeChar == 'U') ? TreatmentType::ULTRASOUND : TreatmentType::EXERCISE;
            
            // Check for duplicate treatment types
            if (treatTypeChar == 'E' && hasElectro) {
            //cout << "Patient " << (i + 1) << ": Duplicate Electro therapy. Skipping." << endl;
            valid = false;
            break;
            }
           if (treatTypeChar == 'U' && hasUltrasound) {
           //cout << "Patient " << (i + 1) << ": Duplicate Ultrasound therapy. Skipping." << endl;
           valid = false;
           break;
           }
          if (treatTypeChar == 'X' && hasExercise) {
           //cout << "Patient " << (i + 1) << ": Duplicate Exercise therapy. Skipping." << endl;
           valid = false;
          break;
           }

            // Create appropriate treatment
            Treatment* newTreatment = nullptr;
            switch (treatTypeChar) 
            {
            case 'E':
                newTreatment = new ElectroTreatment(duration);
                hasElectro = true;
                break;
            case 'U':
                newTreatment = new UltrasoundTreatment(duration);
                hasUltrasound = true;
                break;
            case 'X':
                newTreatment = new ExerciseTreatment(duration);
                hasExercise = true;
                break;
            default:
                //cout << "Patient " << (i + 1) << ": Invalid treatment type "<< treatTypeChar << ". Skipping." << endl;
                valid = false;
            }

            if (!valid) break;
            treatments.enqueue(newTreatment);
        }

        if (valid) 
        {
            Patient* newPatient = new Patient(i + 1, type, PT, VT, treatments);
            ALLPatients.enqueue(newPatient);
        }
        else 
        {
            // Clean up any treatments we created before the error
            Treatment* t;
            while (treatments.dequeue(t)) 
            {
                delete t;
            }
            // Skip remaining treatments if we aborted early
            for (int j = treatments.getCount(); j < NT; ++j) 
            {
                char dummyType;
                int dummyDur;
                file >> dummyType >> dummyDur;
            }
        }
    }
    //delete[] gymCapacities;
            
            /*if(treatTypeChar == 'E')
                treatments.enqueue(new ElectroTreatment(duration));
            else if(treatTypeChar == 'U')
                treatments.enqueue(new UltrasoundTreatment(duration));
            else
                treatments.enqueue(new ExerciseTreatment(duration));*/


        //}


       // Patient* newPatient = new Patient(i + 1, type, PT, VT, treatments);
       //ALLPatients.enqueue(newPatient);
  //  }
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
             TempPatient->optimizeTreatmentOrder(*this);
        
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

int Scheduler:: getLatency(Treatment* t) const
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
        return X_Waiting.calcTreatmentLatency();
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
bool Scheduler::hasAvailableUltrasoundDevice()
{
    return !U_Devices.isEmpty();
}

void Scheduler::addToUWait(Patient* patient)
{
    patient->setState(PatientStatus::WAIT);
    U_Waiting.insertSorted(patient);
}

bool Scheduler::hasAvailableExerciseRoom()
{
    return !X_Rooms.isEmpty();
}
void Scheduler::addToXWait(Patient* patient)
{
    patient->setState(PatientStatus::WAIT);
    X_Waiting.insertSorted(patient);
}

bool Scheduler::hasAvailableElectroDevice()
{
    return !E_Waiting.isEmpty();
}

void Scheduler::addToEWait(Patient* patient)
{
    patient->setState(PatientStatus::WAIT);
    E_Waiting.insertSorted(patient);
}
