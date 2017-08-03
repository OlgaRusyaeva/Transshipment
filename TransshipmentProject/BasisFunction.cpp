//
//  BasisFunction.cpp
//  TransshipmentProject
//
//  Created by Olga on 13/03/15.
//
//

#include "BasisFunction.h"
#include "State.h"
#include <math.h>

BasisFunction::BasisFunction(const State& state)
{
    int size=state.getLength();
    int sumIL=0;
    
    //BF1 - inventory level of each location
    for (int i=0; i<size;i++)
    {
        BF.push_back(state.getElement(i,0));
        sumIL+=state.getElement(i,0);
    }
    
    //BF2 - log(inventory level of each location)
    for (int i=0; i<size;i++)
    {
        BF.push_back(log10(state.getElement(i,0)+1));
    }
    
    //BF3 - log(sum of inventory levels)
    BF.push_back(log10(sumIL+1));
}

row_t BasisFunction::get()const
{
    return BF;
}

void BasisFunction::set(const int& index, const int& value)
{
     BF[index]=value;
}

int BasisFunction::getSize()const
{
    return (int)BF.size();
}

const BasisFunction& BasisFunction::operator=(const BasisFunction& newBasicFunction)
{
    for (int i=0; i<(int)this->getSize(); i++) {
        this->set(i,newBasicFunction.get()[i]);
    }
    return *this;
}