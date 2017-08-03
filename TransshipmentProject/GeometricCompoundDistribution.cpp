//
//  GeometricCompoundDistribution.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 8/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <GeometricCompoundDistribution.h>
#include <boost/math/special_functions/factorials.hpp>

GeometricCompoundDistribution::GeometricCompoundDistribution(float lambda,float beta, int seed):rng(seed),var_poisson(rng, boost::poisson_distribution<>(lambda)),var_geometric(rng, boost::geometric_distribution<>(beta)), averageRate(lambda), b(beta){}


float GeometricCompoundDistribution::sample(){
    float x=0;
    for (int i=0; i<var_poisson(); i++) {
        x+=var_geometric();
    }
    return x;
}

float GeometricCompoundDistribution::pdf(const float& k) 
{
    float probability=1;
    if (k==0) {
        probability=(float)exp(-averageRate);
    }
    else
    {
        for (int i=1; i<=k; i++) {
            probability+=(float)exp(-averageRate)*pow(averageRate, i)/ boost::math::factorial<float>(i)*f(k,i);
        }
    }
    return probability;
}

float GeometricCompoundDistribution::f(const int&j, const int& i)
{
    float prob=1;
    if (i==0) {
        prob=(1-b)*pow(b, j-1);
    }
    else
    {
        for (int r=i-1; r<j; r++) {
            prob=f(j,r)*(1-b)*pow(b, j-r-1);
        }
    }
    return prob;
}

float GeometricCompoundDistribution::cdf(const float& k) {
    
    if (k<0) {
        return 0;
    }
    float cdf=0;
    for (int i=0; i<k; i++) {
     cdf+=pdf(i);
     }
    return cdf;
}
//check
float GeometricCompoundDistribution::appMaxValue() {
    return (int)quantile(0.9);
}

//check
float GeometricCompoundDistribution::minValue() {
    return 0;
}

//check
float GeometricCompoundDistribution::maxValue() {
    return std::numeric_limits<float>::max();
}


//check
float GeometricCompoundDistribution::quantile(const float& p) {
    
    float cdf=0;
    int i=0;
    while (cdf<p) {
        cdf+=pdf(i);
        i++;
    }
    return i;//i+1
}

//check
float GeometricCompoundDistribution::complement_cdf(const float& k) {
    
    if (k<0) {
        return 1;
    }
    return (1-cdf(k));
}
//check
float GeometricCompoundDistribution::complement_quantile(const float& q) {
    
    float ccdf=1;
    int i=0;
    while (ccdf>q) {
        ccdf-=pdf(i);
        i++;
    }
    return i;//i+1
}

const GeometricCompoundDistribution& GeometricCompoundDistribution::operator=(const GeometricCompoundDistribution& pd)
{
    this->averageRate=pd.averageRate;
    return *this;
}

float GeometricCompoundDistribution::mean(){
    return this->averageRate/(1-this->b);
}

float GeometricCompoundDistribution::var() {
    return sqrt(averageRate*(b+1)/((1-b)*(1-b)));
}

//TODO
float GeometricCompoundDistribution::cdfSum(const float& x, const int& n)
{
    return 1;
}
float GeometricCompoundDistribution::pdfSum(const float& x, int n)
{
    return 1;
}

void GeometricCompoundDistribution::changeSeed(const int& newSeed)
{
	var_poisson.engine().seed(newSeed);
    var_geometric.engine().seed(newSeed);
}
void GeometricCompoundDistribution::resetPRNG()
{
    var_poisson.distribution().reset();
    var_geometric.distribution().reset();
}

//sigma^2=averageRate(b+1)/(1-b)^2