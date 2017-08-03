//
//  ValueFunctionClustered.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/5/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ValueFunctionClustered_h
#define TransshipmentProject_ValueFunctionClustered_h

#include "types.h"
#include <map>
#include "boost/tuple/tuple.hpp"

class ValueFunctionClustered{
    
public:
    ValueFunctionClustered(){}
    ~ValueFunctionClustered(){}
    
    float getValueFunction(const int& row_id, const size_t& col_id, const int& cluster_id) const;
    
    void setValueFunction(const int& row_id, const size_t& col_id, const int& cluster_id,const float& value);
    
    void printVFunction(std::ofstream &file)const;
    
    //void initializeClusters(const int& number, const Actions& distance);
    
    void inputVFunction(std::ifstream &inputFile);
    
    //vector_t getCluster(const int& number)const;
    
    const ValueFunctionClustered& operator=(const ValueFunctionClustered& newValueFunction);
    
private:
    std::map<boost::tuple<int, size_t,int>, float> valueFunctionMap;
    
    
    //void initialize();
    
    //vector_t Cluster(const int& numberOfLocations,const Actions& distance, const int& numberOfClusters)const;
    
};


#endif
