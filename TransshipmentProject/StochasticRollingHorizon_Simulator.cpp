//
//  StochasticRollingHorizon_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/12/15.
//
//

#include "StochasticRollingHorizon_Simulator.hpp"
#include "State.h"
#include "Cost.h"
#include "Actions.h"
#include "iDistribution.h"
#include "iProblem.h"
#include "LeadTimes.h"
#include "ProblemMethods.h"
#include "Demand.h"

float StochasticRollingHorizon_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration,  Actions& maxTransshipmentMatrix)
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
    
    const int size=location*location*horizon*sampleSize;
    
    try {
        
        //initialize
        env = new GRBEnv();
        model = new GRBModel(*env);
        varsT = model->addVars(0, NULL, NULL, NULL, NULL, size);
        varsS = model->addVars(0, NULL, NULL, NULL, NULL, size);
        lhsObj = new GRBLinExpr[sampleSize*location*(2*horizon)+(sampleSize-1)*location*location];
        
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
        
        model->update();
        
        //add constraints
        
        //std::cout<<"----------"<<'\n';
        for (int w=0; w<sampleSize; w++) {
            for (int i=0; i<location; i++) {
                GRBLinExpr lns=0;
                for (int j=0; j<location; j++) {
                    if ((leadTimes.getLeadTimeBetweenLocations(i,j)<=horizon)) {
                        lns+=varsS[w*location*location+i*location+j]+varsT[w*location*location+i*location+j];
                    }
                }
                lhsObj[w*location+i]=lns;
            }
            
        }

        
        for (int t=1; t<horizon+1; t++) {
            for (int w=0; w<sampleSize; w++) {
            	for (int j=0; j<location; j++) {
                	GRBLinExpr lns=0;
                	for (int tau=0; tau<maxLead; tau++) {
                    	for (int i=0; i<location; i++) {
                        	if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau-1>=0)) {
                            	lns+=varsT[(t-tau-1)*location*location*sampleSize+w*location*location+i*location+j];
                        	}
                    	}
                	}
                
                	lhsObj[(t)*location*sampleSize+w*location+j]=lns;
                }
            }
        }
        
        
        //std::cout<<"----------"<<'\n';
        
        for (int t=1; t<horizon; t++) {
            for (int w=0; w<sampleSize; w++) {
                for (int j=0; j<location; j++) {
                    GRBLinExpr lns1=0;
                    for (int tau=0; tau<maxLead; tau++) {
                        for (int i=0; i<location; i++) {
                            if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau-1>=0)) {
                                lns1+=varsS[(t-tau-1)*location*location*sampleSize+w*location*location+i*location+j];
                            }
                            
                        }
                    }
                    
                    GRBLinExpr lns2=0;
                    for (int k=0; k<location; k++) {
                        //if ((leadTimes.getLeadTimeBetweenLocations(j,k)<=horizon-t)) {
                        lns2+=varsT[(t)*location*location*sampleSize+w*location*location+j*location+k]+varsS[(t)*location*location*sampleSize+w*location*location+j*location+k];
                        //}
                    }
                    lhsObj[(horizon)*location*sampleSize+t*location*sampleSize+w*location+j]=lns1-lns2;
                }
            }

        }
        
 
        for (int w=0; w<sampleSize-1; w++) {
            for (int i=0; i<location; i++) {
                for (int j=0; j<location; j++) {
                    GRBLinExpr lns=0;
                    lns=varsS[w*location*location+i*location+j]+varsT[w*location*location+i*location+j]-varsS[(w+1)*location*location+i*location+j]-varsT[(w+1)*location*location+i*location+j];
                    lhsObj[sampleSize*location*(2*horizon)+w*location*location+i*location+j]=lns;
                }
            }
        }
        
        
        constr=model->addConstrs(lhsObj, NULL, NULL, NULL, sampleSize*location*(2*horizon)+(sampleSize-1)*location*location);
        
        model->update();
        
        for (int w=0; w<sampleSize; w++) {
            for (int i=0; i<location; i++) {
                constr[w*location+i].set(GRB_CharAttr_Sense,GRB_EQUAL);
                constr[w*location+i].set(GRB_DoubleAttr_RHS,currentState.getElement(i,0));
            }
        }

        
        for (int t=1; t<horizon; t++) {
        	for (int w=0; w<sampleSize; w++) {
            	for (int j=0; j<location; j++) {
                	constr[(t)*location*sampleSize+w*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                    constr[(t)*location*sampleSize+w*location+j].set(GRB_DoubleAttr_RHS,(int)distribution[j]->mean());
                	constr[(horizon)*location*sampleSize+t*location*sampleSize+w*location+j].set(GRB_CharAttr_Sense,GRB_EQUAL);
                    constr[(horizon)*location*sampleSize+t*location*sampleSize+w*location+j].set(GRB_DoubleAttr_RHS,0);
            	}
            }
        }
        for (int w=0; w<sampleSize; w++) {
        	for (int i=0; i<location; i++) {
                if (horizon<time) {
                    constr[(horizon)*location*sampleSize+w*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                    constr[(horizon)*location*sampleSize+w*location+i].set(GRB_DoubleAttr_RHS,(int)distribution[i]->sample());
                }
                else
                {
                    constr[(horizon)*location*sampleSize+w*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                    constr[(horizon)*location*sampleSize+w*location+i].set(GRB_DoubleAttr_RHS,0);
                }
        	}
        }
        
        for (int w=0; w<sampleSize-1; w++) {
            for (int i=0; i<location; i++) {
                for (int j=0; j<location; j++) {
                    constr[sampleSize*location*(2*horizon)+w*location*location+i*location+j].set(GRB_CharAttr_Sense,GRB_EQUAL);
                    constr[sampleSize*location*(2*horizon)+w*location*location+i*location+j].set(GRB_DoubleAttr_RHS,0);
                }
            }

        }
        
        model->update();
        
        //add objective function
        
        GRBLinExpr obj=0;
        float prob=(float)1/sampleSize;
        for (int t=0; t<horizon; t++) {
            for (int w=0; w<sampleSize; w++) {
            	for (int j=0; j<location; j++) {
                	GRBLinExpr sumSold=0;
                	GRBLinExpr sumStored=0;
                    for (int i=0; i<location; i++) {
                        obj-=prob*cost.getTransshipmentCost()[j]*cost.getDistance(j,i)*(varsT[t*location*location*sampleSize+w*location*location+j*location+i]+varsS[t*location*location*sampleSize+w*location*location+j*location+i]);
                    }
                	for (int tau=0; tau<maxLead; tau++) {
                    	for (int i=0; i<location; i++) {
                        	if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau-1>=0)) {
                           	 	sumSold+=varsT[(t-tau-1)*location*location*sampleSize+w*location*location+i*location+j];
                            	sumStored+=varsS[(t-tau-1)*location*location*sampleSize+w*location*location+i*location+j];
                        	}
                        
                    	}
                	}
                	obj+=prob*cost.getSalesCost()[j]*sumSold;
                	obj-=prob*cost.getHoldingCost()[j]*sumStored;
                
            	}
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

State StochasticRollingHorizon_Simulator::getInitialState(iProblem& problem,const int& iteration)
{
    return *IL;
}

void StochasticRollingHorizon_Simulator::initializeInitialState(const boost::shared_ptr<State>& state)
{
    IL=state;
}