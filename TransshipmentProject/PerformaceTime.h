//
//  Time.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 6/19/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Time_h
#define TransshipmentProject_Time_h

#include <stdio.h>
#include <time.h>
#include "types.h"
#include <boost/chrono/process_cpu_clocks.hpp>

class PerformaceTime{
    
public:
    PerformaceTime(){}
    
    ~PerformaceTime(){}
    
    void start();
    
    void finish();
    
    void reset();
    
    void outputIntoCSV(const std::string& fileName);
    
    void outputConsole();
    
    
private:
    boost::chrono::process_real_cpu_clock::time_point begin, end;
    row_t timeForScenario;
    
};


#endif
