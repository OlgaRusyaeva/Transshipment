//
//  NegativeBinomialDistribution.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 5/21/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "NegativeBinomialDistribution.h"
#include <boost/math/special_functions/beta.hpp>

NegativeBinomialDistribution::NegativeBinomialDistribution(int success, float successFraction, int seed):rng(seed), var_negbinom(rng, boost::random::negative_binomial_distribution<>(success,successFraction)), r(success),prob(successFraction){}


float NegativeBinomialDistribution::sample(){
    float x = var_negbinom();
    return x;
}

float NegativeBinomialDistribution::pdf(const float& k) {
    return (float)(prob/(r + k)) * boost::math::ibeta_derivative(r, (k+1), prob);
}

float NegativeBinomialDistribution::cdf(const float& k) {
    
	return boost::math::ibeta(r, (k+1), prob);
}

float NegativeBinomialDistribution::appMaxValue() {
    return boost::math::ibeta_invb(r, prob, 0.9) - 1;
}

float NegativeBinomialDistribution::minValue() {
    return 0;
}

//inf
float NegativeBinomialDistribution::maxValue() {
    return std::numeric_limits<float>::max();
}

/*float NegativeBinomialDistribution::quantile(const float& p) {
    
	return boost::math::ibeta_invb(r, prob, p) - 1;
}*/


float NegativeBinomialDistribution::complement_cdf(const float& k) {
    
	return  boost::math::ibetac(r, (k+1), prob);
}

/*float NegativeBinomialDistribution::complement_quantile(const float& q) {
    
	return boost::math::ibetac_invb(r, prob, q) -1;
}*/

//check
const NegativeBinomialDistribution& NegativeBinomialDistribution::operator=(const NegativeBinomialDistribution& nbd)
{
    this->r=nbd.r;
    this->prob=nbd.prob;
    return *this;
}

float NegativeBinomialDistribution::mean(){
    return (float)(r*(1-prob))/prob;
}

float NegativeBinomialDistribution::var() {
    return (float)(r*(1-prob))/(prob*prob);
}

//can be simplified in case of geometric distribution (r=1)
float NegativeBinomialDistribution::cdfSum(const float& x, const int& n)
{
    return boost::math::ibeta(r*n, (x+1), prob);
    //boost::math::negative_binomial_distribution<>negBinom (r*n,prob);
    //return boost::math::cdf(negBinom,x);
    
    /*float cdfS=0;

    for (int i=0; i<=x; i++) {
            cdfS+=pdfSum(i,n);
    }
        
    
    return cdfS; */
}
float NegativeBinomialDistribution::pdfSum(const float& x, int n)
{
    return (float)(prob/(r*n + x)) * boost::math::ibeta_derivative(r*n, (x+1), prob);
    //boost::math::negative_binomial_distribution<>negBinom (r*n,prob);
    //return boost::math::pdf(negBinom,x);
	/*float pdfS=0;
    
    if (n==1) {
        pdfS=pdf(x);
    }
    else
        for (int i=0; i<=x; i++) {
            pdfS+=pdf(x-i)*pdfSum(i,n-1);
        }
    
    return pdfS;*/
}

void NegativeBinomialDistribution::changeSeed(const int& newSeed)
{
	var_negbinom.engine().seed(newSeed);
}
void NegativeBinomialDistribution::resetPRNG()
{
    var_negbinom.distribution().reset();
}
