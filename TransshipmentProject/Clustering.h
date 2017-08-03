//
//  Clustering.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Clustering_h
#define TransshipmentProject_Clustering_h

#include "types.h"

class State;


namespace clustering {
    
    const State getClusteredState(const State& state,const int& numberOfClusters, const vector_t& clusters);
    
    vecMatrixInt generateActionsForEachLocation(const list_t& requiredQuantity,const list_t& availableQuantity,const int& NumberOfLocations);
};


#endif
