//
//  Demand.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "Demand.h"

using namespace std;

Demand::Demand(int length):vectorLength(length)
{
    initialize();
}
    
vector_t Demand::getDemand() const 
{
    return demandVector;
}
    
int Demand::getElement(const int& index)const 
{
    return demandVector[index];
}
    
void Demand::setElement(const int& index,const int& newValue)
{
    demandVector[index]=newValue;
}
    
void Demand::setDemandParameters(const int& key_id, const float& mean, const float& sd)
{
    demandParameters.insert(pair<int, pair<float, float> >(key_id,make_pair(mean,sd)));
}

map<int, pair<float, float> > Demand::getDemandParameters()const
{
    return demandParameters;
}
    
void Demand::initialize()
{
    demandVector = vector_t(vectorLength);
    distributionType=vector_t(vectorLength);
}
    
pair<float, float> Demand::getDemandParametersForLocation(const int& index) const
{
    map<int, pair<float, float> >:: const_iterator mapIterator = getDemandParameters().find(index);
    
    if (mapIterator!=demandParameters.end()) {
        return mapIterator->second;
    }
    else
    {
        cout<<"ERROR"<<'\n';
        return make_pair(0,0);
    }
}

void Demand::setDistribution(const int& index, const int& value)
{
        distributionType[index]=value;
}

vector_t Demand::getDistribution()const
{
    return distributionType;
}

void Demand::print() const
{
    for (int i=0; i<vectorLength; i++) {
        std::cout<<getElement(i)<<'\n';
    }
}

