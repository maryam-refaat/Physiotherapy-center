#pragma once
class Patient {
protected:
    int PT;          // Appointment time
    int penalty;     // Late penalty (0 if not late)
    int treatmentDuration; // Current treatment duration

public:
    int getEffectivePriority() const;

    int getCurrentTreatmentDuration() const;

};

