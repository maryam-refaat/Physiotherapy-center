#include "Scheduler.h"
#include <fstream>
#include <sstream>
#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand()
#include <cctype>
#include"iostream"
#include "ElectroTreatment.h"
#include "Treatment.h"
#include "UltrasoundTreatment.h"
#include "ExerciseTreatment.h"

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

void Scheduler::ProcessTimeStep(int currentTime) {
    HandleArrivals(currentTime);
    AssignTreatments(currentTime);
    UpdateInTreatment(currentTime);

    // ----- NEW PART START -----
    int X = rand() % 101;

    if (X < 10) {
        MoveFromEarlyToRandomWaiting();
    } else if (X < 20) {
        MoveFromLateToRandomWaiting();
    } else if (X < 40) {
        MoveFromRandomWaitingToInTreatment(2);
    } else if (X < 50) {
        MoveFromInTreatmentToRandomWaiting();
    } else if (X < 60) {
        MoveFromInTreatmentToFinish();
    } else if (X < 70) {
        CancelRandomFromWaiting();
    } else if (X < 80) {
        RescheduleFromEarly();
    }
    // ----- NEW PART END -----
}

void Scheduler::MoveFromEarlyToRandomWaiting() {
    if (!EarlyPatients.isEmpty()) {
        Patient* p;
        int pri;
        EarlyPatients.dequeue(p, pri);
        RandomWaiting.enqueue(p);
    }
}

void Scheduler::MoveFromLateToRandomWaiting() {
    if (!LatePatients.isEmpty()) {
        Patient* p;
        int pri;
        LatePatients.dequeue(p, pri);
        RandomWaiting.enqueue(p);
    }
}

void Scheduler::MoveFromRandomWaitingToInTreatment(int count, int currentTime) {
    for (int i = 0; i < count && !RandomWaiting.isEmpty(); ++i) {
        Patient* p;
        RandomWaiting.dequeue(p);
        Treatment* t = p->getCurrentTreatment();
        if (t->canAssign(*this)) {
            Resource* r;
            if (dynamic_cast<UltrasoundTreatment*>(t)) {
                U_Devices.dequeue(r);
            } else if (dynamic_cast<ElectroTreatment*>(t)) {
                E_Devices.dequeue(r);
            } else {
                X_Rooms.dequeue(r);
            }
            t->assign(r, t->getDuration());
            p->setTreatmentStartTime(currentTime); // ← set start time
            InTreatment.enqueue(p, -currentTime);   // Priority by time started
        } else {
            t->moveToWait(*this, p); // Send to appropriate waiting queue
        }
    }
}

void Scheduler::MoveFromInTreatmentToRandomWaiting() {
    if (!InTreatment.isEmpty()) {
        Patient* p;
        int pri;
        InTreatment.dequeue(p, pri);
        RandomWaiting.enqueue(p);
    }
}

void Scheduler::MoveFromInTreatmentToFinish() {
    if (!InTreatment.isEmpty()) {
        Patient* p;
        int pri;
        InTreatment.dequeue(p, pri);
        MoveToFinished(p);
    }
}

void Scheduler::CancelRandomFromWaiting() {
    int listType = rand() % 3;  // 0 = E, 1 = U, 2 = X

    // Select the queue
    LinkedQueue<Patient*>* queue = nullptr;
    if (listType == 0) queue = &E_Waiting;
    else if (listType == 1) queue = &U_Waiting;
    else queue = &X_Waiting;

    if (queue && queue->getCount() > 0) {
        int index = rand() % queue->getCount();

        LinkedQueue<Patient*> temp;
        Patient* removedPatient = nullptr;
        Patient* p;

        // Rebuild queue, skipping the one to cancel
        for (int i = 0; i < queue->getCount(); ++i) {
            queue->dequeue(p);
            if (i == index) {
                removedPatient = p;
            } else {
                temp.enqueue(p);
            }
        }

        // Move remaining back to original queue
        while (!temp.isEmpty()) {
            temp.dequeue(p);
            queue->enqueue(p);
        }

        // Cancel selected patient
        if (removedPatient) {
            MoveToFinished(removedPatient);  // Treat as canceled
        }
    }
}


// void Scheduler::RescheduleFromEarly() {
//     Patient* rescheduled = EarlyPatients.reschedule();
//     if (rescheduled) {
//         // Assume the patient is now ready to be handled like a newly arrived one
//         if (rescheduled->getVT() == rescheduled->getPT()) {
//             rescheduled->updateStatus(currentTime);  // update status to WAIT
//             switch (rescheduled->getCurrentTreatmentType()) {
//                 case TreatmentType::ELECTRO: E_Waiting.insertSorted(rescheduled); break;
//                 case TreatmentType::ULTRASOUND: U_Waiting.insertSorted(rescheduled); break;
//                 case TreatmentType::EXERCISE: X_Waiting.insertSorted(rescheduled); break;
//             }
//         }
//     }
// }
void Scheduler::RescheduleFromEarly() {
    Patient* rescheduled = EarlyPatients.reschedule();
    if (!rescheduled) return;

    // Check if the patient is now exactly on time
    if (rescheduled->getVT() == rescheduled->getPT()) {
        // Set the status directly instead of using currentTime
        rescheduled->setState(PatientStatus::WAIT);  // Manually mark as waiting

        // Insert into the appropriate waiting queue based on treatment type
        switch (rescheduled->getCurrentTreatmentType()) {
            case TreatmentType::ELECTRO:
                E_Waiting.insertSorted(rescheduled);
                break;
            case TreatmentType::ULTRASOUND:
                U_Waiting.insertSorted(rescheduled);
                break;
            case TreatmentType::EXERCISE:
                X_Waiting.insertSorted(rescheduled);
                break;
        }
    } else {
        // They are still early or late, so you might keep them in early list or handle differently
        EarlyPatients.enqueue(rescheduled,-rescheduled->getPT());  // Optional: keep in early queue
    }
}


