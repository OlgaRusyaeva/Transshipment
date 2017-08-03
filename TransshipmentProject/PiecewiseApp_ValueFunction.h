//
//  PiecewiseApp_ValueFunction.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/7/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_PiecewiseApp_ValueFunction_h
#define TransshipmentProject_PiecewiseApp_ValueFunction_h

#include "types.h"

class PiecewiseApp_ValueFunction {
    
public:
    PiecewiseApp_ValueFunction(int time, int location);
    ~PiecewiseApp_ValueFunction(){}
    
    int getPrameterA()const{return indexA;}
    int getPrameterB()const{return indexB;}
    
    vector_t getIndexVector(const int& index)const;
    
    matrixInt_t getBreakpoints(const int& index)const;
    
    matrix_t getSlopes(const int& index)const;
    
    vector_t getBreakpointsRow(const int& x_id, const int& y_id)const;
    
    row_t getSlopeRow(const int& x_id, const int& y_id)const;
    
    int getIndexVectorElement(const int& x_id, const int& y_id)const;
    
    int getBreakpointsElement(const int& x_id, const int& y_id, const int& z_id)const;
    
    int getBreakpointIndexClosestToElement(const int& x_id, const int& y_id, const int& elementToCheck);
    
    float getSlopeElement(const int& x_id, const int& y_id, const int& z_id)const;
    
    void setIndexVectorElement(const int& x_id, const int& y_id, const int& value);
    
    void setBreakpointsElement(const int& x_id, const int& y_id, const int& z_id, const int& value);
    
    void setSlopeElement(const int& x_id, const int& y_id, const int& z_id, const float& value);
    
    void insertBreakpointsElement(const int& x_id, const int& y_id, const int& pos, const int& value);
    
    void insertSlopeElement(const int& x_id, const int& y_id, const int& pos, const float& value);
    
    void push_backBreakpointsElement(const int& x_id, const int& y_id,  const int& value);
    
    void push_backSlopeElement(const int& x_id, const int& y_id, const float& value);
    
    void update(const int& t, const int& loc,  const float& new_slopePlus, const float& new_slopeMinus, const int& pos_A, const int& pos_B, const int& currentState, const float& alpha);
    
    void insert(const int& t, const int& loc, const int& new_breakpoint, int& increase_index, int& pos);
    
    //should change k, u, v
    void updateSlopeCave(const int& t, const int& loc, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& L, const int& iteration, const int& step);
    
    
    void updateSlopeLeveling(const int& t, const int& loc, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& iteration);
    
    //void updateSlopeSpar(const int& t, const int& loc, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& iteration);
    
    void findUpdateIntervalRight(float& rightUpdateSlope,const int& pos,int& pos_r,const row_t& v, const int& k_max);
    
    void findUpdateIntervalLeft(float& leftUpdateSlope,const int& pos,int& pos_l,const row_t& v);
    
    const PiecewiseApp_ValueFunction& operator=(const PiecewiseApp_ValueFunction& newApproximation);
    
    
private:
    int indexA;
    int indexB;
    
    matrixInt_t indexVector;
    vecMatrixInt breakpoints;
    vecMatrixFloat slope;
    
    void initialize();
    void setInitialValue();
    
};


#endif
