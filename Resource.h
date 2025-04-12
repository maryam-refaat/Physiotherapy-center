#pragma once

enum class TreatmentType { ELECTRO, ULTRASOUND, EXERCISE };

class Resource {
protected:
    static int next_id;
    int id;
    TreatmentType type;
public:
    Resource(TreatmentType t,int c=1);
};
