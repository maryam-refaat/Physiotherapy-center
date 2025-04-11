#include "Resource.h"

Resource::Resource(TreatmentType t,int c=1) :  type(t), id(next_id++), capacity(c)
{
}
int Resource::get_id(){
    return id;
}