//
//  Demand vector for the all locations
//  


#ifndef project2_Demand_h
#define project2_Demand_h

//#include <boost/random.hpp>
#include "types.h"
//#include "randomNumber.h"
//#include <fstream>
#include <map>



class Demand{
    
public:
    Demand(int length);
    
    ~Demand(){}
         
    vector_t getDemand() const;
    
    int getElement(const int& index)const;
    
    void setElement(const int& index,const int& newValue);
    
    void setDemandParameters(const int& key_id, const float& mean, const float& sd);
    
    std::map<int, std::pair<float, float> > getDemandParameters()const;
    
    std::pair<float, float> getDemandParametersForLocation(const int& index) const;
    
    void setDistribution(const int& index, const int& value);
    
    vector_t getDistribution()const;
    
    void print() const;
    
private:
    
    int vectorLength;
    vector_t demandVector;
    std::map<int, std::pair<float, float> > demandParameters;
    vector_t distributionType;
    
    void initialize();
    
    
    
};


#endif
