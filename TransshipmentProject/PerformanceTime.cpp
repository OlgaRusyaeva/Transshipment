//
//  Time.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 6/19/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "PerformaceTime.h"
#include <fstream>


using namespace std;

void PerformaceTime::start()
{
    //begin = clock();
     begin = boost::chrono::process_real_cpu_clock::now();
}

void PerformaceTime::finish()
{
    //end = clock();
   	//timeForScenario.push_back(float(end - begin)/CLOCKS_PER_SEC);
    end = boost::chrono::process_real_cpu_clock::now();
    timeForScenario.push_back(boost::chrono::duration_cast<boost::chrono::seconds>(end - begin).count());
    
}

void PerformaceTime::outputIntoCSV(const std::string& fileName)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(), ios::out | ios::app );
    
    for (int i=0; i<(int)timeForScenario.size(); i++) {
        outputFile<<timeForScenario[i]<<'\n';
    }
    outputFile.close();
}

void PerformaceTime::outputConsole()
{
    
    for (int i=0; i<(int)timeForScenario.size(); i++) {
        std::cout<<timeForScenario[i]<<'\n';
    }
}

void PerformaceTime::reset()
{
    timeForScenario.erase(timeForScenario.begin(),timeForScenario.end());
}