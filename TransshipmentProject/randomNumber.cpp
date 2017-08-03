//
//  Geneartor of random number in a range
//  


#include "randomNumber.h"
#include <boost/random.hpp>
#include "types.h"

//random number in the range [min, max]
    randomIntNumber::randomIntNumber(const int& minValue, const int& maxValue, const int& seed):min(minValue), max(maxValue), rng(seed), var_uni(rng, boost::uniform_int<>(minValue, maxValue)){}
    
    
    int randomIntNumber::sample()
    {
        return var_uni();
    }
    
    randomDoubleNumber::randomDoubleNumber(const double& minValue, const double& maxValue, const int& seed):min(minValue), max(maxValue), rng(seed), var_uni(rng, boost::uniform_real<>(minValue, maxValue)){}
    
    double randomDoubleNumber::sample()
    {
        return var_uni();
    }

	/*base_gen_type gener;

	double randomIntNumber(const int& min, const int& max)
	{
    	boost::random::uniform_int_distribution<> dist(min, max);
    	return dist(gener);
    	//double range = (max - min);    
    	//return min + (range*rand()/((double)RAND_MAX)); 
	}

	double randomDoubleNumber(const double& min, const double& max)
	{
        boost::random::uniform_real_distribution<> dist(min,max);
        return dist(gener);
   	 	//double range = (max - min);    
    	//return min + (range*rand()/((double)RAND_MAX)); 
	}*/