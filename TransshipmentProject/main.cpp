//
//  main.cpp
//
//  Read: data of the problem from input
//  Write: look-up table of Value Function to output
//  Write: average long run profit to console
//  Write: gnuplot
//
// INPUT:
// If input files are not found from the project directory, either to type in whole paths or follow steps
// from this link: http://code-dojo.blogspot.de/2012/02/how-to-read-file-in-xcode-project.html
//
// OUTPUT:
// Be able to see the output do the following: http://stackoverflow.com/questions/14098836/file-creation-in-c-on-xcode
//
//

// To run the executable file, change names of input files. The exe file doens't see txt files in the same directory.


#include "mainHeader.h"
#include "test.h"


using namespace std;

int transshipments();

int main ()
{
    transshipments();
    //Test test;
    //test.checkConcaveUpdate();
    
}

int transshipments()
{
    //--------parameters------
    int numberOfScenarious=15;
    int iterationsSimulation=1000;
    int iterationsLearning=1000;
    int expectationLearning=20;
    //int numberOfClusters=2;
    int seed=2;
    int horizon=28;
    
    //---------input---------
    string input_LocTimeFile, input_PriceFile, input_ActionPeriods, input_HoldingCostFile, input_TransshipCostFile, input_Map, input_LeadTimeLimits, inputOrderingCostFile, input_Distances;
    input_LocTimeFile ="inputLocTime";
    inputOrderingCostFile="inputOrderingCost";
    input_PriceFile="inputPrice";
    input_HoldingCostFile="inputHoldingCost";
    input_TransshipCostFile="inputTransshipmentCost";
    input_Map="inputCoordinates";
    input_LeadTimeLimits="inputLeadTimeDistanceLimits";
    input_Distances="inputDistances";
    
    string input_DemandParameters, input_DistributionParameters;
    input_DemandParameters="inputDemandParameters";
    input_DistributionParameters="inputDistribution";
    
    
    
    string input_InitialIL;
    input_InitialIL="inputInitialState";
    
    Input allInputData(numberOfScenarious,input_LocTimeFile, inputOrderingCostFile, input_PriceFile, input_HoldingCostFile, input_TransshipCostFile, input_Map, input_DemandParameters, input_DistributionParameters, input_InitialIL,input_LeadTimeLimits, input_Distances);
    
    //--------output--------
    string output_InitialData,ouput_ValueFunction,ouput_InitialState;
    output_InitialData="outputInputData";
    ouput_ValueFunction="outputValueFunction";
    ouput_InitialState="outputInitialState";
    
    
    string output_File;
    string output_CSVProfit, output_CSVTransshipmentSizePerIteration,output_CSVFrequency,output_CSVTransshipmentMap,output_CSVExecutionTime,output_CSVLeftAverageInventory, output_CSVTransshipmentSizePerPeriod,output_CSVTransshipmentDays,output_CSVOrderingDecisions,output_CSVStockouts;
    output_File = "outputResults";
    output_CSVProfit="/Users/orusyaeva/Desktop/Calculations/output_CSVProfit.csv";
    output_CSVTransshipmentSizePerIteration="/Users/orusyaeva/Desktop/Calculations/output_CSVTransshipmentSizePerIteration.csv";
    output_CSVTransshipmentSizePerPeriod="/Users/orusyaeva/Desktop/Calculations/output_CSVTransshipmentSizePerPeriod.csv";
    output_CSVFrequency="/Users/orusyaeva/Desktop/Calculations/output_CSVFrequency.csv";
    output_CSVTransshipmentMap="/Users/orusyaeva/Desktop/Calculations/output_CSVTransshipmentMap.csv";
    output_CSVExecutionTime="/Users/orusyaeva/Desktop/Calculations/output_CSVExecutionTime.csv";
    output_CSVLeftAverageInventory="/Users/orusyaeva/Desktop/Calculations/output_CSVLeftAverageInventory.csv";
    output_CSVTransshipmentDays="/Users/orusyaeva/Desktop/Calculations/output_CSVTransshipmentDays.csv";
    output_CSVOrderingDecisions="/Users/orusyaeva/Desktop/Calculations/output_CSVOrderDecisions.csv";
    output_CSVStockouts="/Users/orusyaeva/Desktop/Calculations/output_CSVStockouts.csv";
    
    output::cleanFile(output_File);
    output::cleanFile(output_CSVProfit);
    output::cleanFile(output_CSVTransshipmentSizePerIteration);
    output::cleanFile(output_CSVTransshipmentSizePerPeriod);
    output::cleanFile(output_CSVFrequency);
    output::cleanFile(output_CSVTransshipmentMap);
    output::cleanFile(output_CSVExecutionTime);
    output::cleanFile(output_CSVLeftAverageInventory);
    output::cleanFile(output_CSVTransshipmentDays);
    output::cleanFile(output_CSVOrderingDecisions);
    output::cleanFile(output_CSVStockouts);
    
    string clusterFile;
    clusterFile="";
    
    
    PerformaceTime Time;
    
    
    /*
     for each scenarion:
     intialize problem,
     execute chosen policies,
     output results on screen or in csv files
     */
    
    
    for (int i=1; i<=numberOfScenarious; i++) {
        std::cout<<"Scenario: "<<i<<'\n';
        
        //policy counter
        int n=1;
        
        TransshipmentProblem myProblem(allInputData,i-1);
        
        /*cout<<"No transshipment solver"<<'\n';
        Time.start();
        NoTransshipment_Solver ntp;
        ntp.solve(myProblem);
        Time.finish();*/
        
        //-------------Zero lead time-----------
        
        /*cout<<"DP policy"<<'\n';
         Time.start();
         DPMultipleTransshipments_Solver dp;
         dp.solve(myProblem,ouput_ValueFunction);
         DP_Simulator dpSimulator(iterationsSimulation,seed,dp.getValueFunction(),dp.getHashFunction());
         Profit dpProfit(iterationsSimulation);
         dpSimulator.simulate(myProblem,dpProfit);
         Time.finish();
         
         
         cout<<"Deterministic policy"<<'\n';
         Time.start();
         Deterministic_Solver dtp(seed);
         dtp.solve(myProblem,iterationsSimulation);
         Deterministic_Simulator dtpSimulator(iterationsSimulation,seed,dtp.getActionsMap());
         Profit dtpProfit(iterationsSimulation);
         dtpSimulator.simulate(myProblem,dtpProfit);
         Time.finish();
         
         cout<<"Closest location policy"<<'\n';
         Time.start();
         ClosestLoc_Simulator closSimulator(iterationsSimulation,seed);
         Profit closProfit(iterationsSimulation);
         closSimulator.simulate(myProblem, closProfit);
         Time.finish();*/
         
         /*cout<<"ADP policy"<<'\n';
         Time.start();
         ADPMultipleTransshipments_Solver adp;
         adp.solve(myProblem, iterationsLearning);
         ADP_Simulator adpSimulator(iterationsSimulation,seed,adp.getPiecewiseApp_ValueFunction());
         Profit adpProfit(myProblem,iterationsSimulation);
         adpSimulator.simulate(myProblem, adpProfit);
         Time.finish();*/
        
        /*cout<<"No transshipments policy"<<'\n';
         Time.start();
         NoTransship_Simulator noTransshipSimulator(iterationsSimulation,seed);
         Profit noTransshipProfit(myProblem,iterationsSimulation);
         noTransshipSimulator.simulate(myProblem,noTransshipProfit);
         Time.finish();*/
        
        //----------------Non-zero lead time------------
        
        /*cout<<"DP policy with lead time"<<'\n';
        Time.start();
        DPLeadTimeTransshipment_Solver dpNon(ntp.getUpperBound());
        dpNon.solve(myProblem,ouput_ValueFunction,ouput_InitialState);
        DPLeadTimeTransshipment_Simulator dpNonSimulator(iterationsSimulation,seed,dpNon.getValueFunction(),dpNon.getHashFunction(),dpNon.getInitialState());
        Profit dpNonProfit(myProblem,iterationsSimulation);
        dpNonSimulator.simulate(myProblem,dpNonProfit);
        Time.finish();*/
        
        
        /*cout<<"Deterministic policy with lead time"<<'\n';
         Time.start();
         DeterministicLeadTimeTransshipment_Solver dtpNon(seed);
         dtpNon.solve(myProblem,iterationsSimulation);
         DeterministicLeadTimeTransshipment_Simulator dtpNonSimulator(iterationsSimulation,seed,dtpNon.getTransshipments(),dtpNon.getInitialState());
         Profit dtpNonProfit(myProblem,iterationsSimulation);
         dtpNonSimulator.simulate(myProblem,dtpNonProfit);
         Time.finish();*/
        
        /*cout<<"ADP policy as a single period with lead time"<<'\n';
        Time.start();
        ADPLeadTimeTransshipment_Solver adpNonS(ntp.getUpperBound());
        adpNonS.solveAsSinglePeriod(myProblem, iterationsLearning);
        ADPLeadTimeTransshipmentS_Simulator adpNonSSimulator(iterationsSimulation,seed,adpNonS.getPiecewiseApp_ValueFunction(),adpNonS.getInitialState());
        Profit adpNonSProfit(myProblem,iterationsSimulation);
        adpNonSSimulator.simulate(myProblem, adpNonSProfit);
        Time.finish();*/
        
        /*cout<<"ADP policy as a multi period with lead time"<<'\n';
        Time.start();
        ADPLeadTimeTransshipment_Solver adpNonM(ntp.getUpperBound());
        adpNonM.solveAsMultiPeriod(myProblem, iterationsLearning);
        ADPLeadTimeTransshipmentM_Simulator adpNonMSimulator(iterationsSimulation,seed,adpNonM.getPiecewiseApp_ValueFunction(),adpNonM.getInitialState());
        Profit adpNonMProfit(myProblem,iterationsSimulation);
        adpNonMSimulator.simulate(myProblem, adpNonMProfit);
        Time.finish();
        
        cout<<"No transshipment policy"<<'\n';
        Time.start();
        NoTransshipmentLeadTime_Simulator noTransshipNonSimulator(iterationsSimulation,seed,ntp.getUpperBound());
        Profit noTransshipNonProfit(myProblem,iterationsSimulation);
        noTransshipNonSimulator.simulate(myProblem,noTransshipNonProfit);
        Time.finish();*/
        
        /*cout<<"From closest location policy"<<'\n';
        Time.start();
        ClosLocationLeadTime_Simulator closNonSimulator(iterationsSimulation,seed,ntp.getUpperBound());
        Profit closNonProfit(iterationsSimulation);
        closNonSimulator.simulate(myProblem,closNonProfit);
        Time.finish();*/
        
        /*cout<<"Deterministic rollout policy"<<'\n';
        Time.start();
        //DeterministicRollingHorizon_Solver dRollNon;
        //dRollNon.solve(myProblem,horizon);
        DeterministicRollingHorizon_Simulator dRollSimulator(iterationsSimulation,seed,horizon,ntp.getUpperBound());
        Profit dRollProfit(myProblem,iterationsSimulation);
        dRollSimulator.simulate(myProblem,dRollProfit);
        Time.finish();*/
        
        /*cout<<"Stochastic rollout policy"<<'\n';
        Time.start();
        //StochasticRollingHorizon_Solver sRollNon;
        //sRollNon.solve(myProblem,horizon,expectationLearning);
        StochasticRollingHorizon_Simulator sRollSimulator(iterationsSimulation,seed,horizon, expectationLearning,ntp.getUpperBound());
        Profit sRollProfit(iterationsSimulation);
        sRollSimulator.simulate(myProblem,sRollProfit);
        Time.finish();*/
        
        /*cout<<"ADP runs as simulator"<<'\n';
        Time.start();
        ADPOnline_Simulator adpOnlineSimulator(iterationsSimulation,seed,myProblem,ntp.getUpperBound(),iterationsLearning);
        Profit adpOnlineProfit(myProblem,iterationsSimulation);
        adpOnlineSimulator.simulate(myProblem, adpOnlineProfit);
        Time.finish();*/
        
        //----------------Finite life time------------
        
        cout<<"DP policy"<<'\n';
        Time.start();
        DPMultipleTransshipments_Solver dpLT;
        dpLT.solve(myProblem,ouput_ValueFunction);
        DP_Simulator dpSimulatorLT(iterationsSimulation,seed,dpLT.getValueFunction(),dpLT.getHashFunction());
        Profit dpProfitLT(myProblem,iterationsSimulation);
        dpSimulatorLT.simulate(myProblem,dpProfitLT);
        Time.finish();
        
        cout<<"ADP policy"<<'\n';
         Time.start();
         ADPLifeTimeTransshipment_Solver adpLT;
         adpLT.solve(myProblem, iterationsLearning);
         ADPLifeTimeTransshipment_Simulator adpSimulatorLT(iterationsSimulation,seed,adpLT.getPiecewiseApp_ValueFunction());
         Profit adpProfitLT(myProblem,iterationsSimulation);
         adpSimulatorLT.simulate(myProblem, adpProfitLT);
         Time.finish();
        
        //-------------csv file output-----------
        
        //dtpProfit.outputIntoCSVProfit(output_CSVProfit);
        //dpProfit.outputIntoCSVProfit(output_CSVProfit);
        //adpProfit.outputIntoCSVProfit(output_CSVProfit);
        //closProfit.outputIntoCSVProfit(output_CSVProfit);
        //noTransshipProfit.outputIntoCSVProfit(output_CSVProfit);
        
        

        //dpNonProfit.outputIntoCSVProfit(output_CSVProfit);
        //dtpNonProfit.outputIntoCSVProfit(output_CSVProfit);
        //adpNonSProfit.outputIntoCSVProfit(output_CSVProfit);
       /* adpNonMProfit.outputIntoCSVProfit(output_CSVProfit);
        noTransshipNonProfit.outputIntoCSVProfit(output_CSVProfit);
        //closNonProfit.outputIntoCSVProfit(output_CSVProfit);
        dRollProfit.outputIntoCSVProfit(output_CSVProfit);
        //adpOnlineProfit.outputIntoCSVProfit(output_CSVProfit);
        //sRollProfit.outputIntoCSVProfit(output_CSVProfit);
        
        //dtpNonProfit.outputIntoCSVOrderDecisions(output_CSVOrderingDecisions);
        //dpNonProfit.outputIntoCSVOrderDecisions(output_CSVOrderingDecisions);

        //adpNonSProfit.outputIntoCSVOrderDecisions(output_CSVOrderingDecisions);
        //adpNonMProfit.outputIntoCSVOrderDecisions(output_CSVOrderingDecisions);
        //noTransshipNonProfit.outputIntoCSVOrderDecisions(output_CSVOrderingDecisions);
        //dRollProfit.outputIntoCSVOrderDecisions(output_CSVOrderingDecisions);
        
        //dpNonProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
        adpNonMProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
        noTransshipNonProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
        dRollProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
        
        //dpNonProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
        adpNonMProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
        noTransshipNonProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
        dRollProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);

        
        //dpNonProfit.outputIntoCSVFrequency(output_CSVFrequency);
        adpNonMProfit.outputIntoCSVFrequency(output_CSVFrequency);
        noTransshipNonProfit.outputIntoCSVFrequency(output_CSVFrequency);
        dRollProfit.outputIntoCSVFrequency(output_CSVFrequency);

        
        //dpNonProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
        adpNonMProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
        noTransshipNonProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
        dRollProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);

        
        //dpNonProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        adpNonMProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        noTransshipNonProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        dRollProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory)

        
        //dpNonProfit.outputIntoCSVStockouts(output_CSVStockouts);
        adpNonMProfit.outputIntoCSVStockouts(output_CSVStockouts);
        noTransshipNonProfit.outputIntoCSVStockouts(output_CSVStockouts);
        dRollProfit.outputIntoCSVStockouts(output_CSVStockouts);*/

        
        
        //dpOnePeriodTransshipProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
        /*stProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
         dmdhProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
         noTransshipProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
         bfProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
         uppProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
         adpProfit.outputIntoCSVTransshipmentSizePerIteration(output_CSVTransshipmentSizePerIteration);
         
         //dpOnePeriodTransshipProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
         stProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
         dmdhProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
         noTransshipProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
         bfProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
         uppProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
         adpProfit.outputIntoCSVTransshipmentSizePerPeriod(output_CSVTransshipmentSizePerPeriod);
         
         //dpOnePeriodTransshipProfit.outputIntoCSVFrequency(output_CSVFrequency);
         stProfit.outputIntoCSVFrequency(output_CSVFrequency);
         dmdhProfit.outputIntoCSVFrequency(output_CSVFrequency);
         noTransshipProfit.outputIntoCSVFrequency(output_CSVFrequency);
         bfProfit.outputIntoCSVFrequency(output_CSVFrequency);
         uppProfit.outputIntoCSVFrequency(output_CSVFrequency);
         adpProfit.outputIntoCSVFrequency(output_CSVFrequency);
         
         //dpOnePeriodTransshipProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
         stProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
         dmdhProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
         noTransshipProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
         bfProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
         uppProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
         adpProfit.outputIntoCSVTransshipmentMap(output_CSVTransshipmentMap);
         
         //dpOnePeriodTransshipProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
         stProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
         dmdhProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
         noTransshipProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
         bfProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
         uppProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
         adpProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
         
         
         adpProfit.outputIntoCSVTransshipmentDays(output_CSVTransshipmentDays);*/
        
        //dpOnePeriodTransshipProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        //stProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        //dmdhProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        //noTransshipProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        //bfProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        //uppProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        //adpProfit.outputIntoCSVLeftAverageInventory(output_CSVLeftAverageInventory);
        
        
        Time.outputIntoCSV(output_CSVExecutionTime);
        Time.reset();
        
        
        //---------screen output-------------
        
        /*dpProfit.output(output_File,n);
         noTransshipProfit.output(output_File,n);
         closProfit.output(output_File,n);
         tieProfit.output(output_File,n);
         myopicProfit.output(output_File,n);
         adpProfit.output(output_File,n);
         output::outputNumericalResults(output_File,n);*/
    }
}

