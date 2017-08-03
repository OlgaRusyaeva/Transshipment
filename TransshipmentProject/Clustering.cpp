//
//  Clustering.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "Clustering.h"
#include "State.h"
#include "VectorsOperations.h"
#include "NumbersOperations.h"

using namespace vectorsOperations;
using namespace numbersOperations;

namespace clustering {
    vecMatrixInt generateActionsForEachLocation(const list_t& requiredQuantity,const list_t& availableQuantity,const int& NumberOfLocations)
    {
        vecMatrixInt actions = vecMatrixInt(NumberOfLocations);//availableQuantity.size()
        for (int i=0; i<NumberOfLocations; i++) {//availableQuantity.size()
            actions[i].push_back(vector_t(NumberOfLocations+1));
        }
        
        typedef list_t::const_iterator list_iter;
        
        int min=0;
        int size=0;
        int w=0;
        
        for (list_iter k=availableQuantity.begin(); k!=availableQuantity.end(); k++) {
            for (list_iter j=requiredQuantity.begin(); j!=requiredQuantity.end(); j++) {
                min = getMinValue((*k).second, (*j).second);
                for (int p=min; p>0; p--) {//(floor)(getMinValue((*k).second, (*j).second)/2)
                    actions[(*k).first].push_back(createVector(p,(*j).first,NumberOfLocations)); 
                }
            }
            size = (int)actions[(*k).first].size();
            for (int i=1; i<size; i++) {
                w = actions[(*k).first][i][NumberOfLocations];
                if (w<(*k).second) {
                    for (int p=size-1; p>=i+w; p--) {
                        if ((w+actions[(*k).first][p][NumberOfLocations])<=(*k).second) {
                            actions[(*k).first].push_back(sumOfVectors(actions[(*k).first][i],actions[(*k).first][p]));
                        }
                    }
                }
                
            }
            
        }
        return actions;
    }
    
    const State getClusteredState(const State& state,const int& numberOfClusters, const vector_t& clusters)
    {
        const int L=(int)state.getLength();
        State newState(numberOfClusters);
        for (int i=0; i<L; i++) {
            for (int j=0; j<numberOfClusters; j++) {
                if (j==clusters[i]) {
                    newState.setElement(j, newState.getElement(j)+state.getElement(i));
                }
            }
        }
        
        return newState;
    }
}


