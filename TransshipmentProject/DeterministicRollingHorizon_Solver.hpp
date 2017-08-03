//
//  DeterministicRollingHorizon_Solver.hpp
//  TransshipmentProject
//
//  Created by Olga on 16/12/15.
//
//

#ifndef DeterministicRollingHorizon_Solver_hpp
#define DeterministicRollingHorizon_Solver_hpp

#include "gurobi_c++.h"
#include <boost/shared_ptr.hpp>

class iProblem;
class State;


typedef boost::shared_ptr<State> order_ptr;

class DeterministicRollingHorizon_Solver {
    
public:
    DeterministicRollingHorizon_Solver(){}
    ~DeterministicRollingHorizon_Solver(){}
    
    void solve(iProblem& problem,  int horizonLenght);
    
    order_ptr getInitialState() const;
    
private:
    int seed;
    order_ptr order;
    
};

#endif /* DeterministicRollingHorizon_Solver_hpp */
