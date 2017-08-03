//
//  Deterministic_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 7/31/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "Deterministic_Solver.h"
#include "iDistribution.h"
#include "State.h"
#include "Actions.h"
#include "Demand.h"
#include "Cost.h"
#include "ProblemMethods.h"
#include "iProblem.h"


void Deterministic_Solver::solve(iProblem& problem,const int& numberIterations)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    
    initializeActionsMap(L,T,numberIterations);
    
    State initialState = problem.getState();
    Cost costInState = problem.getCost();
    Demand demand = problem.getDemand();

    
    //check if I need the number of simulations
    getDeterministicSolution(L,T,initialState, costInState, demand,numberIterations);
}

std::vector<actionMap_ptr>& Deterministic_Solver::getActionsMap() 
{
    return actionsMap;
}

void Deterministic_Solver::initializeActionsMap(const int& location, const int& time,const int& numberIterations)
{
    for (int n=0; n<numberIterations; n++) {
        actionMap_ptr ptr(new ActionsMap(location,time));
        actionsMap.push_back(ptr);
    }
}

void Deterministic_Solver::getDeterministicSolution(const int& location, const int& time, State& initialState, const  Cost& cost, Demand& demand,const int& numberIterations)
{
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    const int size=location*location*time;
    
    GRBEnv* env = 0;
    GRBModel* model=0;
    GRBVar* varsT=0;
    GRBVar* varsS=0;
    GRBLinExpr* lhsObj=0;
    GRBConstr* constr=0;
    
    for (int n=0; n<numberIterations; n++) {
        
    try {
        
        //initialize
        env = new GRBEnv();
        model = new GRBModel(*env);
        varsT = model->addVars(0, NULL, NULL, NULL, NULL, size);
        varsS = model->addVars(0, NULL, NULL, NULL, NULL, size);
        lhsObj = new GRBLinExpr[2*location*time];
        
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
        
        model->update();
        
        //add constraints
        
        for (int t=0; t<time; t++) {
            for (int j=0; j<location; j++) {
                GRBLinExpr lns=0;
                for (int i=0; i<location; i++) {
                        lns+=varsT[t*location*location+i*location+j];
                    }
                lhsObj[t*location+j]=lns;
            }
        }
        //std::cout<<"----------"<<'\n';
        for (int i=0; i<location; i++) {
            GRBLinExpr lns=0;
            for (int j=0; j<location; j++) {
                //std::cout<<i*location+j<<'\n';
                lns+=varsS[i*location+j]+varsT[i*location+j];
            }
            lhsObj[time*location+i]=lns;
        }
         
        //std::cout<<"----------"<<'\n';
        
        for (int t=1; t<time; t++) {
			for (int j=0; j<location; j++) {
                GRBLinExpr lns1=0;
                for (int i=0; i<location; i++) {
                    	lns1+=varsS[(t-1)*location*location+i*location+j];
                }
                GRBLinExpr lns2=0;
                for (int k=0; k<location; k++) {
                    lns2+=varsT[(t)*location*location+j*location+k]+varsS[(t)*location*location+j*location+k];
                }
                lhsObj[time*location+(t)*location+j]=lns1-lns2;
            }
        }
        
        constr=model->addConstrs(lhsObj, NULL, NULL, NULL, 2*location*time);
        
        model->update();
        
        for (int t=0; t<time; t++) {
            for (int j=0; j<location; j++) {
                constr[t*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[t*location+j].set(GRB_DoubleAttr_RHS,(int)distribution[j]->sample());
            }
        }
        
        for (int i=0; i<location; i++) {
            constr[time*location+i].set(GRB_CharAttr_Sense,GRB_EQUAL);
            constr[time*location+i].set(GRB_DoubleAttr_RHS,initialState.getElement(i,0));
        }

        
        for (int t=1; t<time; t++) {
            for (int i=0; i<location; i++) {
                constr[time*location+(t)*location+i].set(GRB_CharAttr_Sense,GRB_EQUAL);
                constr[time*location+(t)*location+i].set(GRB_DoubleAttr_RHS,0);
            }
        }
        
        model->update();
        
        //add objective function
        
        GRBLinExpr obj=0;
        
        for (int t=0; t<time; t++) {
            for (int j=0; j<location; j++) {
                GRBLinExpr sumSold=0;
                GRBLinExpr sumStored=0;
                for (int i=0; i<location; i++) {
                    //std::cout<<t*location*location+j*location+i<<'\n';
                    sumSold+=varsT[t*location*location+i*location+j];
                    sumStored+=varsS[t*location*location+i*location+j];
                    obj-=cost.getTransshipmentCost()[j]*cost.getDistance(j,i)*(varsT[t*location*location+j*location+i]+varsS[t*location*location+j*location+i]);
                }
                obj+=cost.getSalesCost()[j]*sumSold;
                obj-=cost.getHoldingCost()[j]*sumStored;

            }
        }
        
        model->setObjective(obj,GRB_MAXIMIZE);
        
        model->update();
        
        model->optimize();
        
        //model->write("/Users/orusyaeva/Desktop/trial.lp");
    	//model->write("/Users/orusyaeva/Desktop/solution.sol");
        
        if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            for (int t = 0; t < time; t++)
            {
                for (int i=0; i<location; i++) {
                    for (int j=0; j<location; j++) {
                        actionsMap[n]->setActionPerPeriodBetweenLocations(t,i,j,varsT[t*location*location+i*location+j].get(GRB_DoubleAttr_X)+varsS[t*location*location+i*location+j].get(GRB_DoubleAttr_X));
                    }
                }
            }
        }
        	
    } catch(GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    } catch(...) {
        std::cout << "Exception during optimization" << std::endl;
    }
        env->resetParams();
    }
    
    delete[] constr;
    delete[] lhsObj;
    delete[] varsT;
    delete[] varsS;
    delete model;
    delete env;

}


