#pragma once

enum class TreatmentType { ELECTRO, ULTRASOUND, EXERCISE };

class Resource {
protected:
    int id;
    TreatmentType type;
public:
    Resource(int i, TreatmentType t);
};

