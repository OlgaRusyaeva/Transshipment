//
//  iProblem.h
//

#ifndef project2_iProblem_h
#define project2_iProblem_h

#include <fstream>
#include "types.h"

class State;
class Demand;
class Cost;
class LeadTimes;
//class Actions;

class iProblem 
{
public:
    virtual ~iProblem(){}
    
    //virtual void initialize()=0;
    
    virtual const State& getState() const=0;
    
    virtual const Demand& getDemand() const=0;
    
    virtual const int getStateDimension() const=0; 

    virtual const int getTimeHorizon() const=0;
    
    virtual const int getLifeTime() const=0;
    
    virtual const LeadTimes& getLeadTimes() const=0;
    
    virtual const matrixInt_t getIndenticalLocations() const=0;
    
    //virtual const Actions& getDistances() const=0;
    
    virtual const Cost& getCost() const=0; 
    
    virtual void outputInput(const std::string& fileName) const=0;
    
    
};



#endif
