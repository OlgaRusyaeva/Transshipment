//
// 1. Dynamic Programming method to get a look-up table with value function in each state (vector of inventories) for each time period
// 2. Evaluation of DP (simulation)
// 3. Methods used for evaluation of DP
//

#include "ExactDP.h"
#include <list>

//#include </Developer/usr/llvm-gcc-4.2/lib/gcc/i686-apple-darwin11/4.2.1/include/omp.h> //for multithreading

//Exact dynamic programming method
void solveExactDP(const int& location, const int& time, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters,ValueFunction& valueFunction,Hashing& hashFunction)
{
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    State state(location);
    state=initialState;
    
    State maxBorder(location);
    int maxValue=0;
    
    for (int i=0; i<location; i++) {
        maxValue+=state.getElement(i);
    }
    
    int size_inventoryLevels=1;
    for (int i=0; i<location; i++) {
        maxBorder.setElement(i,maxValue);
        size_inventoryLevels*=maxValue+1;
    }

    Array2DInt<float> allInventoryLevelsForInitialState(location,size_inventoryLevels);
    allInventoryLevelsForInitialState.initialize();
    generatePossibleDemands(maxBorder,allInventoryLevelsForInitialState);
    
    
    for (int t=time-1; t>=0; t--) 
    {
        for (int inv=0; inv<allInventoryLevelsForInitialState.size(); inv++) 
        {
            float totalProfit = 0;
            
            for (int j=0; j<=location-1; j++) 
            {
                state.setElement(j,(int)allInventoryLevelsForInitialState(inv,j));
                
            }
            size_t initialStateHash = hashFunction.hash_value(state);
            
            State minBorder(location);
            int size_demands=1;
            for (int i=0; i<location; i++) {
                minBorder.setElement(i,state.getElement(i));
                size_demands*=state.getElement(i)+1;
            }
            Array2DInt<float> possibleDemands(location,size_demands);
            possibleDemands.initialize();
            generatePossibleDemands(minBorder,possibleDemands); 
            
            Demand demand(location);
            State stateAfterDemandSatisfaction(location);
            
            for (int i=0; i<possibleDemands.size(); i++) 
            {
                
                float prob=1;
                bool flag;
                for (int j=0; j<location; j++) {
                    flag=true;
                    demand.setElement(j, (int)possibleDemands(i,j));
                    if ((state.getElement(j)-demand.getElement(j)==0)) {
                        flag=false;
                    }
                    prob*=probabilityOfDemand(demand.getElement(j),distribution[j],flag);
                }

                
                float reward = getReward(cost.getSalesCost(),state,demand); 
                
                stateAfterDemandSatisfaction=getNewState(state, demand);
                
                float holdingCost=getHoldingCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
                
                if (t!=time-1) 
                {
                    
                    vecMatrixInt actionsForState=transhipmentFromState(stateAfterDemandSatisfaction,false); 
                    
                    /*cout<<"actions matrix: "<<'\n';
                    for (int l=0; l<location; l++) {
                        cout<<"for location: "<<l<<'\n';
                        printMatrix(actionsForState[l].size(),location,actionsForState[l]);
                    }*/
                    
                    State indexVector(location);
                    int size_actions=1;
                    getIndexVector(indexVector, actionsForState,size_actions);

                    Array2DInt<float> combinationsOfRowsActionMatrix(location, size_actions);
                    combinationsOfRowsActionMatrix.initialize();
                    generatePossibleDemands(indexVector,combinationsOfRowsActionMatrix);
                    
                    
                    float maxCost=-numeric_limits<float>::max();
                    
                    State maxState(location);
                    
                    //row_t costInPeriods(location);
                    //#pragma omp parallel for shared(costInPeriods,actionsForState,combinationsOfRowsActionMatrix)
                    for (int p=0; p<combinationsOfRowsActionMatrix.size(); p++) {
                        float costInPeriod =0;
                        State newState(location);
                        Actions transshipmentMatrix(location);
                        newState=stateAfterDemandSatisfaction;
                        for (int j=0; j<=location-1; j++) {
                            int sumTransshipAmountFromJ=0;
                            for (int k=0; k<=location-1; k++) {
                                int amountToTransship=0;
                                int index=(int)combinationsOfRowsActionMatrix(p,j);
                                if(k!=j) amountToTransship=actionsForState[j][index][k];
                                sumTransshipAmountFromJ+=amountToTransship;
                                newState.setElement(k,newState.getElement(k)+amountToTransship);
                                transshipmentMatrix.setElement(j,k,amountToTransship);
                                
                            }
                            newState.setElement(j,newState.getElement(j)-sumTransshipAmountFromJ);
                        }
                        costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                        
                        costInPeriod+=valueFunction.getValueFunction(t+1, hashFunction.hash_value(newState),0);
                        if (costInPeriod>=maxCost) {
                            maxCost=costInPeriod;
                            maxState=newState;
                        }
                        //costInPeriods[p]=costInPeriod;
                        
                    }
                    
                    /*for (int p=0; p<(int)combinationsOfRowsActionMatrix.size(); p++) 
                    {
                        if (costInPeriods[p]>=maxCost) {
                            maxCost=costInPeriods[p];
                        }

                    }*/
                    
                    totalProfit+=prob*(reward-holdingCost+maxCost);
                    
                }
                else
                    totalProfit+=prob*(reward-holdingCost);
                
            }
            //update value function for the state
            valueFunction.setValueFunction(t,initialStateHash,0,totalProfit); 
            initialStateHash = 0;
            
        }
        
    } 
    
    
}

float getTransshipmentCostByExactDP(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,const Demand& demand, const int& currentPeriod, ValueFunction& valueFunction,Actions& maxTransshipmentMatrix,Hashing& hashFunction)
{
        vecMatrixInt actionsForState=transhipmentFromState(state,false); 
    
        State indexVector(location);
        int size_actions=1;
        getIndexVector(indexVector, actionsForState,size_actions);
        Array2DInt<float> combinationsOfRowsActionMatrix(location, size_actions);
        combinationsOfRowsActionMatrix.initialize(); 
        generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);              
        
        float maxCost=-numeric_limits<float>::max();
        State maxState(location);
        
        State newState(location);
        Actions transshipmentMatrix(location);
    
        for (int p=0; p<combinationsOfRowsActionMatrix.size(); p++) {
            
            float costInPeriod =0;

            newState=state;
            for (int j=0; j<=location-1; j++) {
                int sumTransshipAmountFromJ=0;
                
                for (int i=0; i<=location-1; i++) {
                    int amountToTransship=0;
                    if(i!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][i];
                    newState.setElement(i,newState.getElement(i)+amountToTransship);
                    sumTransshipAmountFromJ+=amountToTransship;
                    transshipmentMatrix.setElement(j,i,amountToTransship);
                    
                }
                newState.setElement(j,newState.getElement(j)-sumTransshipAmountFromJ);
            }
            costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
            
            costInPeriod+=valueFunction.getValueFunction(currentPeriod+1, hashFunction.hash_value(newState),0);
            
            if (costInPeriod>=maxCost) {
                maxCost=costInPeriod;
                maxState=newState;
                maxTransshipmentMatrix=transshipmentMatrix;
                /*for (int s=0; s<location; s++) {
                    for (int y=0; y<location; y++) {
                        maxTransshipmentMatrix.setElement(s,y,transshipmentMatrix.getElement(s,y));
                    }
                }*/
            }
            
        }
        
        state=maxState;
        
     return getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix);  

    
}

/*--------------------------------Comments------------------------------------*/

/*
//Evaluation procedure for the DP algorithm
row_t evaluationPolicy(const ValueFunction& valueFunction, Hashing& hashFunction,const int& location, const int& time, const long& numberOfIterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)
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
                demand.setElement(i, distribution[i]->sample());
                cout<<demand.getElement(i)<<'\n';
            }
            
            float reward = getReward(cost.getSalesCost(),state,demand); 
            //cout<<"reward: "<<reward<<'\n';
            
            
            state.setState(getNewState(state, demand));
            
            cout<<"Pre-decision state: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<state.getElement(i)<<'\n';
            }
            
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), state);
            
            //delete demand;
            
            
            if (t!=time-1) 
            {
                
                vecMatrixInt actionsForState=transhipmentFromState(state,false); 
                
                Array2DInt<float> combinationsOfRowsActionMatrix=generateAllCombinationsOfTransshipmentActions(actionsForState,location);                
                
                float maxCost=-numeric_limits<float>::max();
                State maxState(location);
                //int maxP=-1;
                Actions maxTransshipmentMatrix(location);// = initializeTransshipmentMatrix(location,location);
                
               // row_t costInPeriods=row_t((int)combinationsOfRowsActionMatrix.size());
               // vector<State*> maxStates((int)combinationsOfRowsActionMatrix.size());// = vector<State*>((int)combinationsOfRowsActionMatrix.size());
                
               // #pragma omp parallel  shared(costInPeriods,maxStates)                
                for (int p=0; p<combinationsOfRowsActionMatrix.size(); p++) {
                    
                    float costInPeriod =0;
                    State newState(location);
                    Actions transshipmentMatrix(location);// = initializeTransshipmentMatrix(location,location);
                    newState.setState(state);
                    for (int j=0; j<=location-1; j++) {
                        int sumTransshipAmountFromJ=0;
                        
                        for (int i=0; i<=location-1; i++) {
                            int amountToTransship=0;
                            if(i!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][i];
                            newState.setElement(i,newState.getElement(i)+amountToTransship);
                            sumTransshipAmountFromJ+=amountToTransship;
                            transshipmentMatrix.setElement(j,i,amountToTransship);
                            
                        }
                        newState.setElement(j,newState.getElement(j)-sumTransshipAmountFromJ);
                    }
                    costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                    //costInPeriod-=getHoldingCost(cost.getHoldingCost(), *newState);

                    costInPeriod+=valueFunction.getValueFunction(t+1, hashFunction.hash_value(newState),0);
                    
                    //costInPeriods[p]=costInPeriod;
                    //maxStates[p]=newState;
                    
                    if (costInPeriod>=maxCost) {
                        maxCost=costInPeriod;
                        maxState.setState(newState);
                        for (int s=0; s<location; s++) {
                            for (int y=0; y<location; y++) {
                                maxTransshipmentMatrix.setElement(s,y,transshipmentMatrix.getElement(s,y));
                            }
                        }
                    }
                    //delete newState;
                    
                }
                //cout<<"Cost: "<<'\n';
                //cout<<holdingCost+getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix)<<'\n';
                
                state.setState(maxState);
                //delete maxState;
                
                //for (int p=0; p<(int)combinationsOfRowsActionMatrix.size(); p++) {
                  //  if (costInPeriods[p]>=maxCost) {
                    //    maxCost=costInPeriods[p];
                      //  maxP=p;
                    //}
                    
               // }
                
                //for(int i=0; i<location;i++)
                //{
                  //  state->setElement(i, maxStates[maxP]->getElement(i));
                //}
                //std::for_each( maxStates.begin(), maxStates.end(), boost::lambda::delete_ptr() );

                
                for (int i=0; i<location; i++) {
                    for (int j=0; j<location; j++) {
                        if (maxTransshipmentMatrix.getElement(i,j)!=0) {
                            //cout<<"i "<<i<<'\n';
                            //cout<<"j "<<j<<'\n';
                            //cout<<maxTransshipmentMatrix[i][j]<<'\n';
                            transshipmentSize+=maxTransshipmentMatrix.getElement(i,j);
                            count[i][j]++;
                        }
                    }
                }

                
                cout<<"transshipment Matrix"<<'\n';
                for (int a=0; a<location; a++) {
                    for (int b=0; b<location; b++) {
                        cout<<maxTransshipmentMatrix.getElement(a,b)<<" "<<'\n';
                    }
                    cout<<'\n';
                }
                
                cout<<"Post-decision state: "<<'\n';
                for (int i=0; i<location; i++) {
                    cout<<state.getElement(i)<<'\n';
                }
                
                cout<<"Total profit per period: "<<'\n';
                cout<<reward-holdingCost-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix)<<'\n';
                
                
                totalProfit+=reward-holdingCost-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix);  
                //totalProfit+=reward-maxCost+valueFunction.getValueFunction(t+1, hashFunction.hash_value(*state));
                
            }
            else
            {//cout<<"Cost: "<<'\n';
                //cout<<holdingCost<<'\n';
                cout<<"Total profit per last period: "<<'\n';
                cout<<reward-holdingCost<<'\n';
                totalProfit+=reward-holdingCost;}
        }
        
        //delete state;
        profit[n]=totalProfit;
    }
    
    cout<<"Total amount of stock movement: "<<'\n';
    printMatrix(location, location, count);
    //std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );
    
    //return (float)totalProfit/(float)numberOfIterations;
    cout<<"average transshipment size per the whole time period: "<<(float)(transshipmentSize/numberOfIterations)<<'\n';
    return profit;
    
    
}*/

/*row_t evaluationModifiedPolicy(const ValueFunction& valueFunction, Hashing& hashFunction,const int& location, const int& time, const long& numberOfIterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)
{
    
    row_t profit=row_t(numberOfIterations);
    int transshipmentSize=0;
    matrixInt_t count = initializeTransshipmentMatrix(location,location);
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    for (int n=0; n<numberOfIterations; n++) 
    {
        cout<<"---------------------nn-----------------"<<n<<'\n';
        float totalProfit = 0;
        State state(location);
        state.setState(initialState);
        
        for (int t=0; t<time; t++) 
        {
            cout<<"Time: "<<t<<'\n';
            //cout<<"Demand: "<<'\n';
            Demand demand(location);
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)(ceil(distribution[i]->sample())));
                //cout<<demand->getElement(i)<<'\n';
            }
            
            float reward = getReward(cost.getSalesCost(),state,demand); 
            
            state.setState(getNewState(state, demand));
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), state);
            
            //delete demand;
            
            
            if ((t!=time-1)) // for last time interval [T-1,T] there is no sence to make a transshipment
            {  
                
                vecMatrixInt actionsForState=generateActionsZeroOne(state, t,valueFunction, hashFunction, distanceMatrix, cost.getTransshipmentCost());  
                
                for (int j=0; j<location; j++) {
                    cout<<"location "<< j<<'\n';
                    printMatrix(actionsForState[j].size(),location,actionsForState[j]);
                }
                
                Array2DInt<float> combinationsOfRowsActionMatrix=generateAllCombinationsOfTransshipmentActions(actionsForState,location);
                
                //for each possible action calulate cost
                float maxCost=-numeric_limits<float>::max();
                State maxState(location);
                matrixInt_t maxTransshipmentMatrix = initializeTransshipmentMatrix(location,location);
                for (int p=0; p<combinationsOfRowsActionMatrix.size(); p++) {
                    float costInPeriod =0;
                    matrixInt_t transshipmentMatrix = initializeTransshipmentMatrix(location,location);
                    State newState(location);
                    newState.setState(state);
                    for (int j=0; j<=location-1; j++) {
                        int sumTransshipAmountFromJ=0;
                        for (int k=0; k<=location-1; k++) {
                            int amountToTransship=0;
                            if(k!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][k];
                            sumTransshipAmountFromJ+=amountToTransship;
                            newState.setElement(k,newState.getElement(k)+amountToTransship);
                            transshipmentMatrix[j][k]=amountToTransship;
                            
                        }
                        newState.setElement(j,newState.getElement(j)-sumTransshipAmountFromJ);
                        
                    }
                    
                    costInPeriod-=holdingCost;
                    costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                    
                    float costOfValueFunction=valueFunction.getValueFunction(t+1, hashFunction.hash_value(newState),0);

                    costInPeriod+=costOfValueFunction;
                    
                    if (costInPeriod>=maxCost) {
                        maxCost=costInPeriod;
                        maxState.setState(newState);
                        for (int i=0; i<location; i++) {
                            for (int j=0; j<location; j++) {
                                maxTransshipmentMatrix[i][j]=transshipmentMatrix[i][j];
                            }
                        }
                    }
                    //delete newState;
                    
                }
                
                state.setState(maxState);
                //delete maxState;
                
                
                for (int i=0; i<location; i++) {
                    for (int j=0; j<location; j++) {
                        if (maxTransshipmentMatrix[i][j]!=0) {
                            //cout<<"i "<<i<<'\n';
                            //cout<<"j "<<j<<'\n';
                            //cout<<maxTransshipmentMatrix[i][j]<<'\n';
                            transshipmentSize+=maxTransshipmentMatrix[i][j];
                            count[i][j]++;
                        }
                    }
                }
                
                cout<<"transshipment Matrix"<<'\n';
                printMatrix(location, location, maxTransshipmentMatrix);
                totalProfit+=reward-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix)-holdingCost;  
                
            }
            else
                totalProfit+=reward-holdingCost;
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

