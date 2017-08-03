//
//  Lp_solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 10/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ActionsGenerator.h"
//TODO change the type of solution, u, v, k, if you need

LP_solution optimizer(const State& currentState, const Actions& distance, const row_t& transshipmentCost, const vector_t& k, const matrix_t& u, const matrix_t& v)
{
    const int L=currentState.getLength();
    //Actions solution
    LP_solution solution(L);
    
    GRBEnv* env = 0;
    GRBVar* vars1 =0;
    GRBVar* vars2 =0;
    GRBVar* vars3 =0;
    GRBLinExpr* lhsObj1 = new GRBLinExpr[L];
    GRBLinExpr* lhsObj2 = new GRBLinExpr[L];
    GRBConstr* constr1=0;
    GRBConstr* constr2=0;
    
    try {
        env = new GRBEnv();
        
        GRBModel model = GRBModel(*env);
        
        //disable console output
        model.getEnv().set(GRB_IntParam_OutputFlag, 0);
        
        vars1 = model.addVars(0, NULL, NULL, NULL, NULL, L*L);
    
        int sum=0;
        for (int i=0; i<L; i++) {
            sum+=(k[i]-1);
        }
        vars2 = model.addVars(0, NULL, NULL, NULL, NULL, sum);

        vars3 = model.addVars(0, NULL, NULL, NULL, NULL, L);
        
        model.update();
    
        /* Add variables to the model */
        for (int i=0; i<L; i++) {
            for (int j=0; j<L; j++) {
                //cout<<-transshipmentCost[i]*distance.getElement(i,j)<<'\n';
            vars1[i*L+j].set(GRB_DoubleAttr_Obj,-transshipmentCost[i]*distance.getElement(i,j));
            }
        }
    
        sum=0;
        for (int i=0; i<L; i++) {
            if (i>0) sum+=(k[i-1]-1);
            for (int l=0; l<(k[i]-1); l++) {
                //cout<<u[i][l+1]-u[i][l]<<'\n';
                vars2[sum+l].set(GRB_DoubleAttr_UB,u[i][l+1]-u[i][l]);
            }
            
        }
        model.update();
    
        /* Add constraints */
    
        for (int i=0; i<L; i++) {
            GRBLinExpr lns=0;
            for (int j=0; j<L; j++) {
                lns+=vars1[j+i*L];
            }
            lhsObj1[i]=lns;
            //model.addConstr(lns,GRB_EQUAL, currentState.getElement(i));
        }
        
        constr1=model.addConstrs(lhsObj1, NULL, NULL, NULL, L);
        
        model.update();
        
        for (int i=0; i<L; i++) {
            constr1[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
            constr1[i].set(GRB_DoubleAttr_RHS,currentState.getElement(i));
        }
    
        sum=0;
        for (int i=0; i<L; i++) {
            //GRBLinExpr lns1=0;
            GRBLinExpr lns2=0;
            GRBLinExpr lns3=0;
            for (int j=0; j<L; j++) {
                //lns1+=vars1[j+i*L];
                lns2+=vars1[i+j*L];
            }
            if (i>0) sum+=(k[i-1]-1);
        
            for (int l=0; l<(k[i]-1); l++) {
                lns3+=vars2[l+sum];
            }
            lns3+=vars3[i];
            
            lhsObj2[i]=lns3-lns2;
            //EQUAL
            //model.addConstr(lns3-lns2,GRB_EQUAL,0);
        }
        
        constr2=model.addConstrs(lhsObj2, NULL, NULL, NULL, L);
        
        model.update();
        
        for (int i=0; i<L; i++) {
            constr2[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
            constr2[i].set(GRB_DoubleAttr_RHS,0);
        }
    
        /* Add objective*/
        GRBLinExpr obj=0;
    
        sum=0;
        for (int i=0; i<L; i++) {
            for (int j=0; j<L; j++) {
                obj-=transshipmentCost[i]*distance.getElement(i,j)*vars1[i*L+j];
            }
            if (i>0) sum+=(k[i-1]-1);
            for (int l=0; l<(k[i]-1); l++) {
                obj+=v[i][l]*vars2[sum+l];
            
            }
            obj+=v[i][k[i]-1]*vars3[i];
        
        }
    
    
        model.setObjective(obj,GRB_MAXIMIZE);
    
        model.update();
    
        model.optimize();

        //model.write("/Users/orusyaeva/Documents/ADP_Project/ADP/trial.lp");
        //model.write("/Users/orusyaeva/Documents/ADP_Project/ADP/solution.sol");
        
        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            //double objVal = model.get(GRB_DoubleAttr_ObjVal);
            for (int i = 0; i < L; i++)
            {
                for (int j=0; j<L; j++) {
                    if (i!=j) {
                        solution.setElement(i,j,vars1[i*L+j].get(GRB_DoubleAttr_X));
                    }
                }
            }
            
            //getTrueShadowPrices(solution);
            
            for (int i=0; i<L; i++) {
                solution.setShadowPricePlus(constr1[i].get(GRB_DoubleAttr_Pi));
                solution.setShadowPriceMinus(constr1[i].get(GRB_DoubleAttr_Pi));
            }
            //cout<<"Obj value: "<<model.get(GRB_DoubleAttr_ObjVal)<<'\n';
        }
        
    } catch(GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch(...) {
        cout << "Exception during optimization" << endl;
    }
    
    delete[] vars1;
    delete[] vars2;
    delete[] vars3;
    delete[] lhsObj1;
    delete[] lhsObj2;
    delete[] constr1;
    delete[] constr2;
    delete env;
    
    return solution;
}



