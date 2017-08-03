//
//  Cluster.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/5/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "Cluster.h"
#include "Cost.h"
#include "Output.h"
#include "kmeans.h"
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"


void Cluster::initialize(const Cost& distance)
{
    initializeByKmeans(distance);
}

void Cluster::initialize(const std::string& f_fileName)
{
    try {
        initializeByInput(f_fileName);
    } catch (...) {
        std::cout<<"Empty file of clusters"<<'\n';
    }

}

int Cluster::getNumberLocationsInCluster(const int& clusterIndex) const
{
    return (int)getLocationsForCluster(clusterIndex).size();
}

const Cluster& Cluster::operator=(const Cluster& newCluster)
{
    for (int i=0; i<newCluster.numberOfClusters; i++) {
        for (int j=0; j<(int)newCluster.cluster[i].size(); j++) {
            this->cluster[i].push_back(newCluster.cluster[i][j]);
        }

    }
    return * this;
}

vector_t Cluster::getLocationsForCluster(const int& clusterIndex) const
{
	return cluster[clusterIndex];
}

int Cluster::getNumberOfClusters() const
{
    return numberOfClusters;
}

void Cluster::output(const Cost& distance)
{
    output::outputClustersMap(distance,cluster); 
}

void Cluster::initializeByInput(const std::string& f_fileName)
{
    std::string line;
    std::ifstream inputFile(f_fileName.c_str());
    
    int clusterCounter=0;
    while(std::getline(inputFile, line))
    {
        std::vector<float> inputVector;
        std::istringstream iss(line);
        float value;
        while (iss >> value)
        {
            inputVector.push_back(value);
        }
        if ((int)inputVector.size()>1) {
            for (int i=1; i<(int)inputVector.size(); i++) {
                cluster[inputVector[0]].push_back(inputVector[i]);
            }
            std::sort (cluster[inputVector[0]].begin(), cluster[inputVector[0]].end());  
        }

        
        clusterCounter++;
    }
    if ((clusterCounter>1)&&(clusterCounter<numberOfClusters))
        perror ("Error in the input. Check inputClusters file");
        

    inputFile.close();
}

//TODO own kmeans implementation
void Cluster::initializeByKmeans(const Cost& distance)
{
    int numberOfLocations=distance.getSize();
    vector_t clusterVector(numberOfLocations);
    
    int clusterCount = numberOfClusters;
    int dimensions = 2;
    int sampleCount = numberOfLocations;
    
    cv::Mat points(sampleCount,dimensions, CV_32F);
    cv::Mat labels;
    cv::Mat centers(clusterCount, 1, points.type());
    
    for(int i =0;i<points.rows;i++)
    {
        points.at<float>(i,0)=distance.getXCoordinates(i);
        points.at<float>(i,1)=distance.getYCoordinates(i);
    }
    
    kmeans(points, clusterCount, labels, cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0), 3, cv::KMEANS_PP_CENTERS, centers); 
    
    for (int i=0; i<numberOfLocations; i++) {
        clusterVector[i]=labels.at<int>(i);
    }
    
    for (int i=0; i<numberOfClusters; i++) {
        for (int j=0; j<numberOfLocations; j++) {
            if (clusterVector[j]==i) {
                cluster[i].push_back(j);
            }
        }
    }
    
}



