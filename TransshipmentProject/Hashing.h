//
//  Hash of the state(vector of inventories) 
//  For the storage of the value function


#ifndef project2_Hashing_h
#define project2_Hashing_h

#include <stddef.h>

class State;


class Hashing
{
    
public:
    Hashing(){}
    ~Hashing(){}
    
    size_t hash_value(const State& st) const;
    
    //const Hashing& operator=(const Hashing& newHash);
    
};

#endif
