//
//  DiscreteDistribution.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 9/23/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include <DiscreteDistribution.h>
#include <math.h>  

DiscreteDistribution::DiscreteDistribution(int seed,row_t prob):rng(seed){probabilities=row_t((int)prob.size());probabilities=prob;}

float DiscreteDistribution::sample() {
    
    boost::random::discrete_distribution<> dist(probabilities);
    float x = dist(rng);
    return x;
}

float DiscreteDistribution::mean() {
    float x = 0;
    for (int p=0; p<(int)probabilities.size(); p++) {
        x+=p*probabilities[p];
    }
    return x;
}

float DiscreteDistribution::var() {
    float x = 0;
    for (int p=0; p<(int)probabilities.size(); p++) {
        x+=(p-mean())*(p-mean())*probabilities[p];
    }
    return x;
}

float DiscreteDistribution::appMaxValue() {
    return (int)probabilities.size()-1;
}

float DiscreteDistribution::maxValue() {
    return (int)probabilities.size()-1;
}

float DiscreteDistribution::minValue() {
    return 0;
}

float DiscreteDistribution::cdf(const float& x){
    if (x<0) {
        return 0;
    }
    if (x>(int)probabilities.size()-1) {
        return 1;
    }
    float cdf_value=0;
    for (int i=0; i<=x; i++) {
        cdf_value+=probabilities[i];
    }
    return cdf_value;
}

float DiscreteDistribution::pdf(const float& x){
    if ((x>=0)&&(x<=((int)probabilities.size()-1)) ) {
        return probabilities[(int)x];
    }
    return 0;
}

const DiscreteDistribution& DiscreteDistribution::operator=(const DiscreteDistribution& und)
{
    return *this;
}


float DiscreteDistribution::complement_cdf(const float& k) {
    return (1-cdf(k));
}


float DiscreteDistribution::cdfSum(const float& x, const int& n)
{
    float cdfS=0;
    if (x>n*(int)probabilities.size()) {
   	 	cdfS=1;
    }
    else
    {
    	if (x<n*0) {
    		cdfS=0;
    	}
    else
    	{
    		for (int i=n*0; i<=x; i++) {
    			cdfS+=pdfSum(i,n);
    		}
    	}
     
    }
    
     return cdfS;

}
float DiscreteDistribution::pdfSum(const float& x, int n)
{
	float pdfS=0;
     
    if (n==1) {
		pdfS=pdf(x);
    }
    else
    {
     	for (int i=0; i<=x; i++) {
     		pdfS+=pdf(x-i)*pdfSum(i,n-1);
    	 }
     
    }
    
    return pdfS;
    
}

void DiscreteDistribution::changeSeed(const int& newSeed)
{
	//dist.engine().seed(newSeed);
}
void DiscreteDistribution::resetPRNG()
{
    //dist.distribution().reset();
}
