//
//  Solver.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/7/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Solver_h
#define TransshipmentProject_Solver_h

#include "iProblem.h"
#include "Policy.h"

class Solver
{
public:
    Solver(){}
    virtual ~Solver(){}
    
    virtual void solve(iProblem& problem, Policy& policy)=0;
        
};


#endif
