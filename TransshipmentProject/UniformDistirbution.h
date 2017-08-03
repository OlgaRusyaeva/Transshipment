//
//  UniformDistirbution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_UniformDistirbution_h
#define TransshipmentProject_UniformDistirbution_h

#include "iDistribution.h"

class UniformDistirbution: public iDistribution
{
public:
    UniformDistirbution(float a, float b, int seed);
    ~UniformDistirbution(){}
    float sample();
    float mean();
    float cdf(const float& x);
    const UniformDistirbution& operator=(const UniformDistirbution& und);
    float pdf(const float& x);
    float complement_cdf(const float& x);
    //float complement_quantile(const float& q);
    //float quantile(const float& p);
    float appMaxValue();
    float var();
    float minValue();
    float maxValue();
    float cdfSum(const float& x, const int& n);
    float pdfSum(const float& x, int n);
    
    void changeSeed(const int& newSeed);
   	void resetPRNG();
    
private:
    float aBorder;
    float bBorder;
    boost::mt19937 rng; 
    boost::variate_generator<boost::mt19937&,boost::uniform_int<> > var_uni;
};

#endif
