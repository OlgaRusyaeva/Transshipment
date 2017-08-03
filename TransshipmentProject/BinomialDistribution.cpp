//
//  BinomialDistribution.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "BinomialDistribution.h"
#include <boost/math/special_functions/beta.hpp>

BinomialDistribution::BinomialDistribution(int trialNumbers, float successProb, int seed):rng(seed), var_binom(rng, boost::binomial_distribution<>(trialNumbers, successProb)), binom(trialNumbers, successProb),prob(successProb), tn(trialNumbers){}

float BinomialDistribution::sample() {
    float x = var_binom();
    return x;
}

float BinomialDistribution::pdf(const float& k) {
    
    //two ways to find distribtuion functions
    //1. initialize boost::math::ditribution in constructor
    // and use functions pdf, cdf, quantile, with the distribution as a parameter
    //2.explicitly calculate them as done here
    
    //return boost::math::pdf(binom,k);
    return boost::math::ibeta_derivative(k+1, tn-k+1, prob) / (tn+1);
}

float BinomialDistribution::cdf(const float& k){

    return boost::math::ibetac(k + 1, tn - k, prob);
}

float BinomialDistribution::appMaxValue()
{
    return tn;
}

float BinomialDistribution::minValue()
{
    return 0;
}

float BinomialDistribution::maxValue() {
    return tn;
}

const BinomialDistribution& BinomialDistribution::operator=(const BinomialDistribution& bd)
{
    this->prob=bd.prob;
    this->tn=bd.tn;
    return *this;
}

float BinomialDistribution::mean(){
    return (this->tn*this->prob);
}

float BinomialDistribution::var() {
    return (this->tn*this->prob)*(1-this->prob);
}

/*float BinomialDistribution::quantile(const float& p) //remove
{
    //boost::math::binomial_distribution<>binom (successProb,trialNumbers);
    return boost::math::quantile(binom,p);
}*/


float BinomialDistribution::complement_cdf(const float& k) {
    return boost::math::ibeta(k + 1, tn - k, prob);
}

/*float BinomialDistribution::complement_quantile(const float& q) //remove
{
    
    //boost::math::binomial_distribution<>binom (successProb,trialNumbers);
    return boost::math::quantile(complement(binom,q));
}*/

float BinomialDistribution::cdfSum(const float& x, const int& n)
{
    if (x>tn*n) {
        return 1;
    }
    else
    {
        return boost::math::ibetac(x + 1, tn*n - x, prob); 
    }
    //boost::math::binomial_distribution<>binom (successProb,n*trialNumbers);
    //return boost::math::cdf(binom,x);
}
float BinomialDistribution::pdfSum(const float& x, int n)
{
    if (x>tn*n) {
        return 0;
    }
    else
    {
        return boost::math::ibeta_derivative(x+1, tn*n-x+1, prob) / (tn*n+1);
    }
    //boost::math::binomial_distribution<>binom (successProb,n*trialNumbers);
    //return boost::math::pdf(binom,x);
}

void BinomialDistribution::changeSeed(const int& newSeed)
{
	var_binom.engine().seed(newSeed);
}
void BinomialDistribution::resetPRNG()
{
    var_binom.distribution().reset();
}
