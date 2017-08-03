//
//  iSolver.h
//  


#ifndef project2_iSolver_h
#define project2_iSolver_h

#include "iProblem.h"
#include "Simulator.h"

class iSolver 
{
public:
    virtual ~iSolver(){}
    
    virtual void solve(iProblem& problem, Simulator& policy)=0;
    
};

#endif
