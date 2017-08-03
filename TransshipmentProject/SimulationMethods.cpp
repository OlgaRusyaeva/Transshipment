//
//  SimulationMethods.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "SimulationMethods.h"
#include "State.h"
#include "array.h"

using namespace arrays;

namespace simulationMethods {
    //generate all possible demands for the state
    void generatePossibleDemands(const State& state, Array2D<float>& S)
    {
        int size=S.sizeH();
        vector_t::const_iterator itBegin;
        
        for (int j=1; j<= size-1; j++) {
            int k=j;
            itBegin= state.getCurrentPeriodState().begin();
            int del=*itBegin+1;
            for (int t=0; k>0; t++) {
                int integ=(unsigned)k/del;
                int frac=k-integ*del;
                S(j,t)=frac;
                k=integ;
                //k=(k-frac)/del;
                itBegin++;
                del=1+*itBegin;
            }
        }
    }
    
State generateInventoryLevel(vector_t::iterator itBegin,int& inv,const int& location, const int& leadTime)
{
    State generatedState(location,leadTime);
    int k=inv;
    int del=*itBegin+1;
    for (int t=0; k>0; t++) {
        int integ=(unsigned)k/del;
        int frac=k-integ*del;
        int index=(unsigned)t/leadTime;
        int time=t-index*leadTime;
        generatedState.setElement(index,time,frac);
        k=integ;
        //k=(k-frac)/del;
        itBegin++;
        del=1+*itBegin;
    }
    inv++;
    
    return generatedState;
}
   
State generateInventoryLevel(const int& maxValue,int& inv,const int& location, const int& leadTime)
{
    State generatedState(location,leadTime);
    int k=inv;
    int del=maxValue+1;
    for (int t=0; k>0; t++) {
        int integ=(unsigned)k/del;
        int frac=k-integ*del;
        int index=(unsigned)t/leadTime;
        int time=t-index*leadTime;
        generatedState.setElement(index,time,frac);
        k=integ;

    }
    inv++;
        
    return generatedState;
}
    
    
    
    void getIndexVector(State& vectorOL, const vecMatrixInt& actionsForState,int& size)
    {
        int actions_size=0;
        const int L=vectorOL.getLength();
        for (int j=0; j<L; j++) {
            actions_size=(int)actionsForState[j].size();
            vectorOL.setElement(j,0, actions_size-1);
            size*=actions_size;
        }
        
    }
    
    int getRandomNumber(const int& min, const int& max)
    {
        return min + (rand() % (int)(max - min + 1));
    }

}

