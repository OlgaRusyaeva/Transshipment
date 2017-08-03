//
//  DiscreteDistribution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 9/23/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_DiscreteDistribution_h
#define TransshipmentProject_DiscreteDistribution_h

#include "iDistribution.h"
#include "types.h"
//#include <boost/random/discrete_distribution.hpp>

//static const float probabilities[5]={0.5, 0, 0, 0, 0.5};
//static const float probabilities[5]={0.2, 0.2, 0.2, 0.2, 0.2};
//static const float probabilities[5]={0.1, 0.2, 0.4, 0.2, 0.1};


class DiscreteDistribution: public iDistribution
{
public:
    DiscreteDistribution(int seed, row_t prob);//TODO input row_t probabilities, maybe aBorder and bBorder or vector_t values
    ~DiscreteDistribution(){}
    float sample();
    float mean();
    float cdf(const float& x);
    const DiscreteDistribution& operator=(const DiscreteDistribution& und);
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
    boost::mt19937 rng;
    row_t probabilities;
};


#endif
