//
//  Graph.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 1/16/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "Graph.h"

#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

using namespace std;

void Graph::setEdge(const int& vBeg_id, const int& vEnd_id, const int& degree, const int& edge_id)
{
    graph[boost::make_tuple(vBeg_id,vEnd_id,degree)]=edge_id;
}

int Graph::getEdge(const int& vBeg_id, const int& vEnd_id, const int& degree)const
{
    
    map<boost::tuple<int, int,int>, int>:: const_iterator mapIterator = graph.find(boost::make_tuple(vBeg_id, vEnd_id,degree));
    
    if (mapIterator!=graph.end()) {
        return mapIterator->second;
    }
    else
    {
        cout<<"Error to get edge"<<'\n';
        return 100;
    }
}

void Graph::initializeGraph()
{
    int t=0;
    for (int i=0; i<sourcesV; i++) {
        for (int j=0; j<sourcesV; j++) {
            int tau=linksAllowed[i][j];
            setEdge(i,sourcesV*(tau+1)+j,0,t);
            t++;
        }
    }
    
    for (int i=0; i<sourcesV; i++) {
        for (int tau=0; tau<repeat; tau++) {
            for (int j=0; j<parallelEdges[i*repeat+tau]; j++) {
                setEdge(sourcesV*(tau+1)+i,(repeat+1)*sourcesV,j,t);
                t++;
            }
        }

    }
    
    for (int i=0; i<sourcesV; i++) {
        for (int tau=0; tau<repeat; tau++) {
            setEdge(sourcesV*(tau+1)+i,(repeat+1)*sourcesV,parallelEdges[i*repeat+tau],t);
            t++;
        }

    }
}

//May be easier search in map?
map<int,int> Graph::getAdjacentVerteces(const int& n)
{
    map<int,int> H_n;
    if (n==(repeat+1)*sourcesV) {
        for (int i=0; i<sourcesV; i++) {
            for (int t=0; t<repeat; t++) {
                H_n[sourcesV+t*sourcesV+i]=parallelEdges[i*repeat+t];
            }
        }
    }
    else
    {
        if ((n>=sourcesV)&&(n<(repeat+1)*sourcesV)) {
            int t=(unsigned)(n-sourcesV)/sourcesV;
            int i=n-sourcesV-t*sourcesV;
            
            for (int j=0; j<sourcesV; j++) {
                if (linksAllowed[i][j]==t) {
                    H_n[j]=0;
                }
                
            }
        }

    }
    
    return H_n;
}

map<int,int> Graph::getInvAdjacentVerteces(const int& n)
{
    map<int,int> G_n;
    if (n<sourcesV) {
        for (int i=0; i<sourcesV; i++) {
            int t=linksAllowed[n][i];
            G_n[sourcesV+t*sourcesV+i]=0;
        }
    }
    else
    {
        if ((n>=sourcesV)&&(n<(repeat+1)*sourcesV)) {
        	int t=(unsigned)(n-sourcesV)/sourcesV;
        	int i=n-sourcesV-t*sourcesV;
            
        	G_n[(repeat+1)*sourcesV]=parallelEdges[i*repeat+t];
        }

    }
    return G_n;
}

/*void Graph::setFlow()
{
    for (int i=0; i<(int)flow.size(); i++) {
        flow[i]=vars[i].get(GRB_DoubleAttr_X);
    }
}

void Graph::setWeight()
{
    for (int i=0; i<(int)weight.size(); i++) {
        weight[i]=vars[i].get(GRB_DoubleAttr_Obj);
    }
}

void Graph::setCapacity()
{
    for (int i=0; i<(int)capacity.size(); i++) {
        capacity[i]=vars[i].get(GRB_DoubleAttr_UB);
    }
}*/

int Graph::getFlow(const int& edge_id)const
{
 	return flow[edge_id];   
}

float Graph::getWeight(const int& edge_id)const
{
    return weight[edge_id];
}

int Graph::getCapacity(const int& edge_id)const
{
    return capacity[edge_id];
}


/*void Graph::getSourceNodes(map<int,int>& sourceNodes, const int& edge_id)
{
    int time=(unsigned)(edge_id-sourcesV)/sourcesV;
    int location=edge_id-sourcesV-integ*sourcesV;
    
    for (int i=0; i<sourcesV; i++) {
        if (linksAllowed[location][i]==time) {
            sourceNodes[i]=0;
        }

    }
    
}

void Graph::getTransshipmentNodes(map<int,int>& transshipmentNodes, const int& edge_id)
{
    for (int i=0; i<sourcesV; i++) {
        for (int t=0; t<repeat; t++) {
            if (edge_id>=0) {
                transshipmentNodes[sourcesV+t*sourcesV+i]=parallelEdges[i*repeat+t];
            }
            else
                //if there is a link
                transshipmentNodes[sourcesV+t*sourcesV+i]=0;
        }

    }
}

void Graph::getSinkNode(map<int,int>& sinkNode, const int& edge_id)
{
    int integ=(unsigned)(edge_id-sourcesV)/sourcesV;
    int frac=edge_id-sourcesV-integ*sourcesV;
    
    sinkNode[(repeat+1)*sourcesV]=parallelEdges[frac*repeat+integ];

}*/


void Graph::printGraph()
{
    cout<<"parallel edges"<<'\n';
    for (int i=0; i<(int)parallelEdges.size(); i++) {
        cout<<parallelEdges[i]<<'\n';
    }
	printMap();
    printAdjacentVerteces();
    printInvAdjacentVerteces();
}

void Graph::printMap()
{
    cout<<"EdgeID: VerBegID; VerEndID; Degree"<<'\n';
    typedef map<boost::tuple<int, int,int>, int>:: const_iterator mapIterator;
    for (mapIterator iter=graph.begin(); iter!=graph.end(); iter++) {
        cout << iter->second<<" : "<<iter->first.get<0>()<<" ; "<<iter->first.get<1>()<<" ; "<<iter->first.get<2>()<<'\n';
        cout<<"weight: "<<weight[iter->second]<<" ; "<<"capacity: "<<capacity[iter->second]<<" ; "<<"flow: "<<flow[iter->second]<<'\n';
    }
    
}

void Graph::printAdjacentVerteces()
{
    cout<<"Adjacent verteces:"<<'\n';
	map<int,int> H_n;
    typedef map<int,int>:: const_iterator iter;
    for (int i=0; i<nVertex; i++) {
        cout<<"i: "<<i<<'\n';
        H_n=getAdjacentVerteces(i);
        for (iter it=H_n.begin(); it!=H_n.end(); it++) 
        {
            cout<<it->first<<" - "<<it->second<<" ; ";
        }
        cout<<'\n';
        
    }
}

void Graph::printInvAdjacentVerteces()
{
    cout<<"Inv Adjacent verteces:"<<'\n';
    map<int,int> G_n;
    typedef map<int,int>:: const_iterator iter;
    for (int i=0; i<nVertex; i++) {
        cout<<"i: "<<i<<'\n';
        G_n=getInvAdjacentVerteces(i);
        for (iter it=G_n.begin(); it!=G_n.end(); it++) 
        {
            cout<<it->first<<" - "<<it->second<<" ; ";
        }
        cout<<'\n';
        
    }
}