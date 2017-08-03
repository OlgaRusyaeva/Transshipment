//
//  NormalDistribution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 10/30/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_NormalDistribution_h
#define TransshipmentProject_NormalDistribution_h

#include "iDistribution.h"

class NormalDistribution: public iDistribution
{
public:
    NormalDistribution(float m,float s, int seed);
    ~NormalDistribution(){}
    float sample();
    float pdf(const float& x);
    float cdf(const float& x);
    const NormalDistribution& operator=(const NormalDistribution& nd);
    float mean();
    float complement_cdf(const float& x);
    float complement_quantile(const float& q);
    float quantile(const float& p);
    float appMaxValue();
    float var();
    float minValue();
    float maxValue();
    float cdfSum(const float& x, const int& n);
    float pdfSum(const float& x, int n);
    
    void changeSeed(const int& newSeed);
   	void resetPRNG();
    
private:
    // Use the boost random number generator
    float mn;
    float sdis;
    boost::mt19937 rng;
    // Make a variate_generator OBJECT.
    boost::variate_generator<boost::mt19937&,boost::normal_distribution<> > var_nor;
};


#endif
