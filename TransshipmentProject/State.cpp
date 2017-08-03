//
//  State.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "State.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

State::State(int length,int depth, int width):sizeL(length),sizeD(depth),sizeW(width)
{
    state=matrixInt_t(length,vector_t(width));
}

State::State(int length,int depth):sizeL(length),sizeD(depth),sizeW(1)
{
    state=matrixInt_t(length,vector_t(depth));
}

State::State(int length):sizeL(length),sizeD(1),sizeW(1)
{
    state=matrixInt_t(length,vector_t(1));
}

/*void State::setInitialState(const int& id_l, const int& id_d, const int& value)
{        
    state[id_l][id_d]=value; // constant inventory level

}*/
    
void State::setElement(const int& id_l,const int& id_d, const int& value)
{
    state[id_l][id_d]=value;
}
    
const State& State::operator=(const State& newState)
{
    int b=0;
    if (sizeW>1) {
        b=sizeW;
    }
    else
        b=sizeD;
    
    for (int i=0; i<sizeL; i++) {
        for (int t=0; t<b; t++) {
            this->setElement(i,t,0);
        }
    }
    
    for (int i=0; i<newState.sizeL; i++) {
        for (int t=0; t<b; t++) {
            this->setElement(i,t,newState.getElement(i,t));
        }
    }
        return *this;
}
    
matrixInt_t State::getState() const
{
    return state;
}
    
int State::getElement(const int& id_l, const int& id_d)const
{
    return state[id_l][id_d];
}
    
int State::getLength() const
{
    return sizeL;
}

int State::getWidth() const
{
    return sizeW;
}

vector_t State::getCurrentPeriodState() const
{
    vector_t stateForCurrentTime(sizeL);
    for (int i=0; i<sizeL; i++) {
        stateForCurrentTime[i]=state[i][0];
    }
    return stateForCurrentTime;
}

int State::getDepth() const
{
   return sizeD;
}

void State::print() const
{
    int b=0;
    if (sizeW>1) {
        b=sizeW;
    }
    else
        b=sizeD;
    
    for (int i=0; i<sizeL; i++) {
        for (int t=0; t<b; t++) {
            std::cout<<getElement(i,t)<<" ";
        }
        std::cout<<'\n';
    }
}

void State::inputState(ifstream& inputFile)
{
    string line;
    if (inputFile == NULL) perror ("Error opening file.");
    else
    {
        int count=0;
        while(std::getline(inputFile, line))
        {
            if (count>=sizeL) perror ("Error in optimal IL file.");

            if (line=="") {
                continue;
            }
            int value;
            
            std::stringstream ss(line);
            
            ss>>value;
            setElement(count,0,value);
            
            count++;
            
        }
        
    }
    
}

void State::outputState(ofstream& outputFile) const
{
    for (int i=0; i<sizeL; i++) {
        outputFile<<getElement(i,0)<<'\n';
    }
    outputFile<<"--------------------------new line---------------------------"<<'\n';
}

