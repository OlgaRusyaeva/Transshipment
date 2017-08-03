//
//  ADPMultipleTransshipments_Solver.hpp
//  TransshipmentProject
//
//  Created by Olga on 21/06/16.
//
//

#ifndef ADPMultipleTransshipments_Solver_hpp
#define ADPMultipleTransshipments_Solver_hpp

#include "ADP_Solver.h"

class ADPMultipleTransshipments_Solver: public ADP_Solver
{

private:
    
    
    void getApproximation(const int& location, const int& time, const int& lifeTime, State& initialState,const  Cost& cost,Demand& demand, const LeadTimes& leadTimes, const int& iterationNumber);
    
    
};

#endif /* ADPMultipleTransshipments_Solver_hpp */
