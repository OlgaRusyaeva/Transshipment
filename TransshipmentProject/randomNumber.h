//
//  Header randomNumber.cpp
// 


#ifndef project3_randomNumber_h
#define project3_randomNumber_h


//generate random number in a range

#include <boost/random.hpp>

//can use template
//but what to do with different distribution?
    
class randomIntNumber{
        
public:
    randomIntNumber(const int& minValue, const int& maxValue, const int& seed);
    ~randomIntNumber(){}
        
    int sample();
        
private:
    int min;
    int max;
        
    boost::mt19937 rng; 
    boost::variate_generator<boost::mt19937&,boost::uniform_int<> > var_uni;
};
    
class randomDoubleNumber{
        
public:
    randomDoubleNumber(const double& minValue, const double& maxValue, const int& seed);
    ~randomDoubleNumber(){}
        
    double sample();
        
private:
    double min;
    double max;
        
    boost::mt19937 rng; 
    boost::variate_generator<boost::mt19937&,boost::uniform_real<> > var_uni;
        
};
    
    
    //double randomIntNumber(const int& min, const int& max);
    //double randomDoubleNumber(const double& min, const double& max);


#endif