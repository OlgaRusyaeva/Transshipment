//
//  Actions.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "Actions.h"
#include <fstream>


Actions::Actions(int sizeOfActions):size(sizeOfActions)
{
    initialize();
}

    
const Actions& Actions::operator=(const Actions& newActions)
{
    for (int i=0; i<this->getSize(); i++) {
        for (int j=0; j<this->getSize(); j++) {
            this->setElement(i,j,newActions.getElement(i,j));
        }
    }
    return *this;
}

Actions& operator+=(Actions& left, const Actions& right)
{
    for (int i=0; i<left.getSize(); i++) {
        for (int j=0; j<left.getSize(); j++) {
            left.setElement(i,j,left.getElement(i,j)+right.getElement(i,j));
        }
    }
    return left;
}
    
/*void Actions::setXCoordinates(const int& location, const int& xCoordinate)
{
    x_axis[location]=xCoordinate;
}*/
    
/*void Actions::setYCoordinates(const int& location, const int& yCoordinate)
{
    y_axis[location]=yCoordinate;
}*/

/*int Actions::getXCoordinates(const int& location)const
{
    return x_axis[location];
}*/

/*int Actions::getYCoordinates(const int& location)const
{
    return y_axis[location];
}*/

void Actions::setElement(const int& row_id, const int& column_id, const float& value)
{
    actionMatrix[row_id][column_id]=value;
}
    
float Actions::getElement(const int& row_id, const int& column_id)const
{
    return actionMatrix[row_id][column_id];
}

row_t Actions::getElementsForState(const int& state_id)const
{
    return actionMatrix[state_id];
}
    
int Actions::getSize()const
{
    return size;
}

bool Actions::isZero()const
{
    bool flag=true;
    for(int i=0;i<size;i++)
    {
        for(int j=0;j<size;j++)
        {
            if(actionMatrix[i][j]!=0)
            {
                flag=false;
                break;
                
            }
        }
    }
    
    return flag;
}
    
/*int Actions::getClosestLocation(const int& state_id)const
{
    int closestLocation=state_id;
    int minDistance=actionMatrix[state_id][state_id];
    for (int column_id=0; column_id<size; column_id++) {
        if (actionMatrix[state_id][column_id]>=minDistance) {
            minDistance=actionMatrix[state_id][column_id];
            closestLocation=column_id;
        }
    }
    return closestLocation;
}
    
row_t Actions::getAverageAction() const
{
    row_t averageAction=row_t(size);
    for (int row_id=0; row_id<size; row_id++) {
        float sumAction=0;
        for (int column_id=0; column_id<size; column_id++) {
            sumAction+=actionMatrix[row_id][column_id];
        }
        averageAction[row_id]=sumAction/size;
    }
    return averageAction;
}*/
    
void Actions::initialize()
{
    actionMatrix= matrix_t(size, row_t(size));
    //x_axis=vector_t(size);
    //y_axis=vector_t(size);
}

void Actions::print() const
{
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            std::cout<<getElement(i,j)<<" ";
        }
        std::cout<<'\n';
    }
}
    
    

