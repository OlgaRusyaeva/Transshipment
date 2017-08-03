//
//  QP_optimizer.h
//  TransshipmentProject
//
//  Created by Olga on 15/03/15.
//
//

#ifndef __TransshipmentProject__QP_optimizer__
#define __TransshipmentProject__QP_optimizer__

#include "types.h"
#include "gurobi_c++.h"

class State;
class Cost;
class BasisFunction;

class QP_optimizer
{
public:
    QP_optimizer(){}
    ~QP_optimizer()
    {
        delete[] vars;
        delete model;
        delete env;
    }
    
    row_t optimize(const std::vector<boost::shared_ptr<BasisFunction> >& bSet, const int& size, const row_t& y);
    

private:
    
    int NumVars;
    
    int NumY;
    
    GRBEnv* env;
    GRBModel* model;
    GRBVar* vars;

    
};


#endif /* defined(__TransshipmentProject__QP_optimizer__) */
