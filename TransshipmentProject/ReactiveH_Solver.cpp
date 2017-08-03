//
//  ReactiveH_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/8/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include <iostream>
#include "types.h"
//#include "iSolver.h"
#include "MainSolver.h"
#include "Profit.h"
#include "CommonMethods.h"

class ReactiveH_Solver: public MainSolver
{
public:
    ReactiveH_Solver(){}
    ~ReactiveH_Solver(){}
    
    void solve(iProblem& problem, Profit& profit)
    {
        int L = problem.getNumberOfStates();
        int T = problem.getTimeHorizon();
        
        State initialState = problem.getState();
        Cost costInState = problem.getCost();
        Actions distance = problem.getDistances();
        Demand demand=problem.getDemand();
        
        row_t solution=row_t(numberOfIterations);
        
        initialState.setInitialState();
        demand.setDemandParameters();
        
        simulate(L,T,initialState, costInState, distance, demand,solution);  
        
        getResults(solution, profit);
        
    }
    
    
    
private:
    
    void simulate(const int& location, const int& time, const State& initialState,const Cost& cost, const Actions& distanceMatrix, Demand& demand, row_t& solution)
    {
        int transshipmentSize=0;
        matrixInt_t count = initializeTransshipmentMatrix(location,location);
        
        Actions maxTransshipmentMatrix(location);
        
        vector<boost::shared_ptr<iDistribution> > distribution;
        distribution=initializeDemandDistribution(demand.getDemandParameters(),location,0);
        
        State state(location);
        
        for (int n=0; n <numberOfIterations;n++)
        {
            int numberOfTransshipments=0;
            float totalProfit = 0;
            
            state=initialState;
            
            for (int t=0; t<time; t++) 
            {
                
                for (int i=0; i<location; i++) {
                    demand.setElement(i, (int)distribution[i]->sample());
                }
                
                float reward = getReward(cost.getSalesCost(),state,demand); 
                
                state=getNewState(state, demand);
                
                float holdingCost = getHoldingCost(cost.getHoldingCost(),state);
                
                if (t!=time-1)
                {  
                    totalProfit+=reward-holdingCost;
                    
                    float transshipmentCost=0;
                    
                    transshipmentCost=gettransshipmentCostByReactiveMethod(location, time, state, cost, distanceMatrix, distribution, 0, maxTransshipmentMatrix);
                    //gettransshipmentCostByReactiveMethod(location, time, state, cost, distanceMatrix, distribution, 0-5, maxTransshipmentMatrix)
                    
                    if (transshipmentCost>0) {
                        numberOfTransshipments++;
                    }
                    
                    totalProfit-=transshipmentCost;
                    
                    for (int i=0; i<location; i++) {
                        for (int j=0; j<location; j++) {
                            if (maxTransshipmentMatrix.getElement(i,j)!=0) {
                                transshipmentSize+=maxTransshipmentMatrix.getElement(i,j);
                                count[i][j]++;
                            }
                        }
                    }
                    
                    
                    
                }
                
                else
                {
                    totalProfit+=reward-holdingCost; 
                }           
                
                
            }
            solution[n]=totalProfit;
            
        }
        
        cout<<"Total amount of stock movement: "<<'\n';
        printMatrix(location, location, count);
        cout<<"average transshipment size: "<<((float)(transshipmentSize/numberOfIterations))<<'\n';
        
    }
    
    float gettransshipmentCostByReactiveMethod(const int& location, const int& time,State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, const int& typeOfMethod, Actions& maxTransshipmentMatrix)
    {
        const int close_to_stock_out_parameter=2;
        
        assert(close_to_stock_out_parameter!=0);
        
        vector_t stock_out=getStockOutLocations(state);
        
        vector_t closeTo_stock_out=getCloseToStockOutLocations(state,close_to_stock_out_parameter);
        
        
        vector_t merge_stock_out=nuke_dupes_no_sort_copy(merge_copy(stock_out, closeTo_stock_out));
        
        //how much and from where to transship
        // matrixInt_t transshipmentMatrix = initializeTransshipmentMatrix(location,location);
        
        State oldState(location);
        oldState=state;
        
        int i=0;
        while (((int)merge_stock_out.size()!=location)&&((int)stock_out.size()!=i))
        {
            int j=0;
            switch (typeOfMethod) 
            {
                case 2:
                    j = getClosestLocation(distanceMatrix.getElementsForState(stock_out[i]),merge_stock_out); 
                    break;
                case 3:
                    j = getRandomLocation(location,merge_stock_out);
                    break;
                case 4:
                    j=getLocationWithHighestInventoryLevel(oldState,merge_stock_out);
                    break;
                case 5:
                    j=getMaxRunOutTimeLocation(oldState,distribution,time,merge_stock_out);
            }
            int amountToTransship = getHowMuchToTransship(state.getElement(j),close_to_stock_out_parameter+1);
            //new state after making a transshipment
            state.setElement(stock_out[i], amountToTransship);
            state.setElement(j, state.getElement(j)-state.getElement(stock_out[i]));
            maxTransshipmentMatrix.setElement(j,stock_out[i],amountToTransship);
            
            closeTo_stock_out=getCloseToStockOutLocations(state,close_to_stock_out_parameter);
            merge_stock_out=nuke_dupes_no_sort_copy(merge_copy(stock_out, closeTo_stock_out));
            i++;
            
            
        }
        
        float transshipmentCost = getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
        
        return transshipmentCost;
        
    }
    
    /*--------------------------------------help methods-----------------------------------*/
    
    //get Location with the lowest transshipment cost for the stock_out Location
    int getClosestLocation(row_t distanceVector, vector_t vectorOfStock_outLocations) 
    {
        
        for (int i=0; i<=(int)vectorOfStock_outLocations.size()-1; i++) {
            distanceVector[vectorOfStock_outLocations[i]]=0;
        }
        int indexClosestLocation=0;
        
        while (distanceVector[indexClosestLocation]==0) {
            indexClosestLocation++;
        }
        
        
        for (int i=0; i<=(int)distanceVector.size()-1; i++) {
            if((distanceVector[i]<distanceVector[indexClosestLocation])&&(distanceVector[i]!=0))
                indexClosestLocation=i;
        }
        return indexClosestLocation;
    } 
    
    //get random Location for the stock_out Location
    int getRandomLocation(const int& length, vector_t vectorOfStock_outLocations)
    {
        int i=0;
        int indexRandomLocation=(int)randomIntNumber(0, length-1);
        while (i<=(int)vectorOfStock_outLocations.size()-1) {
            if (vectorOfStock_outLocations[i]==indexRandomLocation) {
                i=0;
                indexRandomLocation=(int)randomIntNumber(0, length-1);
            }
            else
                i++;
        }
        return indexRandomLocation;
    }
    
    //get Location with the highest inventory level for the stock_out Location
    int getLocationWithHighestInventoryLevel(const State& state,vector_t vectorOfStock_outLocations)
    {
        int indexRichestLocation=0;
        for (int i=0; i<=state.getLength()-1; i++) {
            if ((state.getElement(i)>state.getElement(indexRichestLocation))&&(find(vectorOfStock_outLocations.begin(), vectorOfStock_outLocations.end(), i)==vectorOfStock_outLocations.end()))
            {
                indexRichestLocation=i;
            }
        }
        return indexRichestLocation;
    }
    
    //get how many items to transship from the donor Location
    int getHowMuchToTransship(const int& stockOfDonorLocation, const int& divider)
    {
        return floor((float)(stockOfDonorLocation/ divider));//transship the half of the donor stock
    }
    
    int getMaxRunOutTimeLocation(const State& state,vector<boost::shared_ptr<iDistribution> > distribution,const int& time,vector_t vectorOfStock_outLocations)
    {
        int indexMaxRunOutTimeLocation=0;
        int runOutTime=-1;
        for (int i=0; i<=(int)state.getLength()-1;i++ ) {
            if ((getRunOutTime(state.getElement(i),distribution[i]->mean(),time)>runOutTime)&&(find(vectorOfStock_outLocations.begin(), vectorOfStock_outLocations.end(), i)==vectorOfStock_outLocations.end()))
            {
                indexMaxRunOutTimeLocation=i;
                runOutTime=getRunOutTime(state.getElement(indexMaxRunOutTimeLocation),distribution[indexMaxRunOutTimeLocation]->mean(),time);
            }
        }
        return indexMaxRunOutTimeLocation;    
    }
    
    int getRunOutTime(const int& state, const float& expectedDemand, const int& time)
    {
        return (int)(state/(expectedDemand*time));
    }
    
    vector_t getCloseToStockOutLocations(const State& state, const int& parameter)
    {
        vector_t close_to_stock_out;
        for (int i=0; i<=state.getLength()-1; i++) 
        {
            if ((state.getElement(i)<=parameter)&&(state.getElement(i)!=0))
                close_to_stock_out.push_back(i);
        }
        return close_to_stock_out;
        
    }

    
};