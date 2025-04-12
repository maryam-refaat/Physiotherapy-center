#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include "LinkedQueue.h"
#include "PriQueue.h"
#include "ArrayStack.h"
#include "EarlyPList.h"
#include "EU_WaitList.h"
#include "Patient.h"
using namespace std;


class UI {
public:
    // Function to get input filename from user
    static string getInputFilename() 
    {
        cout << "Enter input file name: ";
        string filename;
        cin >> filename;
        return filename;
    }

    // Function to get program mode (interactive/silent)
    /*static bool getProgramMode() {
        std::cout << "Select program mode:\n";
        std::cout << "1. Interactive mode\n";
        std::cout << "2. Silent mode\n";
        std::cout << "Enter choice (1 or 2): ";

        int choice;
        std::cin >> choice;
        return (choice == 1);
    }*/

    static void printConsole(int timestep,const LinkedQueue<Patient*>& allPatients,const EarlyPList& earlyList,const PriQueue<Patient*>& lateList,
        const EU_WaitList& eWait,const EU_WaitList& uWait,const LinkedQueue<Patient*>& xWait,const PriQueue<Patient*>& inTreatment,const ArrayStack<Patient*>& finished,
        int availableEDevices,int availableUDevices,int availableXRooms) 
    {

        system("cls"); // Clear screen

        cout << "Current Timestep: " << timestep << "\n\n";

        printAllPatientsList(allPatients);

        printWaitingLists(eWait, uWait, xWait);

        printEarlyLateLists(earlyList, lateList);

        // Print Available Resources (simplified for Phase 1.2)
        printAvailableResources(availableEDevices, availableUDevices, availableXRooms);

        printInTreatmentList(inTreatment);

        printFinishedPatients(finished);

        cout << "\nPress Enter to continue to next timestep...";
        cin.ignore();
        cin.get();
    }

private:
    // Helper printing functions
    static void printAllPatientsList(const LinkedQueue<Patient*>& list) 
    {
        cout << "ALL List\n";
        cout << list.getCount() << " patients remaining: ";
        // Create a copy to avoid modifying original queue
        LinkedQueue<Patient*> temp(list);
        Patient* p;
        int count = 0;

        while (temp.dequeue(p) && count < 10 && count< list.getCount())
        {
            cout << p;
            count++;
        }

        if (list.getCount() > 10) 
            cout << "...";
        cout << "\n\n";
    }

    static void printWaitingLists(const EU_WaitList& eWait,const EU_WaitList& uWait,const LinkedQueue<Patient*>& xWait) 
    {
        cout << "===================== Waiting Lists ======================\n";
        printSingleWaitList("E-therapy", eWait);
        printSingleWaitList("U-therapy", uWait);
        printSingleWaitList("X-therapy", xWait);
        cout << "\n";
    }

    static void printSingleWaitList(const string& name, const EU_WaitList& list) 
    {
        cout << list.getCount() << " " << name << " patients: ";
        list.print_list();
        /*EU_WaitList temp(list);
        Patient* p;
        int count = 0;

        while (temp.dequeue(p) && count < 10 && count< list.getCount())
        {
            cout << p->getPID() << " ";
            count++;
        }*/
        cout << "\n";
    }

    static void printSingleWaitList(const string& name, const LinkedQueue<Patient*>& list) 
    {
        cout << list.getCount() << " " << name << " patients: ";

        list.print_list();

        /*LinkedQueue<Patient*> temp(list);
        Patient* p;
        int count = 0;

        while (temp.dequeue(p) && count< list.getCount())
        {
            cout << p->getPID() << " ";
            count++;
        }*/
        cout << "\n";
    }

    static void printEarlyLateLists(const EarlyPList& early, const PriQueue<Patient*>& late) 
    {
        cout << "===================== Early List ======================\n";
        cout << early.getCount() << " patients: ";
        early.print_list();

        cout << "===================== Late List ======================\n";
        cout << late.getCount() << " patients: ";
        late.print_list();
        cout << "\n";
    }

    static void printAvailableResources(int eDevices, int uDevices, int xRooms) {
        std::cout << "===================== Avail E-devices ======================\n";
        std::cout << eDevices << " Electro devices: ";
        for (int i = 1; i <= eDevices && i <= 10; i++) {
            std::cout << i << " ";
        }
        std::cout << "\n";

        std::cout << "===================== Avail U-devices ======================\n";
        std::cout << uDevices << " Ultra devices: ";
        for (int i = 1; i <= uDevices && i <= 10; i++) {
            std::cout << i << " ";
        }
        std::cout << "\n";

        std::cout << "===================== Avail X-rooms ======================\n";
        std::cout << xRooms << " rooms: ";
        for (int i = 1; i <= xRooms && i <= 10; i++) {
            std::cout << "R" << i << "[0," << (i == 1 ? 10 : 5) << "] "; // Simplified for Phase 1.2
        }
        std::cout << "\n\n";
    }

    static void printInTreatmentList(const PriQueue<Patient*>& list) {
        cout << "===================== In-treatment List ======================\n";
        cout << list.getCount() << " patients: ";
        list.print_list();
        cout << "\n";
    }

    static void printFinishedPatients(const ArrayStack<Patient*>& stack) {
        std::cout << stack.getCount() << " finished patients: ";
        stack.print_stack();

        cout << "(Recently finished are printed first)\n\n";
    }

};
