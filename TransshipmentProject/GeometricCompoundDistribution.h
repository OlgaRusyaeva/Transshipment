//
//  GeometricCompoundDistribution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 8/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_GeometricCompoundDistribution_h
#define TransshipmentProject_GeometricCompoundDistribution_h

#include "iDistribution.h"

class GeometricCompoundDistribution: public iDistribution
{
public:
    GeometricCompoundDistribution(float averageRate,float b, int seed);
    ~GeometricCompoundDistribution(){}
    float sample();
    float pdf(const float& x);
    float cdf(const float& x);
    float quantile(const float& p);
    float complement_cdf(const float& x);
    float complement_quantile(const float& q);
    const GeometricCompoundDistribution& operator=(const GeometricCompoundDistribution& nd);
    float mean();
    float appMaxValue();
    float var();
    float minValue();
    float maxValue();
    float cdfSum(const float& x, const int& n);
    float pdfSum(const float& x, int n);
    
    void changeSeed(const int& newSeed);
   	void resetPRNG();
    
private:
    float f(const int& i, const int& j);
    float averageRate;
    float b;
    boost::mt19937 rng; 
    boost::variate_generator<boost::mt19937&,boost::poisson_distribution<> > var_poisson;
    boost::variate_generator<boost::mt19937&,boost::geometric_distribution<> > var_geometric;
};



#endif
