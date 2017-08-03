//
//  VectorsOperations.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 12/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_VectorsOperations_h
#define TransshipmentProject_VectorsOperations_h
#include "types.h"

namespace vectorsOperations {
    vector_t createVector(const int& value, const int& index, const int& length);
    vector_t sumOfVectors(const vector_t& v1, const vector_t& v2);
    
    vector_t merge_copy(const vector_t& a, const vector_t& b);
    vector_t copy_no_sort(const vector_t& c);
};




#endif
