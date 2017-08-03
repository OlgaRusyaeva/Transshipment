//
//  GammaDistribution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 1/30/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_GammaDistribution_h
#define TransshipmentProject_GammaDistribution_h

#include "iDistribution.h"

class GammaDistribution: public iDistribution
{
public:
    GammaDistribution(float m,float sd, int seed);
    ~GammaDistribution(){}
    float sample();
    float pdf(const float& x);
    float cdf(const float& x);
    const GammaDistribution& operator=(const GammaDistribution& nd);
    float mean();
    float var();
    float complement_cdf(const float& x);
    float complement_quantile(const float& q);
    float quantile(const float& p);
    float appMaxValue();
    float minValue();
    float maxValue();
    float cdfSum(const float& x, const int& n);
    float pdfSum(const float& x, int n);
    
    void changeSeed(const int& newSeed);
   	void resetPRNG();
    
private:
    // Use the boost random number generator
    float shape;
    float scale;
    boost::mt19937 rng;
    // Make a variate_generator OBJECT.
    boost::variate_generator<boost::mt19937&,boost::gamma_distribution<> > var_gamma;
};




#endif
