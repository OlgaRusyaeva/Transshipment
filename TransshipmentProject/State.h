//
//  State = vector of inventories 
//  


#ifndef project2_State_h
#define project2_State_h

#include "types.h"


class State{

public:
    State(int length, int depth, int width);
    
    State(int length, int depth);
    
    State(int length);
    
    ~State(){}
    
    const State& operator=(const State& newState);
    
    //void setInitialState(const int& id_l, const int& id_d, const int& value);
    
    void setElement(const int& id_l,const int& id_d, const int& value);
    
    matrixInt_t getState() const;
    
    vector_t getCurrentPeriodState() const;
    
    int getElement(const int& id_l, const int& id_d)const;
    
    int getLength() const;
    
    int getDepth() const;
    
    int getWidth() const;
    
    void print() const;
    
    void inputState(std::ifstream& inputFile);
    
    void outputState(std::ofstream& outputFile) const;
    
    //friend class Hashing;
    
private:
    
    int sizeD;
    int sizeW;
    int sizeL;
    matrixInt_t state;
    

};

#endif
