//
//  Profit.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/6/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <fstream>
#include "Profit.h"
#include "iProblem.h"
#include "LeadTimes.h"

//#include <boost/math/distributions.hpp>

using namespace std;


Profit::Profit(iProblem& problem, const int& number)
{
    int totalTime=problem.getTimeHorizon();
    int locationNumber=problem.getStateDimension();
    int leadTimeNumber=problem.getLeadTimes().getMaxLeadTimePlusOne();

    iterationNumber=number;
    profit=row_t(iterationNumber);
    sizePerIteration=vector_t(iterationNumber);
    transshipmentDays=vector_t(iterationNumber);
    frequency=vector_t(totalTime);
    sizePerPeriod=vector_t(totalTime);
    leftovers=matrixInt_t(locationNumber,vector_t(leadTimeNumber));
    transshipmentMatrix=matrixInt_t(locationNumber,vector_t(locationNumber));
    IL=vector_t(locationNumber);
    stockouts=vector_t(iterationNumber);
}


void Profit::setProfit(const int& simulationIter, const float& totalProfit)
{
    profit[simulationIter]=totalProfit;
}

void Profit::outputIntoCSVProfit(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)profit.size(); i++) {
        outputFile<<profit[i]<<',';
    }
    outputFile<<'\n';
    outputFile.close();
}


void Profit::setTransshipmentSizePerIteration(const int& simulationIter, const int& transshipNumber)
{
     sizePerIteration[simulationIter]=transshipNumber;
}

void Profit::outputIntoCSVTransshipmentSizePerIteration(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)sizePerIteration.size(); i++) {
        outputFile<<sizePerIteration[i]<<',';
    }
    outputFile<<'\n';
    outputFile.close();
}


int Profit::getFrequency(const int& period) const
{

    return frequency[period];

}

void Profit::setFrequency(const int& period, const int& transshipNumber)
{

    frequency[period]=transshipNumber;

}

void Profit::outputIntoCSVFrequency(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)frequency.size(); i++) {
        outputFile<<frequency[i]<<',';
    }
    outputFile<<'\n';
    outputFile.close();
}


void Profit::setTransshipmentDays(const int& simulationIter, const int& transshipDays)
{

    transshipmentDays[simulationIter]=transshipDays;

}

void Profit::outputIntoCSVTransshipmentDays(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)transshipmentDays.size(); i++) {
        outputFile<<transshipmentDays[i]<<',';
    }
    outputFile<<'\n';
    outputFile.close();
}


void Profit::setTransshipmentSizePerPeriod(const int& period, const int& transshipSize)
{

    sizePerPeriod[period]=transshipSize;

}

int Profit::getTransshipmentSizePerPeriod(const int& period) const
{
    return sizePerPeriod[period];

}

void Profit::outputIntoCSVTransshipmentSizePerPeriod(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)sizePerPeriod.size(); i++) {
        outputFile<<sizePerPeriod[i]<<',';
    }
    outputFile<<'\n';
    outputFile.close();
}



void Profit::setTransshipmentMatrix(const matrixInt_t& map)
{
    for (int i=0; i<(int)map.size(); i++) {
        for (int j=0; j<(int)map[i].size(); j++) {
            transshipmentMatrix[i][j]=map[i][j];
        }
    }
}

void Profit::outputIntoCSVTransshipmentMap(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)transshipmentMatrix.size(); i++) {
        for (int j=0; j<(int)transshipmentMatrix.size(); j++) {
            outputFile<<transshipmentMatrix[i][j]<<',';
        }
        outputFile<<'\n';
    }
    outputFile.close();
}


void Profit::setLeftInventory(const int& inventory, const int& locationID, const int& timeID)
{
    leftovers[locationID][timeID]=inventory;
}

int Profit::getLeftInvetory(const int& locationID, const int& timeID) const
{
    return leftovers[locationID][timeID];
}

void Profit::outputIntoCSVLeftAverageInventory(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)leftovers.size(); i++) {
        for (int l=0; l<(int)leftovers[i].size(); l++) {
            outputFile<<leftovers[i][l]/iterationNumber<<',';
        }
    }
    outputFile<<'\n';
    outputFile.close();
}

void Profit::setOrderDecision(const int& order, const int& locationID)
{
    IL[locationID]=order;
}

int Profit::getOrderDecision(const int& locationID) const
{
    return IL[locationID];
}

void Profit::outputIntoCSVOrderDecisions(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)IL.size(); i++) {
        outputFile<<IL[i]<<',';
    }
    outputFile<<'\n';
    outputFile.close();
}

void Profit::setStockout(const int& simulationIter,const int& numberOfStockoutLocations)
{
    stockouts[simulationIter]=numberOfStockoutLocations;
}

int Profit::getStockout(const int& simulationIter) const
{
    return stockouts[simulationIter];
}

void Profit::outputIntoCSVStockouts(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)stockouts.size(); i++) {
        outputFile<<stockouts[i]<<',';
    }
    outputFile<<'\n';
    outputFile.close();
}

float Profit::getAverageProfit()
{
    float sum=0;
    for (int i=0; i<iterationNumber; i++)
    {
        sum+=profit[i];
    }
    return (float)sum/iterationNumber;

}

row_t Profit::getProfit()const
{
    return profit;
}

///////////////////////////////////////////////////////////////////////


    
/*const Profit& Profit::operator=(const Profit& newProfit)
{    
	for (int i=0; i<newProfit.iterationNumber; i++) {
        this->profit[i]=newProfit.profit[i];
        this->size[i]=newProfit.size[i];
        this->frequency[i]=newProfit.frequency[i];
        this->leftovers[i]=newProfit.leftovers[i];
    }
    this->iterationNumber=newProfit.iterationNumber;
    //this->mean=newProfit.mean;
    //this->variance=newProfit.variance;
    
    return *this;
}


//min value-0,25 quantile-average-0,75 quantile-max value
void Profit::output(const string& fileName, int& number)
{
    sort(profit.begin(),profit.end());
    
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    outputFile<<number<<" ";
    outputFile<<this->getMin()<<" ";
    outputFile<<this->getQ_25()<<" ";
    outputFile<<this->getMean()<<" ";
    outputFile<<this->getQ_75()<<" ";
    outputFile<<this->getMax()<<" ";
    outputFile<<this->getMax()<<" ";
    outputFile<<this->getVariance()<<" ";
    outputFile<<this->getCIMean(0.05)<<" ";
    outputFile<<this->getReplicationNumber(0.05,0.1)<<'\n';
    outputFile.close();
    number++;
    
}



//sample mean
float Profit::getMean()
{
    float sum=0;
    for (int i=0; i<iterationNumber; i++) 
    {
        sum+=profit[i];
    }
    mean=(float)sum/iterationNumber;
    return mean;
    
}

//sample variance
float Profit::getVariance()
{
    float squareSum=0;
    for (int i=0; i<iterationNumber; i++) 
    {
        squareSum+=pow(profit[i]-mean, 2);
    }
    variance=(float)squareSum/(iterationNumber-1);
    return variance;
}

float Profit::getQ_25()
{
    return profit[(ceil)((iterationNumber+1)*0.25)];
}

float Profit::getQ_75()
{
    return profit[(floor)((iterationNumber+1)*0.75)];
}

float Profit::getMin()
{
    return profit[0];
}

float Profit::getMax()
{
    return profit[iterationNumber-1];
}
*/
/*void Profit::setData(const row_t& profitVector)
{
    for (int i=0; i<(int)profitVector.size(); i++) {
        data[i]=profitVector[i];
    }
    
    sort(data.begin(),data.end());
}*/



/*float Profit::getCIMean(const float& prob)
{
    //double alpha[] = { 0.5, 0.25, 0.1, 0.05, 0.01, 0.001, 0.0001, 0.00001 };
    boost::math::students_t dist(iterationNumber - 1);
    double T = quantile(complement(dist, prob / 2));
    double w = T * sqrt(variance) / sqrt(double(iterationNumber));
    
    return w;
}

int Profit::getReplicationNumber(const float& prob,const float& relativeError)
{
    
    int i=iterationNumber;
    bool flag=true;
    float error=relativeError/(1+relativeError);
	while (flag) {
        boost::math::students_t dist(i - 1);
        double T = quantile(complement(dist, prob / 2));
        double w = T * sqrt(variance) / (sqrt(double(i))*abs(double(mean)));
        if (w<=error) {
            flag=false;
        }
        else
            i++;
    }
    return i;
}*/

