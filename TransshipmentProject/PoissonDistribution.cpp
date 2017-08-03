//
//  PoissonDistribution.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "PoissonDistribution.h"
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/special_functions/factorials.hpp>

PoissonDistribution::PoissonDistribution(float lambda, int seed):rng(seed), var_poisson(rng, boost::poisson_distribution<>(lambda)), poisson(lambda), averageRate(lambda){}


float PoissonDistribution::sample(){
    float x = var_poisson();
    return x;
}

float PoissonDistribution::pdf(const float& k) {
    return boost::math::pdf(poisson,k);
    //return (float)exp(-averageRate)*pow(averageRate, k)/ boost::math::factorial<float>(k);
}

float PoissonDistribution::cdf(const float& k) {
    
    /*if (k<0) {
        return 0;
    }
    return boost::math::gamma_q(k+1, averageRate);*/
    /*double cdf=0;
    for (int i=0; i<k; i++) {
        cdf+=pdf(i);
    }
    return cdf;*/
    return boost::math::cdf(poisson,k);
}

float PoissonDistribution::appMaxValue() {
    boost::math::poisson_distribution<>poisson (averageRate);
    return boost::math::quantile(poisson,0.9);
}

float PoissonDistribution::minValue() {
    return 0;
}

//inf
float PoissonDistribution::maxValue() {
    return std::numeric_limits<float>::max();
}

/*float PoissonDistribution::quantile(const float& p) {
    
    //return boost::math::gamma_q_inva(averageRate,p)-1;
    boost::math::poisson_distribution<>poisson (averageRate);
    return boost::math::quantile(poisson,p);
}*/


float PoissonDistribution::complement_cdf(const float& k) {
    
    if (k<0) {
        return 1;
    }
    return boost::math::gamma_p(k+1, averageRate);
}

/*float PoissonDistribution::complement_quantile(const float& q) {
    
    //return boost::math::gamma_p_inva(averageRate,q)-1;
    boost::math::poisson_distribution<>poisson (averageRate);
    return boost::math::quantile(complement(poisson,q));
}*/

//check
const PoissonDistribution& PoissonDistribution::operator=(const PoissonDistribution& pd)
{
    this->averageRate=pd.averageRate;
    return *this;
}

float PoissonDistribution::mean(){
    return this->averageRate;
}

float PoissonDistribution::var() {
    return this->averageRate;
}

float PoissonDistribution::cdfSum(const float& x, const int& n)
{
    if (x<0) {
        return 0;
    }
    return boost::math::gamma_q(x+1, averageRate*n);
    //boost::math::poisson_distribution<>poisson (averageRate*n);
    //return boost::math::cdf(poisson,x);
}
float PoissonDistribution::pdfSum(const float& x, int n)
{
    return (float)exp(-averageRate*n)*pow(averageRate*n, x)/ boost::math::factorial<float>(x);
    //boost::math::poisson_distribution<>poisson (averageRate*n);
    //return boost::math::pdf(poisson,x);
}

void PoissonDistribution::changeSeed(const int& newSeed)
{
	var_poisson.engine().seed(newSeed);
}
void PoissonDistribution::resetPRNG()
{
    var_poisson.distribution().reset();
}