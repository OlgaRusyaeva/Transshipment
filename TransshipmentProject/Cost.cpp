//
//  Cost.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "Cost.h"

using namespace std;

Cost::Cost(int length):size(length)
{
    initialize();
}

row_t Cost::getOrderingCost() const
{
    return costVector[3];
}
    
row_t Cost::getSalesCost() const
{
    return costVector[1];
}
    
row_t Cost::getHoldingCost() const
{
    return costVector[0];
}
    
row_t Cost::getTransshipmentCost() const
{
    return costVector[2];
}

void Cost::setOrderingCost(const int& i, const float& cost)
{
    costVector[3][i]=cost;
}

void Cost::setSalesCost(const int& i, const float& cost)
{
    costVector[1][i]=cost;
}

void Cost::setHoldingCost(const int& i, const float& cost)
{
    costVector[0][i]=cost;
}

void Cost::setTransshipmentCost(const int& i, const float& cost)
{
    costVector[2][i]=cost;
}
    
void Cost::initialize()
{
    costVector=matrix_t(4, row_t(size));
    distance=matrix_t(size,row_t(size));
    x_axis=vector_t(size);
    y_axis=vector_t(size);
        
}

void Cost::setDistances(const matrix_t& dist)
{
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            if (i!=j) {
                distance[i][j]=dist[i][j];
            }
        }
    }
}
    
void Cost::setXCoordinates(const int& location, const int& xCoordinate)
{
 	x_axis[location]=xCoordinate;
}

void Cost::setYCoordinates(const int& location, const int& yCoordinate)
{
 	y_axis[location]=yCoordinate;
}

int Cost::getXCoordinates(const int& location)const
{
 	return x_axis[location];
}

int Cost::getYCoordinates(const int& location)const
{
 	return y_axis[location];
}


float Cost::getDistance(const int& row_id, const int& column_id) const
{
    //return sqrtf(pow(((float)(x_axis[row_id]-x_axis[column_id])), 2)+pow(((float)(y_axis[row_id]-y_axis[column_id])), 2));
    return distance[row_id][column_id];
}

row_t Cost::getDistanceFromLocation(const int& row_id) const
{
    /*row_t vectorOfDistances(size);
    
    for (int column_id=0; column_id<size; column_id++) {
        vectorOfDistances[column_id]=getDistance(row_id,column_id);
    }
    
    return vectorOfDistances;*/
    return distance[row_id];
}

int Cost::getSize() const
{
    return size;
}






