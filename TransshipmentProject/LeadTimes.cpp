//
//  LeadTimes.cpp
//  TransshipmentProject
//
//  Created by Olga on 23/08/15.
//
//

#include "LeadTimes.h"

LeadTimes::LeadTimes(const matrixInt_t& leadTimeMatrix)
{
    size =(int)leadTimeMatrix.size();
    leadTimes=leadTimeMatrix;
    maxLeadTimes=vector_t(size);
    uniqueLeadTimes=matrixInt_t(size);
    for (int i=0; i<size; i++) {
        vector_t uniqueLeadTimesForLocation;
        uniqueLeadTimesForLocation=leadTimes[i];
        sort( uniqueLeadTimesForLocation.begin(), uniqueLeadTimesForLocation.end() );
        uniqueLeadTimesForLocation.erase( unique( uniqueLeadTimesForLocation.begin(), uniqueLeadTimesForLocation.end() ), uniqueLeadTimesForLocation.end() );
        maxLeadTimes[i]=*std::max_element(leadTimeMatrix[i].begin(),leadTimeMatrix[i].end());
        uniqueLeadTimes[i]=uniqueLeadTimesForLocation;
    }
    maxLeadTimePlusOne=*std::max_element(maxLeadTimes.begin(),maxLeadTimes.end())+1;

}


int LeadTimes::getLeadTimeBetweenLocations(const int& locationA, const int& locationB) const
{
    return leadTimes[locationA][locationB];
}


int LeadTimes::getMaxLeadTimeForLocation(const int& location) const
{
    return maxLeadTimes[location];
    
}

/*vector_t LeadTimes::getMaxLeadTimePlusOne() const
{
    return maxLeadTimes;
}*/

matrixInt_t LeadTimes::getLeadTimes() const
{
    return leadTimes;
}

int LeadTimes::getMaxLeadTimePlusOne() const
{
    return maxLeadTimePlusOne;
}

vector_t LeadTimes::getUniqueLeadTimesIntoLocation(const int& location) const
{
    return uniqueLeadTimes[location];
}

int LeadTimes::getUniqueTotalSize() const
{
    int sum=0;
    for (int i=0; i<size; i++) {
        sum+=(int)uniqueLeadTimes[i].size();
    }
    return sum;
}