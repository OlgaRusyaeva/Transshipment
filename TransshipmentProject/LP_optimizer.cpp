//
//  LP_optimizer.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 12/17/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "LP_optimizer.h"
#include "State.h"
#include "Cost.h"
#include "Graph.h"
#include "Actions.h"
#include "NumbersOperations.h"
#include "LeadTimes.h"

 
using namespace std;

LP_solution LP_optimizer::optimize(const State& currentState,  const int& lifePeriod, const Cost& cost,const boost::shared_ptr<PiecewiseLinearFunctionSet>& PL, const int& t)
{    
    int L=currentState.getLength();
    
    //vector_t k=PL.getIndexVector(currentPeriod);
    //matrixInt_t u=PL.getBreakpoints(currentPeriod);
    //matrix_t v=PL.getSlopes(currentPeriod);

    LP_solution solution(L,2);
    
    env = 0;
    vars =0;
    lhsObj =0;
    constr=0;
    
    try {
        
        env = new GRBEnv();
        env->set(GRB_IntParam_Presolve,0);
        env->set(GRB_IntParam_Method,1);
        
        model = new GRBModel(*env);
        
        lhsObj = new GRBLinExpr[2*L];//4*L
        
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
        
        NumVar1=L*L;
        
        NumVar2=0;
        for (int i=0; i<L; i++) {
            int index=PL->get(t,i).getIndex()-1;
            NumVar2+=(index);
            var2_amount.push_back(index);
        }
        NumVar3=L;
        
        NumVars=NumVar1+NumVar2+NumVar3;
        
        vars = model->addVars(0, NULL, NULL, NULL, NULL, NumVars);
        
        model->update();

        
        /* Add variables to the model */
        
        int sum=0;
        for (int i=0; i<L; i++) {
            if (i>0) sum+=(PL->get(t,i-1).getIndex()-1);
            for (int l=0; l<(PL->get(t,i).getIndex()-1); l++) {
                vars[NumVar1+sum+l].set(GRB_DoubleAttr_UB,PL->get(t,i).getBreakpoint(l+1)-PL->get(t,i).getBreakpoint(l));
            }
            
        }

        
        /* Add constraints */
        
        for (int i=0; i<L; i++) {
            GRBLinExpr lns=0;
            for (int j=0; j<L; j++) {
                lns+=vars[j+i*L];
            }
            lhsObj[i]=lns;
        }
        
        sum=0;
        for (int i=0; i<L; i++) {
            GRBLinExpr lns2=0;
            GRBLinExpr lns3=0;
            for (int j=0; j<L; j++) {
                lns2+=vars[i+j*L];
            }
            if (i>0) sum+=(PL->get(t,i-1).getIndex()-1);
            
            for (int l=0; l<(PL->get(t,i).getIndex()-1); l++) {
                lns3+=vars[NumVar1+l+sum];
            }
            lns3+=vars[NumVar1+NumVar2+i];
            
            lhsObj[L+i]=lns3-lns2;//2*L+i
        }
        
        NumConstr=2*L;//4*L
        constr=model->addConstrs(lhsObj, NULL, NULL, NULL,NumConstr) ;
        
        model->update();
        
        for (int i=0; i<L; i++) {
         constr[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
         constr[i].set(GRB_DoubleAttr_RHS,currentState.getElement(i,lifePeriod));
         constr[L+i].set(GRB_CharAttr_Sense,GRB_EQUAL);
         constr[L+i].set(GRB_DoubleAttr_RHS,0);
         }

        /* Add objective*/
        GRBLinExpr obj=0;
        
        sum=0;
        for (int i=0; i<L; i++) {
            for (int j=0; j<L; j++) {
                if (i!=j) {
                	obj+=cost.getTransshipmentCost()[i]*cost.getDistance(i,j)*vars[i*L+j];
                }
            }
            if (i>0) sum+=(PL->get(t,i-1).getIndex()-1);
            for (int l=0; l<(PL->get(t,i).getIndex()-1); l++) {
                obj-=PL->get(t,i).getSlope(l)*vars[NumVar1+sum+l];
                
            }
            obj-=PL->get(t,i).getSlope(PL->get(t,i).getIndex()-1)*vars[NumVar1+NumVar2+i];
            
        }
        
        
        model->setObjective(obj,GRB_MINIMIZE);
        

        
        model->optimize();
        


        
        //model->write("/Users/orusyaeva/Desktop/trial.lp");
        //model->write("/Users/orusyaeva/Desktop/solution.sol");
        
        if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            for (int i = 0; i < L; i++)
            {
                for (int j=0; j<L; j++) {
                    if (i!=j) {
                        solution.setElement(i,j,vars[i*L+j].get(GRB_DoubleAttr_X));
                    }
                }
            }
            
            /*if (checkIfOptimalBasicDegenerate())
            {
                cout<<"Degenerate"<<'\n';
                model->write("/Users/orusyaeva/Documents/ADP_Project/ADP/trial.lp");
                model->write("/Users/orusyaeva/Documents/ADP_Project/ADP/solution.sol");
                
                cout<<"Dual variables:"<<'\n';
                
                for (int i=0; i<L; i++) 
                {
                    cout<<"dual for location : "<<i<<" equal "<<-constr[i].get(GRB_DoubleAttr_Pi)<<'\n';
                }
                
                //getTrueShadowPrices(solution);
            }*/
            //else
            	//getShadowPricesAsDualVariables(solution);
            

        }
        
    } catch(GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch(...) {
        cout << "Exception during optimization" << endl;
    }
	
    //delete[] lhsObj;
    

    
    return solution;
}

/*LP_solution LP_optimizer::optimize(const State& currentState,  const Cost& cost,const boost::shared_ptr<PLValueFunction>& PL, const int& currentPeriod, const LeadTimes& leadTimes)
{
    L=currentState.getLength();
    int T=leadTimes.getMaxLeadTimePlusOne();
    //change shadow price dimensions
    LP_solution solution(L,T);
    
    env = 0;
    vars =0;
    lhsObj=0;
    constr=0;
    
    try {
        
        env = new GRBEnv();
        env->set(GRB_IntParam_Presolve,0);
        env->set(GRB_IntParam_Method,1);
        
        model = new GRBModel(*env);
        
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
        
        // Number of variables
        
        //transshipment decision
        NumVar1=L*L;
        
        //arcs with upper constraints
        NumVar2=0;
        //arcs with lower constraint only (to make the problem feasible)
        NumVar3=L*T;
        for (int i=0; i<L; i++) {
            for (int t=0; t<T; t++) {
                int index=PL->get(currentPeriod,i,t).getIndex()-1;
                NumVar2+=(index);
                var2_amount.push_back(index);
            }
        }

        NumVars=NumVar1+NumVar2+NumVar3;
        
        // Add variables to the model
        vars = model->addVars(0, NULL, NULL, NULL, NULL, NumVars);
        
        model->update();
        
        

        //Add upper bounds
        int sum=0;
        for (int i=0; i<L; i++) {
            for (int t=0; t<T; t++) {
                int suml=0;
                for (int l=0; l<(PL->get(currentPeriod,i,t).getIndex()-1); l++) {
                    vars[NumVar1+sum+l].set(GRB_DoubleAttr_UB,PL->get(currentPeriod,i,t).getBreakpoint(l+1)-PL->get(currentPeriod,i,t).getBreakpoint(l));
                    suml++;
                }
                sum+=suml;
            }
            
        }
        
        
        // Add constraints
        
        lhsObj= new GRBLinExpr[L+NumVar3];
        
        for (int i=0; i<L; i++) {
            GRBLinExpr lns=0;
            for (int j=0; j<L; j++) {
                lns+=vars[j+i*L];
            }
            lhsObj[i]=lns;
        }
        
        sum=0;
        for (int i=0; i<L; i++) {
            for (int t=0; t<T; t++) {
                int suml=0;
                GRBLinExpr lns2=0;
                GRBLinExpr lns3=0;
                for (int j=0; j<L; j++) {
                    if (leadTimes.getLeadTimeBetweenLocations(i,j)==t) {
                        lns2+=vars[i+j*L];
                    }
                }

                for (int l=0; l<(PL->get(currentPeriod,i,t).getIndex()-1); l++) {
                    lns3+=vars[NumVar1+l+sum];
                    suml++;
                }
            	sum+=suml;
                lns3+=vars[NumVar1+NumVar2+i*T+t];
                lhsObj[L+i*T+t]=lns3-lns2;//2*L+i
            }
            
        }
        
        NumConstr=L+NumVar3;
        constr=model->addConstrs(lhsObj, NULL, NULL, NULL,NumConstr) ;
        
        model->update();
        
        for (int i=0; i<L; i++) {
            constr[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
            constr[i].set(GRB_DoubleAttr_RHS,currentState.getElement(i,0));
        }
        
        for (int i=0; i<L; i++) {
            for (int t=0; t<T; t++) {
            	constr[L+i*T+t].set(GRB_CharAttr_Sense,GRB_EQUAL);
                if (t==0) {
                    constr[L+i*T+t].set(GRB_DoubleAttr_RHS,0);
                }
                else
            		constr[L+i*T+t].set(GRB_DoubleAttr_RHS,currentState.getElement(i,t));
            }
        }
        
        // Add objective
        GRBLinExpr obj=0;
        
        sum=0;
        for (int i=0; i<L; i++) {
            for (int j=0; j<L; j++) {
                if (i!=j) {
                    obj+=cost.getTransshipmentCost()[i]*cost.getDistance(i,j)*vars[i*L+j];
                }
            }
            for (int t=0; t<T; t++) {
            int suml=0;
            //if (i>0) sum+=(PL->get(t,i-1).getIndex()-1);
           	 	for (int l=0; l<(PL->get(currentPeriod,i,t).getIndex()-1); l++) {
               		 obj-=PL->get(currentPeriod,i,t).getSlope(l)*vars[NumVar1+sum+l];
                    suml++;
                }
                
            sum+=suml;
                
            	obj-=PL->get(currentPeriod,i,t).getSlope(PL->get(currentPeriod,i,t).getIndex()-1)*vars[NumVar1+NumVar2+i*T+t];
            }
        }
        
        
        model->setObjective(obj,GRB_MINIMIZE);
        
        
        
        model->optimize();
        
       // model->write("/Users/orusyaeva/Desktop/trial.lp");
        //model->write("/Users/orusyaeva/Desktop/solution.sol");
        

        
        if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            for (int i = 0; i < L; i++)
            {
                for (int j=0; j<L; j++) {
                    if (i!=j) {
                        solution.setElement(i,j,vars[i*L+j].get(GRB_DoubleAttr_X));
                    }
                }
            }
            
            
            
        }
    
        
    } catch(GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch(...) {
        cout << "Exception during optimization" << endl;
    }
    

    
    
    
    return solution;

}*/

LP_solution LP_optimizer::optimize(const State& currentState,  const Cost& cost,const boost::shared_ptr<PLValueFunction>& PL, const int& currentPeriod, const LeadTimes& leadTimes, const int& limit, const bool& flag)
{
    int L=currentState.getLength();
    //change shadow price dimensions
    //limit+1?
    LP_solution solution(L,limit+1);
    
    env = 0;
    vars =0;
    lhsObj=0;
    constr=0;
    
    try {
        
        env = new GRBEnv();
        env->set(GRB_IntParam_Presolve,0);
        env->set(GRB_IntParam_Method,1);
        
        model = new GRBModel(*env);
        
        //disable console output
        model->getEnv().set(GRB_IntParam_OutputFlag, 0);
        
        int time_index=currentPeriod+1;
        int lead_index=0;
        
        /* Number of variables */
        
        //transshipment decision
        NumVar1=L*L;
        
        //arcs with upper constraints
        NumVar2=0;
        //arcs with lower constraint only (to make the problem feasible)
        NumVar3=L*(limit);
        for (int i=0; i<L; i++) {
            for (int t=0; t<limit; t++) {
                if (flag) {
                    time_index=currentPeriod+1+t;
                }
                else
                {
                    lead_index=t;
                }
                int index=PL->get(time_index,i,lead_index).getIndex()-1;
                NumVar2+=(index);
                var2_amount.push_back(index);
            }
        }
        
        NumVars=NumVar1+NumVar2+NumVar3;
        
        /* Add variables to the model */
        vars = model->addVars(0, NULL, NULL, NULL, NULL, NumVars);
        
        model->update();
        
        
        
        /*Add upper bounds*/
        int sum=0;
        for (int i=0; i<L; i++) {
            for (int t=0; t<limit; t++) {
                int suml=0;
                if (flag) {
                    time_index=currentPeriod+t+1;
                }
                else
                {
                    lead_index=t;
                }
                for (int l=0; l<(PL->get(time_index,i,lead_index).getIndex()-1); l++) {
                    vars[NumVar1+sum+l].set(GRB_DoubleAttr_UB,PL->get(time_index,i,lead_index).getBreakpoint(l+1)-PL->get(time_index,i,lead_index).getBreakpoint(l));
                    suml++;
                }
                sum+=suml;
            }
            
        }
        
        
        /* Add constraints */
        
        lhsObj= new GRBLinExpr[L+NumVar3];
        
        for (int i=0; i<L; i++) {
            GRBLinExpr lns=0;
            for (int j=0; j<L; j++) {
                lns+=vars[j+i*L];
            }
            lhsObj[i]=lns;
        }
        
        sum=0;
        for (int i=0; i<L; i++) {
            for (int t=0; t<limit; t++) {
                int suml=0;
                GRBLinExpr lns2=0;
                GRBLinExpr lns3=0;
                if (flag) {
                    time_index=currentPeriod+t+1;
                }
                else
                {
                    lead_index=t;
                }
                for (int j=0; j<L; j++) {
                    if (leadTimes.getLeadTimeBetweenLocations(i,j)==t) {
                        lns2+=vars[i+j*L];
                    }
                }
                
                for (int l=0; l<(PL->get(time_index,i,lead_index).getIndex()-1); l++) {
                    lns3+=vars[NumVar1+l+sum];
                    suml++;
                }
                sum+=suml;
                lns3+=vars[NumVar1+NumVar2+i*(limit)+t];
                lhsObj[L+i*(limit)+t]=lns3-lns2;//2*L+i
            }
            
        }
        
        NumConstr=L+NumVar3;
        constr=model->addConstrs(lhsObj, NULL, NULL, NULL,NumConstr) ;
        
        model->update();
        
        for (int i=0; i<L; i++) {
            constr[i].set(GRB_CharAttr_Sense,GRB_EQUAL);
            constr[i].set(GRB_DoubleAttr_RHS,currentState.getElement(i,0));
        }
        
        for (int i=0; i<L; i++) {
            for (int t=0; t<limit; t++) {
                constr[L+i*(limit)+t].set(GRB_CharAttr_Sense,GRB_EQUAL);
                if (t!=leadTimes.getMaxLeadTimePlusOne()-1) {
                    constr[L+i*(limit)+t].set(GRB_DoubleAttr_RHS,currentState.getElement(i,t+1));
                }
                else
                    constr[L+i*(limit)+t].set(GRB_DoubleAttr_RHS,0);
            }
        }
        
        /* Add objective*/
        GRBLinExpr obj=0;
        
        sum=0;
        for (int i=0; i<L; i++) {
            for (int j=0; j<L; j++) {
                if (i!=j) {
                    obj+=cost.getTransshipmentCost()[i]*cost.getDistance(i,j)*vars[i*L+j];
                }
            }
            for (int t=0; t<limit; t++) {
                int suml=0;
                if (flag) {
                    time_index=currentPeriod+t+1;
                }
                else
                {
                    lead_index=t;
                }
                //if (i>0) sum+=(PL->get(t,i-1).getIndex()-1);
                for (int l=0; l<(PL->get(time_index,i,lead_index).getIndex()-1); l++) {
                    obj-=PL->get(time_index,i,lead_index).getSlope(l)*vars[NumVar1+sum+l];
                    suml++;
                }
                
                sum+=suml;
                
                obj-=PL->get(time_index,i,lead_index).getSlope(PL->get(time_index,i,lead_index).getIndex()-1)*vars[NumVar1+NumVar2+i*(limit)+t];
            }
        }
        
        
        model->setObjective(obj,GRB_MINIMIZE);
        
        
        
        model->optimize();
        
        //model->write("/Users/orusyaeva/Desktop/trial.lp");
        //model->write("/Users/orusyaeva/Desktop/solution.sol");
        
        /*std::cout<<"Shadow prices:"<<'\n';
        for (int i=0; i<solution.getShadowPriceNumber(); i++) {
            std::cout<<i<<": "<<-constr[i].get(GRB_DoubleAttr_Pi)<<'\n';
        }*/
        
        
        if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            for (int i = 0; i < L; i++)
            {
                for (int j=0; j<L; j++) {
                    if (i!=j) {
                        solution.setElement(i,j,vars[i*L+j].get(GRB_DoubleAttr_X));
                    }
                }
            }
            
            
            
        }
        
        
    } catch(GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch(...) {
        cout << "Exception during optimization" << endl;
    }
    
    
    
    
    
    return solution;
    
}




float LP_optimizer::optimalObjectiveFunction()
{
    if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
        return (-model->get(GRB_DoubleAttr_ObjVal));
    }
    else
    {
    	cout << "Exception during optimization" << endl;
        return 0;
    }
}


bool LP_optimizer::checkIfOptimalBasicDegenerate()
{
    bool flag=false;
    vector_t basicVariables;
    getBasicVariables(basicVariables);
    for (int i=0; i<(int)basicVariables.size(); i++) {
        if ((vars[basicVariables[i]].get(GRB_DoubleAttr_X)==0)||(vars[basicVariables[i]].get(GRB_DoubleAttr_X)==vars[basicVariables[i]].get(GRB_DoubleAttr_UB))) {
            flag=true;
            break;
        }
    }
    return flag;
}

void LP_optimizer::getBasicVariables(vector_t& basic)
{
    
    for (int i=0; i<NumVars; i++) {
        if (vars[i].get(GRB_IntAttr_VBasis)==0) {
            //cout<<"Basic variable: "<<i<<'\n';
            basic.push_back(i);
    	}
    }
    
    /*for (int i=0; i<NumConstr; i++) {
        if (constr[i].get(GRB_IntAttr_CBasis)==0) {
            basic.push_back(NumVars+i);
        }
    }*/
    
}


void LP_optimizer::getTrueShadowPrices(LP_solution& currentSolution, const int& L, const int& limit, const LeadTimes& leadTimes)
{
    Graph g(NumVars,NumConstr+1,initializeWeights(),initializeFlow(),initializeCapacities(L,limit),L,limit, var2_amount,leadTimes.getLeadTimes());
    g.initializeGraph();
    //g.printGraph();
    
   	//printDuals();
    
    row_t pi_plus=leastCostFlowAugmentingPath(true,g);
    row_t pi_minus=leastCostFlowAugmentingPath(false,g);
    
    transform(pi_plus,L,limit);
    transform(pi_minus,L,limit);
    
    /*cout<<"min shadow prices:"<<'\n';
    for (int i=0; i<L; i++) {
        cout<<"plus: "<<pi_plus[i]<<'\n';
        cout<<"minus: "<<pi_minus[i]<<'\n';
    }*/
    
    //CHECK: minus and plus
    for (int i=0; i<currentSolution.getShadowPriceNumber(); i++) {
        if (numbersOperations::approximatelyEqual(pi_minus[i],numeric_limits<int>::max(),0.001)) {
            currentSolution.setShadowPriceMinus(i,-constr[i].get(GRB_DoubleAttr_Pi));
        }
        else
        {
            //pi_minus max problem=-pi_minus min problem;
            //the sign of pi_minus min problem show the direction
            //we don't need it => -(-pi_minus)=pi_minus
            currentSolution.setShadowPriceMinus(i,pi_minus[i]);
        }
    }
    
    for (int i=0; i<currentSolution.getShadowPriceNumber(); i++) {
        if (numbersOperations::approximatelyEqual(pi_plus[i],numeric_limits<int>::max(),0.001)) {
            currentSolution.setShadowPricePlus(i,-constr[i].get(GRB_DoubleAttr_Pi));
        }
        else
        {
            //pi_plus max problem=-pi_plus min problem
            currentSolution.setShadowPricePlus(i,-pi_plus[i]);
        }
    }
    
   	//printShadowPrices(currentSolution);
}

void LP_optimizer::getShadowPricesAsDualVariables(LP_solution& solution)
{
    for (int i=0; i<solution.getShadowPriceNumber(); i++) {
        solution.setShadowPricePlus(i,(-constr[i].get(GRB_DoubleAttr_Pi)));
        solution.setShadowPriceMinus(i,(-constr[i].get(GRB_DoubleAttr_Pi)));
    }

}


row_t LP_optimizer::leastCostFlowAugmentingPath(const bool& flag, Graph& g)
{
    int nVertex=g.numberV();
    row_t dual(nVertex);
    vector_t list(nVertex);
    
    int last=nVertex-1;
    
    for (int i=0; i<nVertex-1; i++) {
        dual[i]=numeric_limits<int>::max();//big M
        list[i]=0;
    }
    list[nVertex-1]=numeric_limits<int>::max();
    int n=nVertex-1;
    bool repeat=true;
    
    int edge_id1=0;
    int edge_id2=0;
    
    map<int,int> H_n, G_n;
    typedef map<int,int>:: const_iterator iter;
    
    while (repeat) {
        
        if (flag) {
            H_n=g.getAdjacentVerteces(n);
            G_n=g.getInvAdjacentVerteces(n);
        }
        else
        {
            G_n=g.getAdjacentVerteces(n);
            H_n=g.getInvAdjacentVerteces(n);
        }
        
        for (iter it=H_n.begin(); it!=H_n.end(); it++) 
        {
            int i=it->first;
            for (int k=0; k<=it->second; k++) {
                if (flag) {
                    edge_id1=g.getEdge(i,n,k);
                }
				else
                    edge_id1=g.getEdge(n,i,k);
                if (g.getFlow(edge_id1)<g.getCapacity(edge_id1)) {
                    if (numbersOperations::definitelyLessThan(dual[n]+g.getWeight(edge_id1),dual[i],0.001)) {
                        dual[i]=dual[n]+g.getWeight(edge_id1);
                        if (list[i]==0) {
                            list[last]=i;
                            last=i;
                            list[i]=numeric_limits<int>::max();
                        }
                        else
                        {
                            if (list[i]<0) {
                                list[i]=list[n];
                                list[n]=i;
                            }
                        }
                    }
                }
            }
        }
        
        for (iter it=G_n.begin(); it!=G_n.end(); it++) 
        {
            int j=it->first;
            for (int p=0; p<=it->second; p++) {
                if (flag) {
                    edge_id2=g.getEdge(n,j,p);
                }
				else
                    edge_id2=g.getEdge(j,n,p);
                if (g.getFlow(edge_id2)>0) {
                    if (numbersOperations::definitelyLessThan(dual[n]-g.getWeight(edge_id2),dual[j],0.001)) {
                        dual[j]=dual[n]-g.getWeight(edge_id2);
                        if (list[j]==0) {
                            list[last]=j;
                            last=j;
                            list[j]=numeric_limits<int>::max();
                        }
                        else
                        {
                            if (list[j]<0) {
                                list[j]=list[n];
                                list[n]=j;
                            }
                        }
                    }
                }
            }
        }
        
        int m=n;
        if (list[n]==numeric_limits<int>::max()) {
            repeat=false;
        }
        else
        {
            n=list[n];
            list[m]=-1;
            
        }
    }
    
    return dual;
    
}

row_t LP_optimizer::initializeWeights()
{
    row_t weights(NumVars);
    for (int i=0; i<(int)weights.size(); i++) {
        weights[i]=vars[i].get(GRB_DoubleAttr_Obj);
    }
    return weights;
}
vector_t LP_optimizer::initializeCapacities(const int& L, const int& limit)
{
    vector_t capacities(NumVars);
    for (int i=0; i<(int)capacities.size(); i++) {
        capacities[i]=vars[i].get(GRB_DoubleAttr_UB);
    }
    
	for (int i=0; i<L*L; i++) {
        capacities[i]=numeric_limits<int>::max();
    }

    for (int j=(int)capacities.size()-L*limit; j<(int)capacities.size(); j++) {
        capacities[j]=numeric_limits<int>::max();
    }
    return capacities;
}

vector_t LP_optimizer::initializeFlow()
{
    vector_t flows(NumVars);
	for (int i=0; i<(int)flows.size(); i++) {
        flows[i]=vars[i].get(GRB_DoubleAttr_X);
    }
    return flows;
}

void LP_optimizer::transform(row_t& pi, const int& L, const int& limit)
{
    row_t old_pi=pi;
    for (int i=0; i<L; i++) {
        pi[i]=old_pi[i];
    }
    
    for (int i=0; i<L; i++) {
        for (int t=0; t<limit; t++) {
            pi[L+i*limit+t]=old_pi[L+t*L+i];
        }
    }
}

/*void LP_optimizer::printDuals()
{
    cout<<"LP duals:"<<'\n';
    for (int i=0; i<L; i++) 
    {
        cout<<i<<" : "<<(-constr[i].get(GRB_DoubleAttr_Pi))<<'\n';
    }
}
void LP_optimizer::printShadowPrices(const LP_solution& solution)
{
    cout<<"Shadow prices:"<<'\n';
    for (int i=0; i<L; i++) {
        cout<<i<<" : "<<solution.getShadowPricePlus(i)<<'\n';
        cout<<i<<" : "<<solution.getShadowPriceMinus(i)<<'\n';
    }
}*/

