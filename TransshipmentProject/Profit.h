//
//  Profit.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/6/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Profit_h
#define TransshipmentProject_Profit_h

#include "types.h"
class iProblem;

class Profit{
    
public:
    Profit(iProblem& problem, const int& number);
    ~Profit(){}
    
    void setProfit(const int& simulationIter, const float& totalProfit);
    
    void outputIntoCSVProfit(const std::string& fileName);
    
    
    void setTransshipmentSizePerIteration(const int& simulationIter, const int& transshipNumber);
    
    void outputIntoCSVTransshipmentSizePerIteration(const std::string& fileName);
    
    
    int getFrequency(const int& period) const;
    
    void setFrequency(const int& period, const int& transshipNumber);
    
    void outputIntoCSVFrequency(const std::string& fileName);
    
    
    void setTransshipmentSizePerPeriod(const int& period, const int& transshipSize);
    
    int getTransshipmentSizePerPeriod(const int& period) const;
    
    void outputIntoCSVTransshipmentSizePerPeriod(const std::string& fileName);
    
    
    void setTransshipmentMatrix(const matrixInt_t& map);
    
    void outputIntoCSVTransshipmentMap(const std::string& fileName);
    
    
    void setLeftInventory(const int& inventory, const int& locationID, const int& timeID);
    
    int getLeftInvetory(const int& locationID, const int& timeID) const;
    
    void outputIntoCSVLeftAverageInventory(const std::string& fileName);
    
    
    void setOrderDecision(const int& order, const int& locationID);
    
    int getOrderDecision(const int& locationID) const;
    
    void outputIntoCSVOrderDecisions(const std::string& fileName);
    
    
    float getAverageProfit();
    
    row_t getProfit()const;
    
    
    
    void setTransshipmentDays(const int& simulationIter, const int& transshipDays);
    
    void outputIntoCSVTransshipmentDays(const std::string& fileName);
    
    
    void setStockout(const int& simulationIter,const int& numberOfStockoutLocations);
    
    int getStockout(const int& simulationIter) const;
    
    void outputIntoCSVStockouts(const std::string& fileName);
    
    
    //---------------change---------------------------
    
    /*float getMean();
    
    float getVariance();
    
    float getQ_25();
    
    float getQ_75();
    
    float getMin();
    
    float getMax();
    
    float getCIMean(const float& prob);
    
    int getReplicationNumber(const float& prob,const float& relativeError);
    
    const Profit& operator=(const Profit& newProfit);
    
    void output(const std::string& fileName, int& number);*/
    
    
    
private:
    row_t profit;
    vector_t sizePerIteration;
    vector_t sizePerPeriod;
    vector_t frequency;
    matrixInt_t transshipmentMatrix;
    int iterationNumber;
    matrixInt_t leftovers;
    vector_t transshipmentDays;
    vector_t IL;
    vector_t stockouts;
    
    
    //float mean;
    //float variance;

    
};


#endif
