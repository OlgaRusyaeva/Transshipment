
#ifndef project3_Actions_h
#define project3_Actions_h

#include "types.h"


class Actions{
    
public:
    Actions(int sizeOfActions);
    ~Actions(){}
    
    const Actions& operator=(const Actions& newActions);
    
    friend Actions& operator+=(Actions& left, const Actions& right);
    
    //void setXCoordinates(const int& location, const int& xCoordinate);
    //void setYCoordinates(const int& location, const int& yCoordinate);
    //int getXCoordinates(const int& location)const;
    //int getYCoordinates(const int& location)const;
    void setElement(const int& row_id, const int& column_id, const float& value);
    float getElement(const int& row_id, const int& column_id)const;
    row_t getElementsForState(const int& state_id)const;
    int getSize()const;
    bool isZero()const;
    
    void print() const;
    
    //int getClosestLocation(const int& state_id)const;
    
    //row_t getAverageAction() const;
    
private:
    matrix_t actionMatrix;
    //vector_t x_axis;
    //vector_t y_axis;
    
    void initialize();
    
protected:
    int size;
    
};

class LP_solution: public Actions
{
public:
    //why multiply? should be sum: v_size+s_size
    LP_solution(int v_size, int s_size):Actions(v_size){sp_size=s_size*v_size; shadowPricesPlus=row_t(s_size*v_size);shadowPricesMinus=row_t(s_size*v_size);}
    
    const LP_solution& operator=(const LP_solution& newSolution);
    
    int getShadowPriceNumber() const;
    
    void setShadowPricePlus(const int& id, const float& value);
    void setShadowPriceMinus(const int& id, const float& value);
    
    float getShadowPricePlus(const int& index)const;
    float getShadowPriceMinus(const int& index)const;
    
    void printShadowPricePlus() const;
    
    void printShadowPriceMinus() const;
    
private:
    row_t shadowPricesPlus;
    row_t shadowPricesMinus;
    int sp_size;
};


#endif
