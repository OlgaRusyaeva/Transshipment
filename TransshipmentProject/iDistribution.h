//
//  iDistribution.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/13/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_iDistribution_h
#define TransshipmentProject_iDistribution_h

#include <boost/random.hpp>
//#include <boost/math/special_functions/gamma.hpp>
//#include <boost/math/special_functions/factorials.hpp>
//#include <boost/math/special_functions/beta.hpp>
//#include <boost/random/normal_distribution.hpp>
//#include <boost/math/special_functions/erf.hpp>
#include <boost/math/distributions.hpp>


class iDistribution {
    
public:
    virtual ~iDistribution(){}
    //seed should be a parameter of sample
    virtual float sample()=0;
    virtual float pdf(const float& x)=0;
    virtual float cdf(const float& x)=0;
    virtual float mean()=0;
    virtual float complement_cdf(const float& x)=0;
    //virtual float complement_quantile(const float& q)=0;
    //virtual float quantile(const float& p)=0;
    virtual float appMaxValue()=0;
    virtual float minValue()=0;
    virtual float maxValue()=0;
    virtual float var()=0;
    virtual void changeSeed(const int& newSeed)=0;
    virtual void resetPRNG()=0;
    
    //cdf the sum of n iid random varibales
    virtual float cdfSum(const float& x, const int& n)=0;
    //pdf the sum of n iid random varibales
    virtual float pdfSum(const float& x, int n)=0;
    //virtual const iDistribution& operator=(const iDistribution& distribution);
    
    
};





#endif
