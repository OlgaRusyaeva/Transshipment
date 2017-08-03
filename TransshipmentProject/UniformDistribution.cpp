//
//  UniformDistribution.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "UniformDistirbution.h"
#include <boost/math/distributions/normal.hpp>
#include <math.h>  

//change as for Binomial, input should be mean and sd or other way around
UniformDistirbution::UniformDistirbution(float a, float b, int seed):rng(seed), var_uni(rng, boost::uniform_int<>(a, b)),aBorder(a), bBorder(b){}

float UniformDistirbution::sample() {
    float x = var_uni();
    return x;
}

float UniformDistirbution::mean() {
    float x = (aBorder+bBorder)/2;
    return x;
}

float UniformDistirbution::var() {
    return ((this->bBorder-this->aBorder+1)*(this->bBorder-this->aBorder+1)-1)/12;
}

float UniformDistirbution::appMaxValue() {
    return bBorder;
}

float UniformDistirbution::maxValue() {
    return bBorder;
}

float UniformDistirbution::minValue() {
    return aBorder;
}

float UniformDistirbution::cdf(const float& x){
    if (x<aBorder) {
        return 0;
    }
    if (x>bBorder) {
        return 1;
    }
    return (floor(x)-aBorder+1)/(bBorder-aBorder+1);
}

float UniformDistirbution::pdf(const float& x){
    if ((x>=aBorder)&&(x<=bBorder) ) {
        return 1/(bBorder-aBorder+1);
    }
    return 0;
}

const UniformDistirbution& UniformDistirbution::operator=(const UniformDistirbution& und)
{
    this->aBorder=und.aBorder;
    this->bBorder=und.bBorder;
    return *this;
}

/*float UniformDistirbution::quantile(const float& p) //it is for continous case, remove it
{
    
    return p * (bBorder - aBorder) + aBorder;
}*/


float UniformDistirbution::complement_cdf(const float& k) {
    
    return (1-cdf(k));
}

/*float UniformDistirbution::complement_quantile(const float& q) //it is for continous case, remove it
{
    
    return -q*(bBorder - aBorder) + bBorder;
}*/

//can be approximated with normal(sumMean, sumVariance)
float UniformDistirbution::cdfSum(const float& x, const int& n)
{
    //boost::math::normal_distribution<>normal (n*mean(),sqrt(n*var()));
    //return boost::math::cdf(normal,x);
    float cdfS=0;
    if (x>n*bBorder) {
        cdfS=1;
    }
    else
    {
        if (x<n*aBorder) {
            cdfS=0;
        }
        else
        {
            for (int i=n*aBorder; i<=x; i++) {
                cdfS+=pdfSum(i,n);
            }
        }
        
    }
    
    return cdfS;
}
float UniformDistirbution::pdfSum(const float& x, int n)
{
    //boost::math::normal_distribution<>normal (n*mean(),sqrt(n*var()));
    //return boost::math::pdf(normal,x);
    float pdfS=0;
    
    if (n==1) {
        pdfS=pdf(x);
    }
    else
        for (int i=aBorder; i<=x; i++) {
            pdfS+=pdf(x-i)*pdfSum(i,n-1);
        }
    
    return pdfS;
   
}

void UniformDistirbution::changeSeed(const int& newSeed)
{
	var_uni.engine().seed(newSeed);
}
void UniformDistirbution::resetPRNG()
{
    var_uni.distribution().reset();
}
