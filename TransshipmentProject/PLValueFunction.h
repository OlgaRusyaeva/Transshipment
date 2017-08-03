//
//  PLValueFunction.h
//  TransshipmentProject
//
//  Created by Olga on 25/08/15.
//
//

#ifndef __TransshipmentProject__PLValueFunction__
#define __TransshipmentProject__PLValueFunction__

#include "PiecewiseLinearFunctionSet.h"
#include "types.h"
#include "array.h"
#include <boost/shared_ptr.hpp>


class PLValueFunction
{
public:
    PLValueFunction(int sizeHH, int sizeWW, int sizeDD);
    ~PLValueFunction(){}
    
    PiecewiseLinearFunction& get(const int& indexH, const int& indexW, const int& indexD);
    
    int getSizeH() const;
    
    int getSizeW() const;
    
    int getSizeD() const;
    
    boost::shared_ptr<PiecewiseLinearFunctionSet>& getPiecewiseLinearFunctionFor(const int& indexD);
    
    //should change k, u, v
    void updateSlopeCave(const int& t, const int& loc, const int& leadTime, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& L, const int& iteration, const int& step);
    
    
    void updateSlopeLeveling(const int& t, const int& loc, const int& leadTime, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& iteration);
    
    void updateSlopeSpar(const int& t, const int& loc, const int& leadTime, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& iteration);
    
    void printSlopes();
    
    void printBreakpoints();
    
    void printIndexes();
    
    
private:
    std::vector<boost::shared_ptr<PiecewiseLinearFunctionSet> > set;
    int tuningParameter;
    int sizeH;
    int sizeW;
    int sizeD;
    
    void update(PiecewiseLinearFunction& pl,  const float& new_slopePlus, const float& new_slopeMinus, const int& pos_A, const int& pos_B, const int& currentState, const float& alpha);
    
    void insert(PiecewiseLinearFunction& pl,  const int& new_breakpoint, int& pos);
    
    void findUpdateIntervalRight(float& rightUpdateSlope,const int& pos,int& pos_r,const row_t& v, const int& k_max);
    
    void findUpdateIntervalLeft(float& leftUpdateSlope,const int& pos,int& pos_l,const row_t& v);
    
    int nearestBreakpoint(const PiecewiseLinearFunction& pl, const int& value);
    

    
};

#endif /* defined(__TransshipmentProject__PLValueFunction__) */
