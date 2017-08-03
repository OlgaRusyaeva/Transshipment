//
//  BFValueFunction.cpp
//  TransshipmentProject
//
//  Created by Olga on 13/03/15.
//
//

#include "BFValueFunction.h"

void BFValueFunction::set(const int& timeIndex, const row_t& coefficients)
{
    TC[timeIndex]=coefficients;
}

matrix_t BFValueFunction::get()const
{
    return TC;
}

void BFValueFunction::initialize(const int& time)
{
    TC=matrix_t(time);
}


const BFValueFunction& BFValueFunction::operator=(const BFValueFunction& newValueFunction)
{
    if (this->get().empty()) {
        this->initialize(newValueFunction.get().size());
    }

    for (int i=0; i<this->get().size(); i++) {
        this->set(i,newValueFunction.get()[i]);
    }
    
    return *this;
}

