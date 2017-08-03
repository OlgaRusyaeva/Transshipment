//
//  Graph.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 1/16/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Graph_h
#define TransshipmentProject_Graph_h

#include "types.h"
#include <map>
#include "boost/tuple/tuple.hpp"
#include "LeadTimes.h"



class LP_optimizer;

class Graph {
public:
    Graph(int numberEdges, int numberVerteces, row_t vectorW, vector_t vectorF, vector_t vectorC, int numberSouceVerteces, int numberSourceVertecesRepeat, vector_t vectorParallelEdges,matrixInt_t links):nEdge(numberEdges), nVertex(numberVerteces)
    {
        //capacity=vector_t(nEdge);
        //weight=row_t(nEdge);
        //flow=vector_t(nEdge);
        capacity=vectorC;
        weight=vectorW;
        flow=vectorF;
        sourcesV=numberSouceVerteces;
        repeat=numberSourceVertecesRepeat;
        parallelEdges=vectorParallelEdges;
        linksAllowed=links;
    }
    ~Graph(){}
    
    void initializeGraph();
    void setFlow();
    void setWeight();
    void setCapacity();
    //row_t getPotential(const bool& flag);
    std::map<int,int> getAdjacentVerteces(const int& n);
    std::map<int,int> getInvAdjacentVerteces(const int& n);
    int getFlow(const int& edge_id)const;
    float getWeight(const int& edge_id)const;
    int getCapacity(const int& edge_id)const;
    void setEdge(const int& vBeg_id, const int& vEnd_id, const int& degree, const int& edge_id);
    int getEdge(const int& vBeg_id, const int& vEnd_id, const int& degree)const;
    int numberE()const{return nEdge;}
    int numberV()const{return nVertex;}
    
    void printGraph();
    
    
private:
    int nEdge;
    int nVertex;
    
    int sourcesV;
    int repeat;
    vector_t parallelEdges;
    
    vector_t capacity;
    row_t weight;
    vector_t flow;
    
    matrixInt_t linksAllowed;
    
    std::map<boost::tuple<int,int,int>,int> graph;
    
    //void getSourceNodes(std::map<int,int>& sourceNodes);
    //void getTransshipmentNodes(std::map<int,int>& transshipmentNodes, const int& edge_id);
    //void getSinkNode(std::map<int,int>& sinkNode, const int& edge_id);
    
	//vector_t getTransshipmentNodes(vector_t& degree, const int& edge_id);
    //vector_t getSourceNodes(vector_t& degree);
    
    void printMap();
    void printAdjacentVerteces();
    void printInvAdjacentVerteces();
    
};


#endif
