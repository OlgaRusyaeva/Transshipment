//
//  GammaDistribution.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 1/30/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "GammaDistribution.h"
#include <boost/math/special_functions/gamma.hpp>


GammaDistribution::GammaDistribution(float m, float sd, int seed):rng(seed), var_gamma(rng, boost::gamma_distribution<>((m*m)/(sd*sd), (sd*sd)/m)),shape((m*m)/(sd*sd)), scale((sd*sd)/m){}

float GammaDistribution::pdf(const float& x)
{
    //return boost::math::gamma_p_derivative(shape,x/scale)/scale;
    return cdf(x+0.5)-cdf(x-0.5);
}

float GammaDistribution::sample() //discretization
{
    float x = (float)var_gamma();
    return (floor)(x+0.5);
}

float GammaDistribution::cdf(const float& x)
{
    if (x<0) {
        return 0;
    }
    return boost::math::gamma_p(shape, (x+0.5)/scale); //x+0.5
}

//check
const GammaDistribution& GammaDistribution::operator=(const GammaDistribution& gm)
{
    this->shape=gm.shape;
    this->scale=gm.scale;
    return *this;
}

float GammaDistribution::mean() {
    return shape*scale;
}

float GammaDistribution::var() {
    return shape*scale*scale;
}


float GammaDistribution::appMaxValue() {
    return quantile(0.9);
}

float GammaDistribution::minValue() {
    return 0;
}

//inf
float GammaDistribution::maxValue() {
    return std::numeric_limits<float>::max();;
}

float GammaDistribution::quantile(const float& p) {
    
    //return scale*boost::math::gamma_p_inv(shape, p);
    boost::math::gamma_distribution<>gamma (shape,scale);
    return boost::math::quantile(gamma,p);
}


float GammaDistribution::complement_cdf(const float& x) {
    
    if (x<0) {
        return 1;
    }
    return boost::math::gamma_q(shape,(x-0.5)/scale);//x-0.5
}

float GammaDistribution::complement_quantile(const float& q) {
    
    //return scale*boost::math::gamma_q_inv(shape, q);
    boost::math::gamma_distribution<>gamma (shape,scale);
    return boost::math::quantile(complement(gamma,q));
}

//TODO
float GammaDistribution::cdfSum(const float& x, const int& n)
{
    return 1;
}
float GammaDistribution::pdfSum(const float& x, int n)
{
    return 1;
}

void GammaDistribution::changeSeed(const int& newSeed)
{
	var_gamma.engine().seed(newSeed);
}
void GammaDistribution::resetPRNG()
{
    var_gamma.distribution().reset();
}
