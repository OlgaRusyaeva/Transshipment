//
//  LeadTimes.h
//  TransshipmentProject
//
//  Created by Olga on 23/08/15.
//
//

#ifndef __TransshipmentProject__LeadTimes__
#define __TransshipmentProject__LeadTimes__

#include "types.h"


class LeadTimes{
    
public:
    LeadTimes(const matrixInt_t& leadTimeMatrix);
    
    ~LeadTimes(){}
    
    int getLeadTimeBetweenLocations(const int& locationA, const int& locationB) const;
    
    int getMaxLeadTimeForLocation(const int& location) const;
    
    matrixInt_t getLeadTimes() const;
    
    //vector_t getMaxLeadTimePlusOne() const;
    
    int getMaxLeadTimePlusOne() const;
    
    vector_t getUniqueLeadTimesIntoLocation(const int& location) const;
    
    int getUniqueTotalSize() const;
    
private:
    matrixInt_t leadTimes;
    vector_t maxLeadTimes;
    int maxLeadTimePlusOne;
    matrixInt_t uniqueLeadTimes;
    int size;
    
    
};

#endif /* defined(__TransshipmentProject__LeadTimes__) */
