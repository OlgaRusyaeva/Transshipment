//
//  VectorActions.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 12/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "VectorsOperations.h"

namespace vectorsOperations{
    vector_t createVector(const int& value, const int& index, const int& length)
    {
        vector_t newVector = vector_t(length+1);
        newVector[index]=value;
        newVector[length]=value;
        return newVector; 
    }
    
    vector_t sumOfVectors(const vector_t& v1, const vector_t& v2)
    {
        vector_t v3=vector_t((int)v1.size());
        for (int i=0; i<(int)v1.size(); i++) {
            v3[i]=v1[i]+v2[i];
        }
        return v3;
    }
    
    vector_t merge_copy(const vector_t& a, const vector_t& b) {
        vector_t ret(b);
        copy(a.begin(), a.end(), back_inserter(ret));
        return ret;
    }
    
    vector_t copy_no_sort(const vector_t& c) {
        vector_t ret;
        for (vector_t::const_iterator i = c.begin(); i != c.end(); ++i) {
            if (find(ret.begin(), ret.end(), *i) == ret.end()) {
                ret.push_back(*i);
            }
        }
        return ret;
    }

}

