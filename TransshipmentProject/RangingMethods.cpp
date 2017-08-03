//
//  RangingMethods.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "RangingMethods.h"
#include "iDistribution.h"
#include "State.h"
#include <boost/shared_ptr.hpp>

namespace rangingMethods 
{
    vector_t getMaxDemandForDistribution(const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& NumberOfLocations)
    {
        vector_t maxDemandForDistribution = vector_t(NumberOfLocations);
        for (int i=0; i<NumberOfLocations; i++) {
            maxDemandForDistribution[i]=distribution[i]->appMaxValue();
        }
        return maxDemandForDistribution;
    }
    
    list_t calculateRequiredQuantity(const vector_t& stock_out,const State& state,const vector_t& upperBound)
    {
        const int size=(int)stock_out.size();
        list_t requiredQuantity;
        for (int i=0; i<size; i++) {
            requiredQuantity.push_back(std::make_pair(stock_out[i],upperBound[stock_out[i]]-state.getElement(stock_out[i],0)));
        }
        return requiredQuantity;
    }
    
    list_t calculateAvailableQuantity(const vector_t& left_over,const State& state,const vector_t& lowerBound)
    {
        const int size=(int)left_over.size();
        list_t availableQuantity;
        for (int i=0; i<size; i++) {
            availableQuantity.push_back(std::make_pair(left_over[i],state.getElement(left_over[i],0)-lowerBound[left_over[i]]));
        }
        return availableQuantity;
    }
    
    vector_t getShortageLocations(const State& state,const vector_t& lowerBound)
    {
        const int L=(int)state.getLength();
        vector_t stock_out;
        for (int i=0; i<L; i++) {
            if (state.getElement(i,0)<lowerBound[i]) {
                stock_out.push_back(i);
            }
        }
        return stock_out; 
    }
    
    vector_t getExcessLocations(const State& state,const vector_t& upperBound)
    {
        const int L=(int)state.getLength();
        vector_t left_over;
        for (int i=0; i<L; i++) {
            if (state.getElement(i,0)>upperBound[i]) {
                left_over.push_back(i);
            }
        }
        return left_over; 
    }
    
    
    
    bool MyDataSortPredicateDescending(const std::pair<int,int>& lhs, const std::pair<int,int>& rhs) 
    { 
        return (lhs.second > rhs.second); 
    } 
}


