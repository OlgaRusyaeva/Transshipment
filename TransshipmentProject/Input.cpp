//
//  Input.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 6/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "Input.h"
#include <fstream>
#include <math.h>   
#include <sstream>
#include <string>

using namespace std;

Input::Input(const int numberScenarious, const std::string& f_fileNameData, const std::string& f_fileNameOrderCost, const std::string& f_fileNamePrice,const std::string& f_fileNameHoldingCost,const std::string& f_fileNameTransshipmentCost,const std::string& f_fileNameMap,const std::string& f_fileNameDemand,const std::string& f_fileNameDistribution,const std::string& f_fileNameIL, const std::string& f_fileNameLeadTimeLimits, const std::string& f_fileNameDistances)
{
    
    nS=numberScenarious;
    
    readMainData(f_fileNameData);
    
    initializeOrderingCost();
    readParameter(f_fileNameOrderCost,od,numberLocations);
    
    initializePrice();
    readParameter(f_fileNamePrice,p,numberLocations);
    
    initializeHoldingCost();
    readParameter(f_fileNameHoldingCost,h,numberLocations);
    
    initializeTransshipmentCost();
    readParameter(f_fileNameTransshipmentCost,c,numberLocations);
    
    initializeDemandParameters();
    readDemand(f_fileNameDemand,f_fileNameDistribution);
    
    initializeILvector();
    readIL(f_fileNameIL);
    
    intializeDistances();
    initializeCoordinates();
    readMap(f_fileNameMap,f_fileNameDistances);
    
    initializeLeadTimeDistanceLimits();
    readParameter(f_fileNameLeadTimeLimits,leadTimeDistanceLimits);

    
}

int Input::getNumberLocations(const int& scenarioNumber)const
{
    return numberLocations[scenarioNumber];
}
int Input::getNumberTimePeriods(const int& scenarioNumber)const
{
    return numberTimePeriods[scenarioNumber];
}
int Input::getMaxLifeTime(const int& scenarioNumber) const
{
    return maxLifeTime[scenarioNumber];
}
vector_t Input::getInitialIL(const int& scenarioNumber)const
{
    return IL[scenarioNumber];
}
row_t Input::getDemandParameter1(const int& scenarioNumber)const
{
    return distribParam1[scenarioNumber];
}
row_t Input::getDemandParameter2(const int& scenarioNumber)const
{
    return distribParam2[scenarioNumber];
}
row_t Input::getOrderingCost(const int& scenarioNumber)const
{
    return od[scenarioNumber];
}
row_t Input::getPrice(const int& scenarioNumber)const
{
    return p[scenarioNumber];
}
row_t Input::getHoldingCost(const int& scenarioNumber)const
{
    return h[scenarioNumber];
}
row_t Input::getTransshipmentCost(const int& scenarioNumber)const
{
    return c[scenarioNumber];
}
row_t Input::getCoordinateX(const int& scenarioNumber)const
{
    return coordinateX[scenarioNumber];
}
row_t Input::getCoordinateY(const int& scenarioNumber)const
{
    return coordinateY[scenarioNumber];
}
matrix_t Input::getDistance(const int& scenarioNumber)const
{
    matrix_t distanceForScenario(numberLocations[scenarioNumber],row_t(numberLocations[scenarioNumber]));
    int locationNumber=numberLocations[scenarioNumber];
    for (int i=0; i<locationNumber; i++) {
    	for (int j=0; j<locationNumber; j++) {
            if (i!=j) {
                if (!distances[scenarioNumber].empty()) {
                    distanceForScenario[i][j]=distances[scenarioNumber][i*locationNumber+j];
                }
                else
                	distanceForScenario[i][j]=sqrtf(pow(((float)(coordinateX[scenarioNumber][i]-coordinateX[scenarioNumber][j])), 2)+pow(((float)(coordinateY[scenarioNumber][i]-coordinateY[scenarioNumber][j])), 2));
            }
        }
    }

    return distanceForScenario;
}
vector_t  Input::getDistribution(const int& scenarioNumber)const
{
    return distrib[scenarioNumber];
}

void Input::readDemand(const std::string& f_fileNameDemand,const std::string& f_fileNameDistribution)
{
    if ((checkIfEmpty(f_fileNameDemand))&&(checkIfEmpty(f_fileNameDistribution))) {
        	throw InputException("Error: All demand files are empty!"); 
    	}
    else
        {
            readParameters(f_fileNameDemand,distribParam1,distribParam2,numberLocations);
            readParameter(f_fileNameDistribution,distrib,numberLocations);

        }    
}


void Input::readIL(const std::string& f_fileNameIL)
{
    if (checkIfEmpty(f_fileNameIL)) {
        calculateIL();
    }
    else
        readParameter(f_fileNameIL, IL,numberLocations);
}

bool Input::checkIfEmpty(const std::string& f_fileNameDemand)const
{
    ifstream inputFile(f_fileNameDemand.c_str());
    streampos current = inputFile.tellg();
    inputFile.seekg (0, inputFile.end);
    bool empty = !inputFile.tellg(); // true if empty file
    inputFile.seekg (current, inputFile.beg); 
    inputFile.close();
    return empty;
}

void Input::initializeOrderingCost()
{
    od=matrix_t(nS);
}

void Input::initializePrice()
{
    p=matrix_t(nS);
}

void Input::initializeHoldingCost()
{
	h=matrix_t(nS);
}

void Input::initializeTransshipmentCost()
{
	c=matrix_t(nS);
}

void Input::initializeDemandParameters()
{
    distrib=matrixInt_t(nS);
	distribParam1=matrix_t(nS);
    distribParam2=matrix_t(nS);
}


void Input::initializeILvector()
{
	IL=matrixInt_t(nS);
}

void Input::intializeDistances()
{
    distances=matrix_t(nS);
}

void Input::initializeCoordinates()
{
    coordinateX=matrix_t(nS);
    coordinateY=matrix_t(nS);
}


void Input::readMainData(const std::string& fileName)
{
    ifstream inputFile(fileName.c_str());
    string line;
    if (inputFile == NULL) throw InputException("Error opening file"); 
    else
    {
        getline(inputFile, line);
        set(line,numberLocations,nS);
        getline(inputFile, line);
        set(line,numberTimePeriods,nS);
        getline(inputFile, line);
        set(line,maxLifeTime,nS);
    }
    inputFile.close();
}

template < typename T >
void Input::set(const string& line, T& parameterToIntialize)
{
    istringstream iss(line);
    float value;
    while (iss >> value)
    {
        parameterToIntialize.push_back(value);
    }
    
    
}

template < typename T >
void Input::set(const string& line, T& parameterToIntialize, const int& limit)
{
    istringstream iss(line);
    float value;
    int counter=0;
    while (iss >> value)
    {
        parameterToIntialize.push_back(value);
        counter++;
    }
    if (counter!=limit)
    {
        if (counter<limit) {
            for (int i=0; i<limit-counter; i++) {
                parameterToIntialize.push_back(parameterToIntialize[counter-1]);
            }
        }
        else
            throw InputException("Error in the number of columns in some input files");
    }

    
}

template < typename T >
void Input::set(const string& line, T& parameterToIntialize1,T& parameterToIntialize2, const int& limit)
{
    istringstream iss(line);
    float value;
    int counter=0;
    while (iss >> value)
    {
        if (counter%2==0) {
        	parameterToIntialize1.push_back(value);
        }
        else
           	parameterToIntialize2.push_back(value);
        counter++;
    }
    if (counter!=limit)
    {
        if (counter<limit) {
            for (int i=0; i<limit-counter; i++) {
                parameterToIntialize1.push_back(parameterToIntialize1[(int)(counter/2)-1]);
                parameterToIntialize2.push_back(parameterToIntialize2[(int)(counter/2)-1]);
            }
        }
        else
            throw InputException("Error in the number of columns in Demand parameter or Coordinates file");
    }
    
    
}

template < typename T >
void Input::readParameter(const std::string& fileName, T& parameterToIntialize)
{
    ifstream inputFile(fileName.c_str());
    string line;
    if (inputFile == NULL) throw InputException("Error opening file");
    else
    {
        int scenarioCounter=0;
        while(std::getline(inputFile, line))
        {
            set(line,parameterToIntialize[scenarioCounter]);
            scenarioCounter++;
            
        }
        if (scenarioCounter!=nS) {
            throw InputException("Error: Number of rows differs from the input number of scenarious ");
        }
        
    }
    inputFile.close();
    
}

template < typename T >
void Input::readParameter(const std::string& fileName, T& parameterToIntialize, const vector_t& parameterToCheck)
{
    ifstream inputFile(fileName.c_str());
    string line;
    if (inputFile == NULL) throw InputException("Error opening file"); 
    else
    {
		int scenarioCounter=0;
        while(std::getline(inputFile, line))
        {
			set(line,parameterToIntialize[scenarioCounter],parameterToCheck[scenarioCounter]);
            scenarioCounter++;
            
        }
        if (scenarioCounter!=nS) {
            throw InputException("Error: Number of rows in Price, Cost or IL file exceeds the input number of scenarious "); 
        }

    }
    inputFile.close();
    
}

template < typename T >
void Input::readParameters(const std::string& fileName, T& parameterToIntialize1, T& parameterToIntialize2,const vector_t& parameterToCheck)
{
    ifstream inputFile(fileName.c_str());
    string line;
    if (inputFile == NULL) throw InputException("Error opening file"); 
    else
    {
		int scenarioCounter=0;
        while(std::getline(inputFile, line))
        {
			set(line,parameterToIntialize1[scenarioCounter],parameterToIntialize2[scenarioCounter],2*parameterToCheck[scenarioCounter]);
            scenarioCounter++;
            
        }
        if (scenarioCounter!=nS) {
            throw InputException("Error: Number of rows in Demand parameters or Coordinates file exceeds the input number of scenarious"); 
        }

    }
    inputFile.close();
}

void Input::calculateIL()
{
    for (int i=0; i<nS; i++) {
        for (int j=0; j<numberLocations[i]; j++) {
            IL[i].push_back(floor(numberTimePeriods[i]*getMean(i,j)+sqrt(getVariance(i,j)*numberTimePeriods[i])));
        }
    }
}

float Input::getMean(const int& scenarioNumber, const int& locationNumber) const
{
    float mean=0;
    try {
        switch (distrib[scenarioNumber][locationNumber]) {
            case 1: mean=getMeanUnifrom(scenarioNumber,locationNumber); break;
            case 2: mean=getMeanPoisson(scenarioNumber,locationNumber); break;
            case 3: mean=getMeanNegBinom(scenarioNumber,locationNumber); break;
            case 4: mean=getMeanBinom(scenarioNumber,locationNumber); break;
            case 5: mean=2;break;
            case 6: mean=2;break;
            case 7: mean=2;break;
        }
    } catch (...) {
        throw InputException("Error: Impossible to define the distribution"); 
    }
    return mean;
}


float Input::getVariance(const int& scenarioNumber, const int& locationNumber) const
{
    float variance=0;
    try {
        switch (distrib[scenarioNumber][locationNumber]) {
            case 1: variance=getVarianceUniform(scenarioNumber,locationNumber); break;
            case 2: variance=getVariancePoisson(scenarioNumber,locationNumber); break;
            case 3: variance=getVarianceNegBinom(scenarioNumber,locationNumber); break;
            case 4: variance=getVarianceBinom(scenarioNumber,locationNumber); break;
            case 5: variance=4;break;
            case 6: variance=1.2;break;
            case 7: variance=2;break;
        }
    } catch (...) {
        throw InputException("Impossible to define the distribution"); 
    }
    return variance;

}

float Input::getMeanUnifrom(const int& scenarioNumber, const int& locationNumber) const
{
    float x = (distribParam1[scenarioNumber][locationNumber]+distribParam2[scenarioNumber][locationNumber])/2;
    return x;
}

float Input::getVarianceUniform(const int& scenarioNumber, const int& locationNumber) const
{
    return ((distribParam2[scenarioNumber][locationNumber]-distribParam1[scenarioNumber][locationNumber]+1)*(distribParam2[scenarioNumber][locationNumber]-distribParam1[scenarioNumber][locationNumber]+1)-1)/12;
}

float Input::getMeanPoisson(const int& scenarioNumber, const int& locationNumber) const
{
	return distribParam1[scenarioNumber][locationNumber];
}

float Input::getVariancePoisson(const int& scenarioNumber, const int& locationNumber) const
{
    if (distribParam1[scenarioNumber][locationNumber]!=distribParam2[scenarioNumber][locationNumber]) {
        throw InputException("Error: Different mean and variance for Poisson distribution");
    }
	return	distribParam2[scenarioNumber][locationNumber];
}

float Input::getMeanNegBinom(const int& scenarioNumber, const int& locationNumber) const
{
 	return (float)(distribParam1[scenarioNumber][locationNumber]*(1-distribParam2[scenarioNumber][locationNumber]))/distribParam2[scenarioNumber][locationNumber];
}

float Input::getVarianceNegBinom(const int& scenarioNumber, const int& locationNumber) const
{
	return (float)(distribParam1[scenarioNumber][locationNumber]*(1-distribParam2[scenarioNumber][locationNumber]))/(distribParam2[scenarioNumber][locationNumber]*distribParam2[scenarioNumber][locationNumber]);
}

float Input::getMeanBinom(const int& scenarioNumber, const int& locationNumber) const
{
 	return (float)(distribParam1[scenarioNumber][locationNumber]*distribParam2[scenarioNumber][locationNumber]);
}

float Input::getVarianceBinom(const int& scenarioNumber, const int& locationNumber) const
{
	return (float)(distribParam1[scenarioNumber][locationNumber]*(1-distribParam2[scenarioNumber][locationNumber])*distribParam2[scenarioNumber][locationNumber]);
}


//generate different map for all scenarious
void Input::generateMapNotEqual()
{
    //srand(time(NULL));
    base_gen_type generator(156);//can be any number in this case also time(NULL)
    gen_uniform_int_AB unif_gen_0100(generator, uniform_int_AB(0,100));
    for (int i=0; i<nS; i++) {
        generateCoordinates(unif_gen_0100,i);
        std::cout<<"X:"<<'\n';
        for (int j=0; j<numberLocations[i]; j++) {
            std::cout<<coordinateX[i][j]<<",";
        }
        std::cout<<'\n';
        std::cout<<"Y:"<<'\n';
        for (int j=0; j<numberLocations[i]; j++) {
            std::cout<<coordinateY[i][j]<<",";
        }
        std::cout<<'\n';
    }
}

//generate the same map for all scenarious
void Input::generateMapEqual()
{
    //srand(time(NULL));
    base_gen_type generator(156);//fix the number to reapeat the same random numb every run
    gen_uniform_int_AB unif_gen_0100(generator, uniform_int_AB(0,100));
    generateCoordinates(unif_gen_0100,0); 
    
    for (int i=1; i<nS; i++) {
        for (int j=0; j<=numberLocations[i]-1; j++) {
            coordinateX[i].push_back(coordinateX[i-1][j]);
            coordinateY[i].push_back(coordinateY[i-1][j]);
        }
    }
    
}



void Input::generateCoordinates(gen_uniform_int_AB& gen, const int& scenarioNumber)
{
    for (int i=0; i<=numberLocations[scenarioNumber]-1; i++) {
        coordinateX[scenarioNumber].push_back(gen());
        coordinateY[scenarioNumber].push_back(gen());
    }
    for (int i=0; i<=numberLocations[scenarioNumber]-1; i++) {
        if ((coordinateX[scenarioNumber][i]==0)&&(coordinateY[scenarioNumber][i]==0)){
            generateCoordinates(gen,scenarioNumber);
        }
        for (int j=0; j<=numberLocations[scenarioNumber]-1; j++) {
            if ((i!=j)&&(coordinateX[scenarioNumber][i]==coordinateX[scenarioNumber][j])&&(coordinateY[scenarioNumber][i]==coordinateY[scenarioNumber][j])) {
                generateCoordinates(gen,scenarioNumber);
                
            }
        }
    }
}
void Input::readMap(const std::string& f_fileNameMap, const std::string& f_fileNameDistances)
{
    if (checkIfEmpty(f_fileNameDistances)) {
        if (checkIfEmpty(f_fileNameMap)) {
            //std::cout<<"Warning: All experiments are run for the same map of locations. Check settings if it is not what you want."<<'\n';
            //generateMapEqual();
            std::cout<<"Warning: All experiments are run for the different map of locations. Check settings if it is not what you want."<<'\n';
            generateMapNotEqual();

            /*if (checkIfEqual(numberLocations)) {
             generateMapEqual();
             }
             else
             {
             std::cout<<"Warning: All experiments are run for the different map of locations. Check settings if it is not what you want."<<'\n';
             generateMapNotEqual();
             }*/
            
        }
        else
            readParameters(f_fileNameMap,coordinateX,coordinateY,numberLocations);
    }
    else
    {
        vector_t limits(nS);
        for (int i=0; i<nS; i++) {
            limits[i]=numberLocations[i]*numberLocations[i];
            for (int j=0; j<numberLocations[i]; j++) {
                coordinateX[i].push_back(0);
                coordinateY[i].push_back(0);
            }
        }
        readParameter(f_fileNameDistances,distances,limits);

    }

}


bool Input::checkIfEqual(const vector_t& numberLocations)const
{
    bool flag=true;
	for (int i=1; i<nS; i++) {
        if (numberLocations[i]!=numberLocations[i-1]) {
            flag=false;
        }
    }
    return flag;
}

void Input::initializeLeadTimeDistanceLimits()
{
    leadTimeDistanceLimits=matrix_t(nS);
}

matrixInt_t Input::getLeadTimes(const int& scenarioNumber)const
{
    matrix_t distanceForScenario=getDistance(scenarioNumber);
    int numberOfLocations=numberLocations[scenarioNumber];
    matrixInt_t leadTimesPerScenario(numberOfLocations,vector_t(numberOfLocations));
    for (int i=0; i<numberOfLocations; i++) {
        for (int j=0; j<numberOfLocations; j++) {
            if (i!=j) {
                leadTimesPerScenario[i][j]=std::upper_bound(leadTimeDistanceLimits[scenarioNumber].begin(),leadTimeDistanceLimits[scenarioNumber].end(),distanceForScenario[i][j])-leadTimeDistanceLimits[scenarioNumber].begin();
            }
        }
    }
    
    return leadTimesPerScenario;

}
