//
//  DeterministicRollingHorizon_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/12/15.
//
//

//get the initial replenishment

#include "DeterministicRollingHorizon_Solver.hpp"
#include "State.h"
#include "iProblem.h"
#include "Demand.h"
#include "LeadTimes.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ProblemMethods.h"

void DeterministicRollingHorizon_Solver::solve(iProblem& problem, int horizonLength)
{
    const int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    Demand demand = problem.getDemand();
    LeadTimes leadTimes = problem.getLeadTimes();
    const int time= problem.getTimeHorizon();
    
    order.reset(new State(location));
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,20);
    
    const int maxLead=leadTimes.getMaxLeadTimePlusOne();
    
    
    GRBEnv* env = 0;
    GRBModel* model=0;
    GRBVar* varsT=0;
    GRBVar* varsS=0;
    GRBVar* qT=0;
    GRBVar* qS=0;
    GRBLinExpr* lhsObj=0;
    GRBConstr* constr=0;
    
    if (time<horizonLength) {
        horizonLength=time;
    }
    
    const int size=location*location*horizonLength;
    
    try {
            
        //initialize
        env = new GRBEnv();
        model = new GRBModel(*env);
        varsT = model->addVars(0, NULL, NULL, NULL, NULL, size);
        varsS = model->addVars(0, NULL, NULL, NULL, NULL, size);
        qT=model->addVars(0, NULL, NULL, NULL, NULL, location);
        qS=model->addVars(0, NULL, NULL, NULL, NULL, location);
        lhsObj = new GRBLinExpr[location*(2*horizonLength+1)];
            
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
            
        model->update();
            
        //add constraints
            
        //std::cout<<"----------"<<'\n';
        for (int i=0; i<location; i++) {
            GRBLinExpr lns=0;
            for (int j=0; j<location; j++) {
                if ((leadTimes.getLeadTimeBetweenLocations(i,j)<=horizonLength)) {
                    lns+=varsS[i*location+j]+varsT[i*location+j];
                }
            }
            lhsObj[i]=lns-qS[i];
        }
        
        for (int i=0; i<location; i++) {
            lhsObj[location+i]=qT[i];
        }
        
        
        for (int t=1; t<horizonLength+1; t++) {
            for (int j=0; j<location; j++) {
                GRBLinExpr lns=0;
                for (int tau=0; tau<maxLead; tau++) {
                    for (int i=0; i<location; i++) {
                        if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau-1>=0)) {
                            lns+=varsT[(t-tau-1)*location*location+i*location+j];
                        }
                    }
                }
                
                lhsObj[(t+1)*location+j]=lns;
            }
        }
        
        
        //std::cout<<"----------"<<'\n';
        
        for (int t=1; t<horizonLength; t++) {
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
                    //if ((leadTimes.getLeadTimeBetweenLocations(j,k)<=horizonLength-t)) {
                        lns2+=varsT[(t)*location*location+j*location+k]+varsS[(t)*location*location+j*location+k];
                    //}
                }
                lhsObj[(horizonLength+1)*location+t*location+j]=lns1-lns2;
            }
        }
        
        
            
        constr=model->addConstrs(lhsObj, NULL, NULL, NULL, location*(2*horizonLength+1));
            
        model->update();
            
        for (int i=0; i<location; i++) {
            constr[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
            constr[i].set(GRB_DoubleAttr_RHS,0);
            constr[location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
            constr[location+i].set(GRB_DoubleAttr_RHS,(int)distribution[i]->mean());
        }
            
        for (int t=1; t<horizonLength; t++) {
            for (int j=0; j<location; j++) {
                constr[(t+1)*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[(t+1)*location+j].set(GRB_DoubleAttr_RHS,(int)distribution[j]->mean());
                constr[(horizonLength+1)*location+t*location+j].set(GRB_CharAttr_Sense,GRB_EQUAL);
                constr[(horizonLength+1)*location+t*location+j].set(GRB_DoubleAttr_RHS,0);
            }
        }
            
        for (int i=0; i<location; i++) {
            if (horizonLength<time) {
                constr[(horizonLength+1)*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[(horizonLength+1)*location+i].set(GRB_DoubleAttr_RHS,(int)distribution[i]->mean());
            }
            else
            {
                constr[(horizonLength+1)*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[(horizonLength+1)*location+i].set(GRB_DoubleAttr_RHS,0);
            }
        }
            
        model->update();
            
        //add objective function
            
        GRBLinExpr obj=0;
            
        for (int i=0; i<location; i++) {
            obj+=(cost.getSalesCost()[i]-cost.getOrderingCost()[i])*qT[i]+(-cost.getOrderingCost()[i]-cost.getHoldingCost()[i])*qS[i];
        }
            
        for (int t=0; t<horizonLength; t++) {
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
            
        if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            for (int i=0; i<location; i++) {
                order->setElement(i,0,qT[i].get(GRB_DoubleAttr_X)+qS[i].get(GRB_DoubleAttr_X));
            }
        }
            
    } catch(GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    } catch(...) {
        std::cout << "Exception during optimization" << std::endl;
    }
    env->resetParams();
    
    
    delete[] constr;
    delete[] lhsObj;
    delete[] qT;
    delete[] qS;
    delete[] varsT;
    delete[] varsS;
    delete model;
    delete env;
    
    
}

order_ptr DeterministicRollingHorizon_Solver::getInitialState() const
{
    return order;
}