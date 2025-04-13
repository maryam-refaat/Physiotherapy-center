#pragma once

enum class TreatmentType { ELECTRO, ULTRASOUND, EXERCISE };

class Resource {
protected:
static int next_id;
    int id;
    TreatmentType type;
    int capacity;
public:
    Resource(TreatmentType t,int c=1);
   int  get_id();
   int getCapacity() const;
};
int Resource::next_id = 0;