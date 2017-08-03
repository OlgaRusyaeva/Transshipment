//
//  ADP_piecewise_app.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 10/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ADP.h"

void solveByADP_piecewise(const int& location, const int& time, const long& numberOfIterations, State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters,matrixInt_t& indexVector, vecMatrixFloat& breakpoints, vecMatrixFloat& slope)
{
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,40);
    State state(location);
    Demand demand(location);
    
    for (int n=0; n <1000;n++)
    {
        //cout<<"-----------------------n----------------"<<n<<'\n';

        for (int j=0; j<=location-1; j++) 
        {
            state.setElement(j,rand()%initialState.getElement(j)+1);
            //state.setElement(j,initialState.getElement(j));
            
        }
        
        for (int t=0; t<time; t++) 
        { 
            /*cout<<"Time: "<<t<<'\n';
            cout<<"State: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<state.getElement(i)<<'\n';
            }
            
            cout<<"K: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<indexVector[t][i]<<'\n';
            }
            
            cout<<"Breakpoints: "<<'\n';
            for (int i=0; i<location; i++) {
                for (int k=0; k<indexVector[t][i]; k++) {
                    cout<<breakpoints[t][i][k]<<" ; ";
                }
                cout<<'\n';
            }
            
            cout<<"Slope: "<<'\n';
            for (int i=0; i<location; i++) {
                for (int k=0; k<indexVector[t][i]; k++) {
                    cout<<slope[t][i][k]<<" ; ";
                }
                cout<<'\n';
            }*/
            
            Actions decisions = optimizer(state, distanceMatrix, cost.getTransshipmentCost(), indexVector[t], breakpoints[t], slope[t]);
            
            /*cout<<"Actions:"<<'\n';
            for (int i=0; i<location; i++) {
                for (int j=0; j<location; j++) {
                    cout<<decisions.getElement(i,j)<<" ; ";
                }
                cout<<'\n';
            }*/
            
            getNewState(state, decisions);
            
            /*cout<<"Post-decision state: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<state.getElement(i)<<'\n';
            }*/
            

            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
                //cout<<demand.getElement(l)<<'\n';
                
            }
            
            row_t gradient(location);
            //cout<<"Gradient:"<<'\n';
            for (int i=0; i<location; i++) {
                //cout<<"for location "<<i<<'\n';
                if (state.getElement(i)!=0) {
                    if (state.getElement(i)>demand.getElement(i)) {
                        gradient[i]=-cost.getHoldingCost()[i];
                    }
                    else
                        gradient[i]=cost.getSalesCost()[i];
                //cout<<gradient[i]<<'\n';
                updateSlope(indexVector[t][i], breakpoints[t][i], slope[t][i], gradient[i], state.getElement(i),location,n);
                }

            }

            /*cout<<"After update:"<<'\n';
            cout<<"K: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<indexVector[t][i]<<'\n';
            }
            
            cout<<"Breakpoints: "<<'\n';
            for (int i=0; i<location; i++) {
                for (int k=0; k<indexVector[t][i]; k++) {
                    cout<<breakpoints[t][i][k]<<" ; ";
                }
                cout<<'\n';
            }
            
            cout<<"Slope: "<<'\n';
            for (int i=0; i<location; i++) {
                for (int k=0; k<indexVector[t][i]; k++) {
                    cout<<slope[t][i][k]<<" ; ";
                }
                cout<<'\n';
            }*/
            state=getNewState(state, demand);
                
        }
        
    }
    
    
}

//should change k, u, v
void updateSlope(int& k, row_t& u, row_t& v, const float& gradient, const int& inventory, const int& L, const int& iteration)
{
    
    int min_k=-1;
    int max_k=-1;

    float alpha=(float)1/(1+iteration); 
    int epsilon=1;
        
    for (int j=0; j<k; j++) {
        if ((j==k-1)&&(v[j]<=gradient)) {
            min_k=j;
            break;
        }
        else
        {
            if (v[j]<=(1-alpha)*v[j+1]+alpha*gradient) {
            min_k=j;
                break;
            }
        }
    }
    
    for (int j=k-1; j>=0; j--) {
        if ((j==0)&&(v[j]>=gradient)) {
            max_k=j;
            break;
        }
        else
        {
            if ((1-alpha)*v[j-1]+alpha*gradient<=v[j]) {
                max_k=j;
                break;
            }
        }
    }
    //cout<<"min_k: "<<min_k<<'\n';
    //cout<<"max_k: "<<max_k<<'\n';
    //if new breakpoint isn't there yet
    //insert new index and breakpoint
    //in any case update slope
    
    int left=0;
    int right=0;
    
    if ((min_k<0)||(inventory-epsilon<u[min_k])) {
        left=inventory-epsilon;
    }
    else
    {
        left=u[min_k];
    }
    
    if (max_k+1>=k) {
        right=-1;
    }
    else
    {
        if ((max_k<0)||(inventory+epsilon>u[max_k+1])) {
            right=inventory+epsilon;
        }
        else
        {
            right=u[max_k+1];
        }
    }
   // cout<<"left: "<<left<<'\n';
   //cout<<"right: "<<right<<'\n';
    
    int increment=0;
    int pos_a=0;
    int pos_b=0;
    int pos_c=0;
    insert(k, u, v, left,increment, pos_a);
    insert(k, u, v, inventory,increment,pos_b);
    if (right>0) {
        insert(k, u, v, right,increment,pos_c);
    }
    else
        pos_c=k+increment;
    //cout<<"pos_a: "<<pos_a<<'\n';
    //cout<<"pos_c: "<<pos_c<<'\n';
    
    update(v, gradient, pos_a, pos_c, alpha);
    
    k=k+increment;
    
        
}

void insert(int& k, row_t& u, row_t& v,const int& new_breakpoint, int& increase_index, int& pos)
{
    for (int i=0; i<k+increase_index; i++) {
        if (u[i]==new_breakpoint) {
            pos=i;
            break;
        }
        else
        {
            if (u[i]>new_breakpoint) {
                u.insert(u.begin()+i,new_breakpoint);
                v.insert(v.begin()+i,v[i-1]);
                pos=i;
                increase_index++;
                break;
                
            }
            else
            {
                if (i==(k+increase_index)-1) {
                    u.push_back(new_breakpoint);
                    v.push_back(v[i]);
                    pos=i+1;
                    increase_index++;
                    break;
                }
            }
        }
    }
}

//should change v
void update(row_t& v, const float& new_slope, const int& pos_A, const int& pos_B, const float& alpha)
{
    //<=pos_B
    for (int i=pos_A; i<pos_B; i++) {
        v[i]=(1-alpha)*v[i]+alpha*new_slope;
    }
}

float getTransshipmentCostByADP_piecewise(matrixInt_t& indexVector, vecMatrixFloat& breakpoints, vecMatrixFloat& slope,const int& currentPeriod, State& state,const Cost& cost, const Actions& distanceMatrix, Actions& maxTransshipmentMatrix)
{
    maxTransshipmentMatrix = optimizer(state, distanceMatrix, cost.getTransshipmentCost(), indexVector[currentPeriod], breakpoints[currentPeriod], slope[currentPeriod]);
    
    getNewState(state, maxTransshipmentMatrix);
    
    float transshipmentCost = getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}

/*------------------------------------------------Comments---------------------------------------*/

/*
row_t evaluationADP_piecewisePolicy(matrixInt_t& indexVector, vecMatrixFloat& breakpoints, vecMatrixFloat& slope,const int& location, const int& time, const long& numberOfIterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)
{
    
    row_t profit=row_t(numberOfIterations);
    int transshipmentSize=0;
    matrixInt_t count = initializeTransshipmentMatrix(location,location);
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    for (int n=0; n<numberOfIterations; n++) 
    {
        cout<<"---------------------n-----------------"<<n<<'\n';
        float totalProfit = 0;
        State state(location);
        state.setState(initialState);
        
        for (int t=0; t<time; t++) 
        {
            cout<<"Time: "<<t<<'\n';
            cout<<"State: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<state.getElement(i)<<'\n';
            }
            cout<<"Demand: "<<'\n';
            Demand demand(location);
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)(ceil(distribution[i]->sample())));
                cout<<demand.getElement(i)<<'\n';
            }
            
            float reward = getReward(cost.getSalesCost(),state,demand); 
            
            state.setState(getNewState(state, demand));
            
            cout<<"Pre-decision state: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<state.getElement(i)<<'\n';
            }

            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), state);
            
            //delete demand;
            
            
            if ((t!=time-1)) // for last time interval [T-1,T] there is no sence to make a transshipment
            {  
               
                //Actions decisions
                Actions decisions = optimizer(state, distanceMatrix, cost.getTransshipmentCost(), indexVector[t], breakpoints[t], slope[t]);
                
                cout<<"transshipment Matrix"<<'\n';
                for (int a=0; a<location; a++) {
                    for (int b=0; b<location; b++) {
                        cout<<decisions.getElement(a,b)<<" "<<'\n';
                    }
                    cout<<'\n';
                }

                
                getNewState(state, decisions);
                
                cout<<"Post-decision state: "<<'\n';
                for (int i=0; i<location; i++) {
                    cout<<state.getElement(i)<<'\n';
                }
                
                for (int i=0; i<location; i++) {
                    for (int j=0; j<location; j++) {
                        if (decisions.getElement(i,j)!=0) {
                            //cout<<"i "<<i<<'\n';
                            //cout<<"j "<<j<<'\n';
                            //cout<<maxTransshipmentMatrix[i][j]<<'\n';
                            transshipmentSize+=decisions.getElement(i,j);
                            count[i][j]++;
                        }
                    }
                }
                
                //cout<<"transshipment Matrix"<<'\n';
                //printMatrix(location, location, maxTransshipmentMatrix);
                
                cout<<"Total profit per period: "<<'\n';
                cout<<reward-holdingCost-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, decisions)<<'\n';
                
                totalProfit+=reward-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, decisions)-holdingCost;  
                
            }
            else
            {
                totalProfit+=reward-holdingCost;
                cout<<"Total profit per last period: "<<'\n';
                cout<<reward-holdingCost<<'\n';
            }
        }
        
        //delete state;
        profit[n]=totalProfit;
    }
    //std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );
    
    cout<<"Total amount of stock movement: "<<'\n';
    printMatrix(location, location, count);
    cout<<"average transshipment size: "<<(float)(transshipmentSize/numberOfIterations)<<'\n';
    
    return profit;
    
    
}*/
