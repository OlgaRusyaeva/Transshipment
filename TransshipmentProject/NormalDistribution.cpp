//
//  NormalDistribution.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 10/30/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "NormalDistribution.h"
#include <boost/random/normal_distribution.hpp>
#include <boost/math/special_functions/erf.hpp>

NormalDistribution::NormalDistribution(float m, float s, int seed):rng(seed), var_nor(rng, boost::normal_distribution<>(m, s)),mn(m), sdis(s){}

float NormalDistribution::pdf(const float& x)
{
    return cdf(x+0.5)-cdf(x-0.5);
    //static const float inv_sqrt_2pi=0.39894228040143217f;
    //float a=(x-mn)/sd;
    //return inv_sqrt_2pi/sd*std::exp(-float(0.5)*a*a);
}

float NormalDistribution::sample() //discretization
{
    float x = (float)var_nor();
    return (floor)(x+0.5);
}

float NormalDistribution::cdf(const float& x)
{
    return 0.5f * (float)erfc(-((x+0.5)-mn)/(sdis*sqrt(2))); //x+0.5
}

//check
const NormalDistribution& NormalDistribution::operator=(const NormalDistribution& nd)
{
    this->mn=nd.mn;
    this->sdis=nd.sdis;
    return *this;
}

float NormalDistribution::mean() {
    return mn;
}

float NormalDistribution::var() {
    return sdis;
}

float NormalDistribution::appMaxValue() {
    return (int)quantile(0.9);
}

//-inf
float NormalDistribution::minValue() {
    return -std::numeric_limits<float>::max();
}

//+inf
float NormalDistribution::maxValue() {
    return std::numeric_limits<float>::max();
}

float NormalDistribution::quantile(const float& p) {
    
    return (mn - sdis * sqrt(2) * boost::math::erfc_inv(2*p));
}


float NormalDistribution::complement_cdf(const float& x) {
    
    return 0.5f * (float)erfc(((x-0.5)-mn)/(sdis*sqrt(2)));//x-0.5
}

float NormalDistribution::complement_quantile(const float& q) {
    
    return (mn + sdis * sqrt(2) * boost::math::erfc_inv(2*q));
}

//TODO
float NormalDistribution::cdfSum(const float& x, const int& n)
{
    return 1;
}
float NormalDistribution::pdfSum(const float& x, int n)
{
    return 1;
}

void NormalDistribution::changeSeed(const int& newSeed)
{
	var_nor.engine().seed(newSeed);
}
void NormalDistribution::resetPRNG()
{
    var_nor.distribution().reset();
}
