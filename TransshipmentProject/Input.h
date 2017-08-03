//
//  Input.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 6/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Input_h
#define TransshipmentProject_Input_h


#include "types.h"


class Input{
    
public:
    Input(const int numberScenarious, const std::string& f_fileNameData, const std::string& f_fileNameOrderCost,  const std::string& f_fileNamePrice,const std::string& f_fileNameHoldingCost,const std::string& f_fileNameTransshipmentCost,const std::string& f_fileNameMap,const std::string& f_fileNameDemand,const std::string& f_fileNameDistribution,const std::string& f_fileNameIL, const std::string& f_fileNameLeadTimeLimits, const std::string& f_fileNameDistances);
    
    int getNumberLocations(const int& scenarioNumber)const;
    int getNumberTimePeriods(const int& scenarioNumber)const;
    int getMaxLifeTime(const int& scenarioNumber) const;
	vector_t getInitialIL(const int& scenarioNumber)const;
    row_t getDemandParameter1(const int& scenarioNumber)const;
    row_t getDemandParameter2(const int& scenarioNumber)const;
    row_t getOrderingCost(const int& scenarioNumber)const;
    row_t getPrice(const int& scenarioNumber)const;
    row_t getHoldingCost(const int& scenarioNumber)const;
    row_t getTransshipmentCost(const int& scenarioNumber)const;
    matrix_t getDistance(const int& scenarioNumber)const;
    row_t getCoordinateX(const int& scenarioNumber)const;
    row_t getCoordinateY(const int& scenarioNumber)const;
    vector_t getDistribution(const int& scenarioNumber)const;
    matrixInt_t getLeadTimes(const int& scenarioNumber)const;
    
    
    ~Input(){}
    
    
private:
    int nS;//number of scenarious
    
    vector_t numberLocations;
    vector_t numberTimePeriods;
    vector_t maxLifeTime;
    
    matrix_t leadTimeDistanceLimits;
    
    matrix_t od;
    matrix_t p;
    matrix_t h;
    matrix_t c;
    
    matrix_t coordinateX;
    matrix_t coordinateY;
    matrix_t distances;
    
    matrixInt_t IL;
    
    matrixInt_t distrib;//can take 1 - Uniform, 2 - Poisson, 3 - NegBinomial, 4 - Binom, 5,6,7 - Discrete with given probabilities
    
    matrix_t distribParam1;
    matrix_t distribParam2;
    
    void readMainData(const std::string& fileName);
    
    template < typename T >
    void readParameter(const std::string& fileName, T& parameterToIntialize);
    
    template < typename T >
    void readParameter(const std::string& fileName, T& parameterToIntialize,const vector_t& parameterToCheck);//Price, HoldingCost, TransshipmentCost, IL if the file is not empty, otherwise calculate
    
    template < typename T >
    void readParameters(const std::string& fileName, T& parameterToIntialize1, T& parameterToIntialize2,const vector_t& parameterToCheck);// DistributionParameters(Uniform, Poisson, NegBinom, Binom)]
    
    //void readTransshipmentPeriods(const std::string& f_fileNameActionPeriods);
    
    void readIL(const std::string& f_fileNameIL);
    
    void readDemand(const std::string& f_fileNameDemand,const std::string& f_fileNameDistribution);
    
    void readMap(const std::string& f_fileNameMap,const std::string& f_fileNameDistances);

    
    void readLeadTime(const std::string& f_fileNameLeadTimeLimits, const std::string& f_fileNameLeadTime);
    
    void calculateIL();
    
    void generateMapEqual();
    
    void generateMapNotEqual();
    
    void generateCoordinates(gen_uniform_int_AB& gen, const int& scenarioNumber);
    
    float getMean(const int& scenarioNumber, const int& locationNumber) const;
    
    float getVariance(const int& scenarioNumber, const int& locationNumber) const;
    
    float getMeanUnifrom(const int& scenarioNumber, const int& locationNumber) const;
    
    float getVarianceUniform(const int& scenarioNumber, const int& locationNumber) const;
    
    float getMeanPoisson(const int& scenarioNumber, const int& locationNumber) const;
    
    float getVariancePoisson(const int& scenarioNumber, const int& locationNumber) const;
    
    float getMeanNegBinom(const int& scenarioNumber, const int& locationNumber) const;
    
    float getVarianceNegBinom(const int& scenarioNumber, const int& locationNumber) const;
    
    float getMeanBinom(const int& scenarioNumber, const int& locationNumber) const;
    
    float getVarianceBinom(const int& scenarioNumber, const int& locationNumber) const;
    
    bool checkIfEmpty(const std::string& f_fileNameDemand) const;
    
    bool checkIfEqual(const vector_t& numberLocations) const;
    
    template < typename T >
    void set(const std::string& line, T& parameterToIntialize);
    
    template < typename T >
    void set(const std::string& line, T& parameterToIntialize,const int& limit);
    
    template < typename T >
    void set(const std::string& line, T& parameterToIntialize1,T& parameterToIntialize2, const int& limit);
    
    void initializeOrderingCost();
    void initializePrice();
    void initializeHoldingCost();
    void initializeTransshipmentCost();
    void initializeDemandParameters();
    void initializeILvector();
    void initializeCoordinates();
    void initializeLeadTimeDistanceLimits();
    void intializeDistances();


    
};

class InputException : public std::runtime_error
{
public:
    InputException(const std::string &msg) : std::runtime_error(msg){}
};


#endif
