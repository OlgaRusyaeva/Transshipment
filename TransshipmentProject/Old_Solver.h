//
//  Launcher of the solver method
//  


#ifndef project2_ADPSolver_h
#define project2_ADPSolver_h

#include "iSolver.h"
#include "PolicyEvaluation.h"
#include <stdio.h>  
#include <stdlib.h>  
#include <math.h> 
#include "OutputGnuplot.h"

//#include </Developer/usr/llvm-gcc-4.2/lib/gcc/i686-apple-darwin11/4.2.1/include/omp.h> // for parallel

class ADPSolver: public iSolver
{
public:
    ADPSolver(){initializeSettings();}
    ~ADPSolver(){}
    
    void solve(iProblem& problem)
    {
        FILE *outputResultsFile;
        const char *fileName; 
        fileName = "/Users/orusyaeva/Desktop/Calculations/Results.txt";  
        outputResultsFile = fopen(fileName,"w"); 
        
        int L = problem.getNumberOfStates();
        int T = problem.getTimeHorizon();
        
        State initialState = problem.getState();
        Cost costInState = problem.getCost();
        Actions distance = problem.getActions();
        
        //for ADP_aggregation
        ValueFunction valueFunction = problem.getValueFunction();
        Hashing hashFunction;
        
        matrixInt_t matrixOfClusters=matrixInt_t(L);
        for (int j=0; j<L; j++) {
            matrixOfClusters[j]=Cluster(L,distance,j+1);
        }
        
        //for ADP_piecewise
        matrixInt_t indexVector(T,L);
        vecMatrixFloat breakpoints = vecMatrixFloat(T,matrix_t(L));
        vecMatrixFloat slope= vecMatrixFloat(T,matrix_t(L));
        
        for (int t=0; t<T; t++) {
            for (int i=0; i<L; i++) {
                indexVector[t][i]=1;
                breakpoints[t][i].push_back(0);
                slope[t][i].push_back(0);
            }
        }

        map<int, pair<float, float> > demandParameters;
        demandParameters = initializeParameters("/Users/orusyaeva/Documents/ADP_Project/ADP/TransshipmentProject/TransshipmentProject/Parameters",L);
        
        //initialState=setOrderUpToLevel(demandParameters,L,T);
        initialState = setOrderUpToLevel("/Users/orusyaeva/Documents/ADP_Project/ADP/TransshipmentProject/TransshipmentProject/InitialState",L);
        
        outputResultGnuplot(initialState, distance,L);
        
        //TODO: file name as input
        outputInitialData(L,T,costInState,distance);
        
        //outputCSV(solveExactDP(L,T,numberOfIterations,initialState, costInState, distance, demandParameters, valueFunction));
        clock_t start,end;

        //TODO: the range as a parameter to outputNumericalResults
        for (int i=11; i<12; i++) {        
            start = clock();
            addToOutput(getStatistics(policyEvaluation(L,T, numberOfIterations,initialState, costInState, distance, demandParameters,valueFunction,hashFunction, indexVector,breakpoints, slope, matrixOfClusters, i)), *fileName, *"Method: ", i);
            end = clock();
            cout<<"Time of method "<<i<<" : "<<double( end - start ) / (double)CLOCKS_PER_SEC<<'\n';
        }
        
        outputNumericalResults(*fileName);
        
        fclose(outputResultsFile);  
        
    }
    
   
    
private:
    
    long numberOfIterations;//number of iterations for simulation


//-----------------main methods---------------------------------------//
    
    void initializeSettings(){numberOfIterations=100;} 
    
};



#endif
