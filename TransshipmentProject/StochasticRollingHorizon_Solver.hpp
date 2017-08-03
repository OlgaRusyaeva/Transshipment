//
//  StochasticRollingHorizon_Solver.hpp
//  TransshipmentProject
//
//  Created by Olga on 16/12/15.
//
//

#ifndef StochasticRollingHorizon_Solver_hpp
#define StochasticRollingHorizon_Solver_hpp

#include "gurobi_c++.h"
#include <boost/shared_ptr.hpp>

class iProblem;
class State;


typedef boost::shared_ptr<State> order_ptr;

class StochasticRollingHorizon_Solver {
    
public:
    StochasticRollingHorizon_Solver(){}
    ~StochasticRollingHorizon_Solver(){}
    
    void solve(iProblem& problem,  int horizonLength, const int& sampleSize);
    
    order_ptr getInitialState() const;
    
private:
    int seed;
    order_ptr order;
    
};

#endif /* StochasticRollingHorizon_Solver_hpp */
