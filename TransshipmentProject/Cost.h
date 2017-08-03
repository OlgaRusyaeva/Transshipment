//
//  Vector of costs
//  

#ifndef project3_Cost_h
#define project3_Cost_h

#include "types.h"


class Cost{
    
public:
    Cost(int length);
    ~Cost(){}
    
    row_t getOrderingCost() const;
    
    row_t getSalesCost() const;
    
    row_t getHoldingCost() const;

    row_t getTransshipmentCost() const;
    
    void setOrderingCost(const int& i, const float& cost);
    void setSalesCost(const int& i, const float& cost);
    void setHoldingCost(const int& i, const float& cost);
    void setTransshipmentCost(const int& i, const float& cost);
    
    void setDistances(const matrix_t& dist);
    
    void setXCoordinates(const int& location, const int& xCoordinate);
    
    void setYCoordinates(const int& location, const int& yCoordinate);
    
    int getXCoordinates(const int& location)const;
    
    int getYCoordinates(const int& location)const;
    
    float getDistance(const int& row_id, const int& column_id) const;
    
    row_t getDistanceFromLocation(const int& row_id) const;
    
    int getSize() const;

private:
    matrix_t costVector;
    matrix_t distance;
    
    vector_t x_axis;
    vector_t y_axis;
    int size;
    
    
    void initialize();
    
    
    
};



#endif
