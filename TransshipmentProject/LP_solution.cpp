//
//  File.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 12/3/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "Actions.h"


//LP_solution::LP_solution(int size):vector_size(size), Actions(size){}

void LP_solution::setShadowPricePlus(const int& id, const float& value)
{
    shadowPricesPlus[id]=value;
}

void LP_solution::setShadowPriceMinus(const int& id, const float& value)
{
    shadowPricesMinus[id]=value;
}

float LP_solution::getShadowPricePlus(const int& index)const
{
    return shadowPricesPlus[index];
}

float LP_solution::getShadowPriceMinus(const int& index)const
{
    return shadowPricesMinus[index];
}

const LP_solution& LP_solution::operator=(const LP_solution& newSolution)
{
    for (int i=0; i<this->size; i++) {
        for (int j=0; j<this->size; j++) {
            this->setElement(i,j,newSolution.getElement(i,j));
        }
    }
    
    for (int i=0; i<this->sp_size;i++) {
        this->setShadowPricePlus(i,newSolution.getShadowPricePlus(i));
    }
    
    for (int i=0; i<this->sp_size;i++) {
        this->setShadowPriceMinus(i,newSolution.getShadowPriceMinus(i));
    }
    return *this;
}



void LP_solution::printShadowPricePlus() const
{
    for (int i=0; i<sp_size; i++) {
        std::cout<<getShadowPricePlus(i)<<'\n';
    }
}

void LP_solution::printShadowPriceMinus() const
{
    for (int i=0; i<sp_size; i++) {
        std::cout<<getShadowPriceMinus(i)<<'\n';
    }
}

int LP_solution::getShadowPriceNumber() const
{
    return sp_size;
}