//
//  NegativeBinomialDistribution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 5/21/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_NegativeBinomialDistribution_h
#define TransshipmentProject_NegativeBinomialDistribution_h

#include "iDistribution.h"


class NegativeBinomialDistribution: public iDistribution
{
public:
    NegativeBinomialDistribution(int success, float successFraction,int seed);
    ~NegativeBinomialDistribution(){}
    float sample();
    float pdf(const float& x);
    float cdf(const float& x);
    //float quantile(const float& p);
    float complement_cdf(const float& x);
    //float complement_quantile(const float& q);
    const NegativeBinomialDistribution& operator=(const NegativeBinomialDistribution& nd);
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
    int r;
    float prob;
    boost::mt19937 rng; 
    boost::variate_generator<boost::mt19937&,boost::random::negative_binomial_distribution<> > var_negbinom;
};


#endif
