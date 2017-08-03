//
//  Value Function stores the value of being in the state(hash value) at specific time (int)
//  


#ifndef project2_ValueFunction_h
#define project2_ValueFunction_h

#include "types.h"
#include <map>
#include "boost/tuple/tuple.hpp"


class ValueFunction{
    
public:
    ValueFunction(){}
    ~ValueFunction(){}
    
    float getValueFunction(const int& row_id, const size_t& col_id) const;
    
    void setValueFunction(const int& row_id, const size_t& col_id, const float& value);
    
    void printVFunction(std::ofstream &file)const;
    
    //void initializeClusters(const int& number, const Actions& distance);

    void inputVFunction(std::ifstream &inputFile);
    
    //vector_t getCluster(const int& number)const;
    
    const ValueFunction& operator=(const ValueFunction& newValueFunction);
    
private:
    std::map<std::pair<int, size_t>, float> valueFunctionMap;
    //matrixInt_t matrixOfClusters;
    
    //void initialize();
    
    //vector_t Cluster(const int& numberOfLocations,const Actions& distance, const int& numberOfClusters)const;

};

#endif
