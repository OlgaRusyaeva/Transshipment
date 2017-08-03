//
//  DeterministicLeadTimeTransshipment_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#include "DeterministicLeadTimeTransshipment_Solver.h"
#include "iDistribution.h"
#include "State.h"
#include "Actions.h"
#include "Demand.h"
#include "Cost.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "LeadTimes.h"
#include "NumbersOperations.h"


void DeterministicLeadTimeTransshipment_Solver::solve(iProblem& problem,const int& numberIterations)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    
    initialize(L,T,numberIterations);
    
    State initialState = problem.getState();
    Cost costInState = problem.getCost();
    Demand demand = problem.getDemand();
    LeadTimes leadTimes = problem.getLeadTimes();
    
    
    //if the order level is decision
    //getDeterministicSolution(L,T, costInState, demand,numberIterations,leadTimes);
    
    //if the order level is given as an input
    getDeterministicSolution(L,T,initialState, costInState, demand,numberIterations,leadTimes);
}

std::vector<actionMap_ptr>& DeterministicLeadTimeTransshipment_Solver::getTransshipments()
{
    return transshipments;
}

std::vector<order_ptr>& DeterministicLeadTimeTransshipment_Solver::getInitialState()
{
    return order;
}

void DeterministicLeadTimeTransshipment_Solver::initialize(const int& location, const int& time,const int& numberIterations)
{
    for (int n=0; n<numberIterations; n++) {
        actionMap_ptr ptr_action(new ActionsMap(location,time));
        transshipments.push_back(ptr_action);
        order_ptr ptr_order(new State(location));
        order.push_back(ptr_order);
    }
}

void DeterministicLeadTimeTransshipment_Solver::getDeterministicSolution(const int& location, const int& time, const  Cost& cost, Demand& demand,const int& numberIterations, const LeadTimes& leadTimes)
{
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    const int size=location*location*time;
    const int maxLead=leadTimes.getMaxLeadTimePlusOne();
    
    for (int n=0; n<numberIterations; n++) {
        
        GRBEnv* env = 0;
        GRBModel* model=0;
        GRBVar* varsT=0;
        GRBVar* varsS=0;
        GRBVar* qT=0;
        GRBVar* qS=0;
        GRBLinExpr* lhsObj=0;
        GRBConstr* constr=0;
        
        try {
            
            //initialize
            env = new GRBEnv();
            model = new GRBModel(*env);
            varsT = model->addVars(0, NULL, NULL, NULL, NULL, size);
            varsS = model->addVars(0, NULL, NULL, NULL, NULL, size);
            qT=model->addVars(0, NULL, NULL, NULL, NULL, location);
            qS=model->addVars(0, NULL, NULL, NULL, NULL, location);
            lhsObj = new GRBLinExpr[location*(2*time+1)];
            
            //disable console output
            model->getEnv().set(GRB_IntParam_OutputFlag, 0);
            
            model->update();
            
            //add constraints
            
            //std::cout<<"----------"<<'\n';
            for (int i=0; i<location; i++) {
                GRBLinExpr lns=0;
                for (int j=0; j<location; j++) {
                    if ((leadTimes.getLeadTimeBetweenLocations(i,j)<=time)) {
                    	lns+=varsS[i*location+j]+varsT[i*location+j];
                    }
                }
                lhsObj[i]=lns-qS[i];
            }
            
            for (int i=0; i<location; i++) {
                lhsObj[location+i]=qT[i];
            }
            
            
            for (int t=1; t<time+1; t++) {
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
            
            for (int t=1; t<time; t++) {
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
                        //if ((leadTimes.getLeadTimeBetweenLocations(j,k)<=time-t)) {
                        	lns2+=varsT[(t)*location*location+j*location+k]+varsS[(t)*location*location+j*location+k];
                        //}
                    }
                    lhsObj[(time+1)*location+t*location+j]=lns1-lns2;
                }
            }
            

            
            constr=model->addConstrs(lhsObj, NULL, NULL, NULL, location*(2*time+1));
            
            model->update();
            
        	for (int i=0; i<location; i++) {
                constr[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
                constr[i].set(GRB_DoubleAttr_RHS,0);
                constr[location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[location+i].set(GRB_DoubleAttr_RHS,(int)distribution[i]->sample());
            }
            
            for (int t=1; t<time; t++) {
                for (int j=0; j<location; j++) {
                    constr[(t+1)*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                    constr[(t+1)*location+j].set(GRB_DoubleAttr_RHS,(int)distribution[j]->sample());
                    constr[(time+1)*location+t*location+j].set(GRB_CharAttr_Sense,GRB_EQUAL);
                    constr[(time+1)*location+t*location+j].set(GRB_DoubleAttr_RHS,0);
                }
            }
            
            for (int i=0; i<location; i++) {
                constr[(time+1)*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[(time+1)*location+i].set(GRB_DoubleAttr_RHS,0);
            }
            
            model->update();
            
            //add objective function
            
            GRBLinExpr obj=0;
            
            for (int i=0; i<location; i++) {
                obj+=(cost.getSalesCost()[i]-cost.getOrderingCost()[i])*qT[i]+(-cost.getOrderingCost()[i]-cost.getHoldingCost()[i])*qS[i];
            }
            
            for (int t=0; t<time; t++) {
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
                for (int t = 0; t < time; t++)
                {
                    for (int i=0; i<location; i++) {
                        order[n]->setElement(i,0,qT[i].get(GRB_DoubleAttr_X)+qS[i].get(GRB_DoubleAttr_X));
                        for (int j=0; j<location; j++) {
                            transshipments[n]->setActionPerPeriodBetweenLocations(t,i,j,varsT[t*location*location+i*location+j].get(GRB_DoubleAttr_X)+varsS[t*location*location+i*location+j].get(GRB_DoubleAttr_X));
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
        
        delete[] constr;
        delete[] lhsObj;
        delete[] qT;
        delete[] qS;
        delete[] varsT;
        delete[] varsS;
        delete model;
        delete env;
    }
    

    
}

void DeterministicLeadTimeTransshipment_Solver::getDeterministicSolution(const int& location, const int& time, State& initialState, const  Cost& cost, Demand& demand,const int& numberIterations, const LeadTimes& leadTimes)
{
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    const int size=location*location*time;
    const int maxLead=leadTimes.getMaxLeadTimePlusOne();
    
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
            lhsObj = new GRBLinExpr[location*(2*time)];
            
            //disable console output
            model->getEnv().set(GRB_IntParam_OutputFlag, 0);
            
            model->update();
            
            //add constraints
            
            //std::cout<<"----------"<<'\n';
            for (int i=0; i<location; i++) {
                GRBLinExpr lns=0;
                for (int j=0; j<location; j++) {
                    if ((leadTimes.getLeadTimeBetweenLocations(i,j)<=time)) {
                        lns+=varsS[i*location+j]+varsT[i*location+j];
                    }
                }
                lhsObj[i]=lns;
            }
            
            
            for (int t=1; t<time+1; t++) {
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
            
            for (int t=1; t<time; t++) {
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
                        //if ((leadTimes.getLeadTimeBetweenLocations(j,k)<=time-t)) {
                        lns2+=varsT[(t)*location*location+j*location+k]+varsS[(t)*location*location+j*location+k];
                        //}
                    }
                    lhsObj[(time)*location+t*location+j]=lns1-lns2;
                }
            }
            
            
            
            constr=model->addConstrs(lhsObj, NULL, NULL, NULL, location*(2*time));
            
            model->update();
            
            for (int i=0; i<location; i++) {
                constr[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
                constr[i].set(GRB_DoubleAttr_RHS,numbersOperations::getMaxValue(initialState.getElement(i,0)-(int)distribution[i]->sample(),0));
            }
            
            for (int t=1; t<time; t++) {
                for (int j=0; j<location; j++) {
                    constr[(t)*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                    constr[(t)*location+j].set(GRB_DoubleAttr_RHS,(int)distribution[j]->sample());
                    constr[(time)*location+t*location+j].set(GRB_CharAttr_Sense,GRB_EQUAL);
                    constr[(time)*location+t*location+j].set(GRB_DoubleAttr_RHS,0);
                }
            }
            
            for (int i=0; i<location; i++) {
                constr[(time)*location+i].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                constr[(time)*location+i].set(GRB_DoubleAttr_RHS,0);
            }
            
            model->update();
            
            //add objective function
            
            GRBLinExpr obj=0;
            
            for (int t=0; t<time; t++) {
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
                for (int t = 0; t < time; t++)
                {
                    for (int i=0; i<location; i++) {
                        order[n]->setElement(i,0,initialState.getElement(i,0));
                        for (int j=0; j<location; j++) {
                            transshipments[n]->setActionPerPeriodBetweenLocations(t,i,j,varsT[t*location*location+i*location+j].get(GRB_DoubleAttr_X)+varsS[t*location*location+i*location+j].get(GRB_DoubleAttr_X));
                        }
                    }
                }
                //output the objective function + first period demand*sales price - firt period carryover*holding cost
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


/*void DeterministicLeadTimeTransshipment_Solver::getDeterministicSolution(const int& location, const int& time, State& initialState, const  Cost& cost, Demand& demand,const int& numberIterations, const LeadTimes& leadTimes)
{
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    const int size=location*location*time;
    const int maxLead=leadTimes.getMaxLeadTimePlusOne();
    
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
                    for (int tau=0; tau<maxLead; tau++) {
                        for (int i=0; i<location; i++) {
                            if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau>=0)) {
                                lns+=varsT[(t-tau)*location*location+i*location+j];
                            }
                        }
                    }
                    
                    lhsObj[t*location+j]=lns;
                }
            }
 
 			//std::cout<<"----------"<<'\n';
 			for (int i=0; i<location; i++) {
 				GRBLinExpr lns=0;
 				for (int j=0; j<location; j++) {
 					if ((leadTimes.getLeadTimeBetweenLocations(i,j)<=time)) {
							 lns+=varsS[i*location+j]+varsT[i*location+j];
 						}
                }
                lhsObj[time*location+i]=lns;
            }
 
 
            //std::cout<<"----------"<<'\n';
 
            for (int t=0; t<time-1; t++) {
                for (int j=0; j<location; j++) {
                    GRBLinExpr lns1=0;
                    for (int tau=0; tau<maxLead; tau++) {
                        for (int i=0; i<location; i++) {
                            if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau>=0)) {
                                lns1+=varsS[(t-tau)*location*location+i*location+j];
                            }
                            
                        }
                    }
                    
                    GRBLinExpr lns2=0;
                    for (int k=0; k<location; k++) {
                        if ((leadTimes.getLeadTimeBetweenLocations(j,k)<=time-t)) {
                            lns2+=varsT[(t+1)*location*location+j*location+k]+varsS[(t+1)*location*location+j*location+k];
                        }
                    }
                    lhsObj[time*location+(t+1)*location+j]=lns1-lns2;
                }
            }
            
            
            
            constr=model->addConstrs(lhsObj, NULL, NULL, NULL, 2*location*time);
            
            model->update();
 
 
            for (int i=0; i<location; i++) {
 				constr[time*location+i].set(GRB_CharAttr_Sense,GRB_EQUAL);
 				constr[time*location+i].set(GRB_DoubleAttr_RHS,getMaxValue(initialState.getElement(i,0)-(int)distribution[i]->sample(),0));
 			}
 
            for (int t=0; t<time-1; t++) {
                for (int j=0; j<location; j++) {
                    constr[t*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
                    constr[t*location+j].set(GRB_DoubleAttr_RHS,(int)distribution[j]->sample());
                }
            }
 
 			for (int j=0; j<location; j++) {
 				constr[(time-1)*location+j].set(GRB_CharAttr_Sense,GRB_LESS_EQUAL);
 				constr[(time-1)*location+j].set(GRB_DoubleAttr_RHS,0);
 			}

 
 			for (int t=0; t<time-1; t++) {
 				for (int i=0; i<location; i++) {
 					constr[time*location+(t+1)*location+i].set(GRB_CharAttr_Sense,GRB_EQUAL);
 					constr[time*location+(t+1)*location+i].set(GRB_DoubleAttr_RHS,0);
 				}
 			}
 
            model->update();
 
            //add objective function
            
            GRBLinExpr obj=0;
            
            for (int t=0; t<time; t++) {
                for (int j=0; j<location; j++) {
                    GRBLinExpr sumSold=0;
                    GRBLinExpr sumStored=0;
                    for (int tau=0; tau<maxLead; tau++) {
                        for (int i=0; i<location; i++) {
                            if ((leadTimes.getLeadTimeBetweenLocations(i,j)==tau)&&(t-tau>=0)) {
                                sumSold+=varsT[(t-tau)*location*location+i*location+j];
                                sumStored+=varsS[(t-tau)*location*location+i*location+j];
                            }
                            obj-=cost.getTransshipmentCost()[j]*cost.getDistance(j,i)*(varsT[t*location*location+j*location+i]+varsS[t*location*location+j*location+i]);
                            
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
                for (int t = 0; t < time; t++)
                {
                    for (int i=0; i<location; i++) {
                        for (int j=0; j<location; j++) {
                            transshipments[n]->setActionPerPeriodBetweenLocations(t,i,j,varsT[t*location*location+i*location+j].get(GRB_DoubleAttr_X)+varsS[t*location*location+i*location+j].get(GRB_DoubleAttr_X));
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
*/
