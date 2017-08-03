//
//  StochasticRollingHorizon_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/12/15.
//
//

//get the initial replenishment

#include "StochasticRollingHorizon_Solver.hpp"
#include "State.h"
#include "iProblem.h"
#include "Demand.h"
#include "LeadTimes.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ProblemMethods.h"

void StochasticRollingHorizon_Solver::solve(iProblem& problem, int horizonLength, const int& sampleSize)
{
    const int location = problem.getStateDimension();
    const int time= problem.getTimeHorizon();
    Cost cost = problem.getCost();
    Demand demand = problem.getDemand();
    LeadTimes leadTimes = problem.getLeadTimes();
    
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
    
    const int size=location*location*horizonLength*sampleSize;
    
    try {
        
        //initialize
        env = new GRBEnv();
        model = new GRBModel(*env);
        varsT = model->addVars(0, NULL, NULL, NULL, NULL, size);
        varsS = model->addVars(0, NULL, NULL, NULL, NULL, size);
        qT=model->addVars(0, NULL, NULL, NULL, NULL, location*sampleSize);
        qS=model->addVars(0, NULL, NULL, NULL, NULL, location*sampleSize);
        lhsObj = new GRBLinExpr[sampleSize*location*(2*horizonLength+1)+(sampleSize-1)*location];
        
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
        
        model->update();
        
        //add constraints
        
        //std::cout<<"----------"<<'\n';
        for (int w=0; w<sampleSize; w++) {
            for (int i=0; i<location; i++) {
                GRBLinExpr lns=0;
                for (int j=0; j<location; j++) {
                    if ((leadTimes.getLeadTimeBetweenLocations(i,j)<=horizonLength)) {
                        lns+=varsS[w*location*location+i*location+j]+varsT[w*location*location+i*location+j];
                    }
                }
                lhsObj[w*location+i]=lns-qS[w*location+i];
            }
        }

    	for (int w=0; w<sampleSize; w++) {
        	for (int i=0; i<location; i++) {
            	lhsObj[location*sampleSize+w*location+i]=qT[w*location+i];
        	}
        }
        
        

        for (int t=1; t<horizonLength+1; t++) {
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
                	lhsObj[(t+1)*location*sampleSize+w*location+j]=lns;
            	}
            }
        }
        
        
        //std::cout<<"----------"<<'\n';
        
        for (int t=1; t<horizonLength; t++) {
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
                    	if ((leadTimes.getLeadTimeBetweenLocations(j,k)<=horizonLength-t)) {
                        	lns2+=varsT[(t)*location*location*sampleSize+w*location*location+j*location+k]+varsS[(t)*location*location*sampleSize+w*location*location+j*location+k];
                    	}
                	}
                	lhsObj[(horizonLength+1)*location*sampleSize+t*location*sampleSize+w*location+j]=lns1-lns2;
            	}
            }
        }
        

        for (int w=0; w<sampleSize-1; w++) {
            for (int i=0; i<location; i++) {
                GRBLinExpr lns=0;
                lns=qS[w*location+i]+qT[w*location+i]-qS[(w+1)*location+i]-qT[(w+1)*location+i];
                lhsObj[sampleSize*location*(2*horizonLength+1)+w*location+i]=lns;
            }
        }
        
        
        constr=model->addConstrs(lhsObj, NULL, NULL, NULL, sampleSize*location*(2*horizonLength+1)+(sampleSize-1)*location);
        
        model->update();
        
        for (int w=0; w<sampleSize; w++) {
        	for (int i=0; i<location; i++) {
            	constr[w*location+i].set(GRB_CharAttr_Sense,GRB_EQUAL);
            	constr[w*location+i].set(GRB_DoubleAttr_RHS,0);
            	constr[location*sampleSize+w*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
            	constr[location*sampleSize+w*location+i].set(GRB_DoubleAttr_RHS,(int)distribution[i]->sample());
        	}
        }
        
        for (int t=1; t<horizonLength; t++) {
        	for (int w=0; w<sampleSize; w++) {
            	for (int j=0; j<location; j++) {
               	 	constr[(t+1)*location*sampleSize+w*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                	constr[(t+1)*location*sampleSize+w*location+j].set(GRB_DoubleAttr_RHS,(int)distribution[j]->sample());
                	constr[(horizonLength+1)*location*sampleSize+t*location*sampleSize+w*location+j].set(GRB_CharAttr_Sense,GRB_EQUAL);
                	constr[(horizonLength+1)*location*sampleSize+t*location*sampleSize+w*location+j].set(GRB_DoubleAttr_RHS,0);
            	}
            }
        }
        
        //demand at the last period is equal to zero
        //in order to avoid transshipments in the last period
    	for (int w=0; w<sampleSize; w++) {
        	for (int i=0; i<location; i++) {
                if (horizonLength<time) {
                    constr[(horizonLength+1)*location*sampleSize+w*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                    constr[(horizonLength+1)*location*sampleSize+w*location+i].set(GRB_DoubleAttr_RHS,(int)distribution[i]->sample());
                }
                else
                {
           	 		constr[(horizonLength+1)*location*sampleSize+w*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
            		constr[(horizonLength+1)*location*sampleSize+w*location+i].set(GRB_DoubleAttr_RHS,0);
                }
        	}
        }
        
        for (int w=0; w<sampleSize-1; w++) {
            for (int i=0; i<location; i++) {
                constr[sampleSize*location*(2*horizonLength+1)+w*location+i].set(GRB_CharAttr_Sense,GRB_EQUAL);
                constr[sampleSize*location*(2*horizonLength+1)+w*location+i].set(GRB_DoubleAttr_RHS,0);
            }

        }
        
        
        model->update();
        
        //add objective function
        
        GRBLinExpr obj=0;
        float prob=(float)1/sampleSize;
        for (int w=0; w<sampleSize; w++) {
            for (int i=0; i<location; i++) {
                obj+=prob*((cost.getSalesCost()[i]-cost.getOrderingCost()[i])*qT[w*location+i]+(-cost.getOrderingCost()[i]-cost.getHoldingCost()[i])*qS[w*location+i]);
            }
        }

        
        for (int t=0; t<horizonLength; t++) {
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

order_ptr StochasticRollingHorizon_Solver::getInitialState() const
{
    return order;
}