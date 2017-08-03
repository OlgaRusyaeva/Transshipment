//
//  PoissonDistribution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_PoissonDistribution_h
#define TransshipmentProject_PoissonDistribution_h

#include "iDistribution.h"


class PoissonDistribution: public iDistribution
{
public:
    PoissonDistribution(float averageRate,int seed);
    ~PoissonDistribution(){}
    float sample();
    float pdf(const float& x);
    float cdf(const float& x);
    //float quantile(const float& p);
    float complement_cdf(const float& x);
    //float complement_quantile(const float& q);
    const PoissonDistribution& operator=(const PoissonDistribution& nd);
    float mean();
    float var();
    float appMaxValue();
    float minValue();
    float maxValue();
    float cdfSum(const float& x, const int& n);
    float pdfSum(const float& x, int n);
    
    void changeSeed(const int& newSeed);
   	void resetPRNG();
    
private:
    float averageRate;
    boost::mt19937 rng; 
    boost::math::poisson_distribution<> poisson;
    boost::variate_generator<boost::mt19937&,boost::poisson_distribution<> > var_poisson;
};


#endif
