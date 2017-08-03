//
//  QP_optimizer.cpp
//  TransshipmentProject
//
//  Created by Olga on 15/03/15.
//
//

#include "QP_optimizer.h"
#include "BasisFunction.h"
#include <boost/shared_ptr.hpp>

row_t QP_optimizer::optimize(const std::vector<boost::shared_ptr<BasisFunction> >& bSet, const int& size, const row_t& y)
{
    NumY=(int)y.size();
    NumVars=size+1;
    
    row_t solution(NumVars);
    env = 0;
    vars =0;
    
    try {
        env = new GRBEnv();
        
        model = new GRBModel(*env);
        
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
        
        vars=model->addVars(NULL, NULL, NULL, NULL, NULL, NumVars);
        
        model->update();
        
        for (int i=0; i<NumVars; i++) {
            vars[i].set(GRB_DoubleAttr_LB,-GRB_INFINITY);
        }
        
        // Integrate new variables
        
        //model->update();
        
        // Set objective
        
        GRBQuadExpr obj=0;
        
        for (int i=0; i<NumY; i++) {
            GRBLinExpr linExpr=y[i]-vars[NumVars-1];
            for (int j=0; j<NumVars-1; j++) {
                linExpr-=vars[j]*bSet[i]->get()[j];
            }
            obj+=linExpr*linExpr;
        }
        
        
        model->setObjective(obj,GRB_MINIMIZE);
    
        
        model->optimize();

        if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
        {
            for (int i = 0; i < NumVars; i++)
            {
                solution[i]=vars[i].get(GRB_DoubleAttr_X);
            }
            
            std::cout << "Obj: " << model->get(GRB_DoubleAttr_ObjVal) << std::endl;
        }
        
    } catch(GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    } catch(...) {
        std::cout << "Exception during optimization" << std::endl;
    }
    
    return solution;
}
