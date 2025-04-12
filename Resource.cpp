#include "Resource.h"

int Resource::next_id = 1;

Resource::Resource(TreatmentType t,int c) :  type(t), id(next_id++), capacity(c)
{
}

int Resource::get_id()
{
    return id;
}
