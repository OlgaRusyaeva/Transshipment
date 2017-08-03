//
//  DPMultipleTransshipments_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 24/02/15.
//
//

#ifndef __TransshipmentProject__DPMultipleTransshipments_Solver__
#define __TransshipmentProject__DPMultipleTransshipments_Solver__

#include "types.h"
#include "DP_Solver.h"


class State;
class Cost;
class Demand;
class iDistribution;


class DPMultipleTransshipments_Solver:public DP_Solver
{
    
private:
    
    
    float getMaxProfit(const int& t, const int& time, const State& state, Demand& demand, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution) const;


    
};


#endif /* defined(__TransshipmentProject__DPMultipleTransshipments_Solver__) */
