//
//  DeterministicRollingHorizon_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/12/15.
//
//

#include "DeterministicRollingHorizon_Simulator.hpp"
#include "State.h"
#include "Actions.h"
#include "iDistribution.h"
#include "iProblem.h"
#include "ProblemMethods.h"
#include "Cost.h"
#include "LeadTimes.h"
#include "Demand.h"

float DeterministicRollingHorizon_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration,  Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    LeadTimes leadTimes=problem.getLeadTimes();
    Demand demand=problem.getDemand();
    
    const int maxLead=leadTimes.getMaxLeadTimePlusOne();
    const int time= problem.getTimeHorizon();
    int horizon=horizonLength;
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,0);
    
    GRBEnv* env = 0;
    GRBModel* model=0;
    GRBVar* varsT=0;
    GRBVar* varsS=0;
    GRBLinExpr* lhsObj=0;
    GRBConstr* constr=0;
    
    if (time-currentPeriod<horizonLength) {
        horizon=time-currentPeriod;
    }
    
    const int size=location*location*horizon;
    
    try {
            
        //initialize
        env = new GRBEnv();
        model = new GRBModel(*env);
        varsT = model->addVars(0, NULL, NULL, NULL, NULL, size);
        varsS = model->addVars(0, NULL, NULL, NULL, NULL, size);
        lhsObj = new GRBLinExpr[location*(2*horizon)];
            
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
            
        model->update();
            
        //add constraints
            
        //std::cout<<"----------"<<'\n';
        for (int i=0; i<location; i++) {
            GRBLinExpr lns=0;
            for (int j=0; j<location; j++) {
                if ((leadTimes.getLeadTimeBetweenLocations(i,j)<=horizon)) {
                    lns+=varsS[i*location+j]+varsT[i*location+j];
                }
            }
            lhsObj[i]=lns;
        }
        
            
        for (int t=1; t<horizon+1; t++) {
            for (int j=0; j<location; j++) {
                GRBLinExpr lns=0;
                for (int tau=0; tau<maxLead; tau++) {
                    for (int i=0; i<location; i++) {
                        if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau-1>=0)) {
                            lns+=varsT[(t-tau-1)*location*location+i*location+j];
                        }
                    }
                }
                
                lhsObj[(t)*location+j]=lns;
            }
        }
            
            
        //std::cout<<"----------"<<'\n';
            
        for (int t=1; t<horizon; t++) {
            for (int j=0; j<location; j++) {
                GRBLinExpr lns1=0;
                for (int tau=0; tau<maxLead; tau++) {
                    for (int i=0; i<location; i++) {
                        if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau-1>=0)) {
                            lns1+=varsS[(t-tau-1)*location*location+i*location+j];
                        }
                            
                    }
                }
                    
                GRBLinExpr lns2=0;
                for (int k=0; k<location; k++) {
                    //if ((leadTimes.getLeadTimeBetweenLocations(j,k)<=horizon-t)) {
                        lns2+=varsT[(t)*location*location+j*location+k]+varsS[(t)*location*location+j*location+k];
                    //}
                }
                lhsObj[(horizon)*location+t*location+j]=lns1-lns2;
            }
        }
            
            
            
        constr=model->addConstrs(lhsObj, NULL, NULL, NULL, location*(2*horizon));
            
        model->update();
            
        for (int i=0; i<location; i++) {
            constr[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
            constr[i].set(GRB_DoubleAttr_RHS,currentState.getElement(i,0));
        }
            
        for (int t=1; t<horizon; t++) {
            for (int j=0; j<location; j++) {
                constr[(t)*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[(t)*location+j].set(GRB_DoubleAttr_RHS,(int)distribution[j]->mean());
                constr[(horizon)*location+t*location+j].set(GRB_CharAttr_Sense,GRB_EQUAL);
                constr[(horizon)*location+t*location+j].set(GRB_DoubleAttr_RHS,0);
            }
        }
            
        for (int i=0; i<location; i++) {
            if (horizon<time) {
                constr[(horizon)*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[(horizon)*location+i].set(GRB_DoubleAttr_RHS,(int)distribution[i]->mean());
            }
            else
            {
                constr[(horizon)*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[(horizon)*location+i].set(GRB_DoubleAttr_RHS,0);
            }

        }
            
        model->update();
            
        //add objective function
            
        GRBLinExpr obj=0;
            
        for (int t=0; t<horizon; t++) {
            for (int j=0; j<location; j++) {
                GRBLinExpr sumSold=0;
                GRBLinExpr sumStored=0;
                for (int i=0; i<location; i++) {
                    obj-=cost.getTransshipmentCost()[j]*cost.getDistance(j,i)*(varsT[t*location*location+j*location+i]+varsS[t*location*location+j*location+i]);
                }
                for (int tau=0; tau<maxLead; tau++) {
                    for (int i=0; i<location; i++) {
                        if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau-1>=0)) {
                            sumSold+=varsT[(t-tau-1)*location*location+i*location+j];
                            sumStored+=varsS[(t-tau-1)*location*location+i*location+j];
                        }

                            
                    }
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
        
        //output just the current action
        if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            for (int i=0; i<location; i++) {
                for (int j=0; j<location; j++) {
                    maxTransshipmentMatrix.setElement(i,j,varsT[i*location+j].get(GRB_DoubleAttr_X)+varsS[i*location+j].get(GRB_DoubleAttr_X));
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
    
    problemMethods::getNewState(currentState);
    
    problemMethods::getNewState(currentState, maxTransshipmentMatrix,leadTimes);
    
    float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
    
    delete[] constr;
    delete[] lhsObj;
    delete[] varsT;
    delete[] varsS;
    delete model;
    delete env;
    
    return transshipmentCost;
}

State DeterministicRollingHorizon_Simulator::getInitialState(iProblem& problem,const int& iteration) 
{
     return *IL;    
}

void DeterministicRollingHorizon_Simulator::initializeInitialState(const boost::shared_ptr<State>& state)
{
    IL=state;
}