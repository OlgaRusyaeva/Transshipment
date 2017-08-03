//
//  ADPForwardMultipleTransshipments_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 04/03/15.
//
//

#ifndef TransshipmentProject_ADPForwardMultipleTransshipments_Solver_h
#define TransshipmentProject_ADPForwardMultipleTransshipments_Solver_h

#include "ADPForward_Solver.h"

class State;
class Cost;
class Demand;
class iDistribution;


class ADPForwardMultipleTransshipments_Solver:public ADPForward_Solver
{
    
private:
    
    float getCostForActions(State& state,const int& currentPeriod,const Cost& cost, const int& time, const arrays::Array2D<float>& possibleDemands)const;
    
    
    
};


#endif
