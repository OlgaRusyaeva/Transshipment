//
//  Header ActionsGenerator.cpp
//  


#ifndef project3_ActionsGenerator_h
#define project3_ActionsGenerator_h

#include "types.h"

class State;

namespace actions {
    void init(vector_t& v, const int& index, const int& i); // initialization of the first row 
    bool valid(const vector_t& v, const int& j);//validation if the sum of all elements of the vector is equal to the flag, flag in [0..N]
    bool last(const vector_t& v, const int& i, const vector_t& stock_out); // checking last element if it is equal to the flag return true and break cycle
    void next(vector_t& v, const int& i,vector_t stock_out);
    
    std::vector<matrixInt_t> transhipmentFromState(const State& state,const int& lifeTime, const bool& outOfStock);//return the vector of all possible transshipments(matrixes) from the state
    
    std::vector<matrixInt_t> transhipmentFromState(const State& state, const matrixInt_t& map);
};


#endif
