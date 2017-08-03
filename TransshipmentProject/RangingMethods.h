//
//  RangingMethods.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_RangingMethods_h
#define TransshipmentProject_RangingMethods_h

#include "types.h"

class State;
class iDistribution;

namespace rangingMethods {
    
    bool MyDataSortPredicateDescending(const std::pair<int,int>& lhs, const std::pair<int,int>& rhs);
    
    vector_t getMaxDemandForDistribution(const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& NumberOfLocations);
    
    vector_t getShortageLocations(const State& state,const vector_t& lowerBound);
    
    vector_t getExcessLocations(const State& state,const vector_t& upperBound);
    
    list_t calculateRequiredQuantity(const vector_t& stock_out,const State& state,const vector_t& upperBound);
    
    list_t calculateAvailableQuantity(const vector_t& left_over,const State& state,const vector_t& lowerBound);
};


#endif
