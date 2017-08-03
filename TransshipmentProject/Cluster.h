//
//  Cluster.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/5/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Cluster_h
#define TransshipmentProject_Cluster_h

#include "types.h"

class Cost;

class Cluster{
    
public:
    Cluster(int nClusters):numberOfClusters(nClusters){cluster=matrixInt_t(nClusters);}
    
    ~Cluster(){}
    
    void initialize(const Cost& distance);
    
    void initialize(const std::string& f_fileName);
    
    const Cluster& operator=(const Cluster& newCluster);
    
    vector_t getLocationsForCluster(const int& clusterIndex) const;
    
	int getNumberOfClusters() const;
    
    int getNumberLocationsInCluster(const int& clusterIndex) const;
    
    void output(const Cost& distance);
      
    
    
private:
    
    int numberOfClusters;
    
    //clusterID - vector of locations ID inside cluster
    matrixInt_t cluster;
    
    void initializeByInput(const std::string& f_fileName);
    
    void initializeByKmeans(const Cost& distance);
    

};


#endif
