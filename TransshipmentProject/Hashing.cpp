//
//  Hashing.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "Hashing.h"
#include "State.h"
#include <boost/functional/hash.hpp>

size_t Hashing::hash_value(const State& st)const
{
    int b=0;
    if (st.getWidth()>1) {
        b=st.getWidth();
    }
    else
        b=st.getDepth();
    
    size_t seed=0;
    for (int i=0; i<st.getLength(); i++) {
        for (int t=0; t<b; t++) {
            boost::hash_combine(seed, st.getElement(i,t)*2654435761);
        }
    }
    
    return seed;
}

