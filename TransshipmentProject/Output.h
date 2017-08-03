//
//  OutputGnuplot.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_OutputGnuplot_h
#define TransshipmentProject_OutputGnuplot_h

#include "types.h"

class State;
class Cost;

namespace output {
    void outputNumericalResults(const std::string& fileName, const int& number);
    
    void outputMap(const State& state, const Cost& distance);
    
    void outputClustersMap(const Cost& distance,const matrixInt_t& clusters);
    
    void cleanFile(const std::string& fileName);
    
    void outputValueFunction(const int& size1, const int& size2,const std::string& fileName,const int& time);
    
    void outputIntoCSV(const std::string& fileName,const row_t& data, int& number);
};




#endif
