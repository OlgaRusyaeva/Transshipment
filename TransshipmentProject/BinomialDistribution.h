//
//  BinomialDistribution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_BinomialDistribution_h
#define TransshipmentProject_BinomialDistribution_h

#include "iDistribution.h"


class BinomialDistribution: public iDistribution
{
public:
    BinomialDistribution(int trialNumbers, float successProb, int seed);
    ~BinomialDistribution(){}
    float sample();
    float pdf(const float& x);
    float cdf(const float& x);
    const BinomialDistribution& operator=(const BinomialDistribution& bd);
    float mean();
    float var();
    float complement_cdf(const float& x);
    //float complement_quantile(const float& q);
    //float quantile(const float& p);
    float appMaxValue();
    float minValue();
    float maxValue();
    float cdfSum(const float& x, const int& n);
    float pdfSum(const float& x, int n);
    
    void changeSeed(const int& newSeed);
   	void resetPRNG();
    
private:
    float prob;
    int tn;
    boost::mt19937 rng; 
    boost::math::binomial_distribution<>binom;
    boost::variate_generator<boost::mt19937&,boost::binomial_distribution<> > var_binom;
};


#endif
