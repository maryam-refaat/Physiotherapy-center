#include "Resource.h"

Resource::Resource(TreatmentType t,int c) :  type(t), id(next_id++), capacity(c)
{
}
int Resource::get_id(){
    return id;
}
int Resource::getCapacity() const { return capacity; }