//
//  ValueFunctionClustered.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/5/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "ValueFunctionClustered.h"
#include <fstream>
#include "Actions.h"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

using namespace std;



/*ValueFunctionClustered::ValueFunctionClustered()
{
    initialize();
}

void ValueFunctionClustered::initialize()
{
    map<boost::tuple<int, size_t,int>, float> valueFunctionMap;
}*/

float ValueFunctionClustered::getValueFunction(const int& row_id, const size_t& col_id, const int& cluster_id) const
{
    map<boost::tuple<int, size_t,int>, float>:: const_iterator mapIterator = valueFunctionMap.find(boost::make_tuple(row_id, col_id,cluster_id));
    
    if (mapIterator!=valueFunctionMap.end()) {
        return mapIterator->second;
    }
    else
        return 0;
    
    
}

void ValueFunctionClustered::setValueFunction(const int& row_id, const size_t& col_id, const int& cluster_id,const float& value)
{
    valueFunctionMap[boost::make_tuple(row_id, col_id,cluster_id)]=value;
}

void ValueFunctionClustered::printVFunction(ofstream &file)const
{
    typedef map<boost::tuple<int, size_t,int>, float>:: const_iterator mapIterator;
    for (mapIterator iter=valueFunctionMap.begin(); iter!=valueFunctionMap.end(); iter++) {
        // time-hash_state_id-cluster-value
        file << iter->first.get<0>()<<" "<<iter->first.get<1>()<<" "<<iter->first.get<2>()<<" "<<iter->second<<'\n';
    }
}


void ValueFunctionClustered::inputVFunction(ifstream &inputFile)
{
    valueFunctionMap.clear();
    //string f_fileName="/Users/orusyaeva/Documents/ADP_Project/ADP/TransshipmentProject/TransshipmentProject/VF_input";
    string line;
    //ifstream inputFile(f_fileName.c_str());
    if (inputFile == NULL) perror ("Error opening file.");
    else
    {
        while(std::getline(inputFile, line))
        {
            if (line=="") {
                continue;
            }
            int time;
            int cluster;
            size_t hash_id;
            float value;
            
            std::stringstream ss(line);
            
            ss>>time;
            ss>>hash_id;
            ss>>cluster;
            ss>>value;
            
            //valueFunctionMap[boost::make_tuple(time, hash_id,cluster)]=value;
            valueFunctionMap.insert(std::pair<boost::tuple<int, size_t,int>, float>(boost::make_tuple(time, hash_id,cluster),value));
            
        }
        
    }
    inputFile.close();
    
}


const ValueFunctionClustered& ValueFunctionClustered::operator=(const ValueFunctionClustered& newValueFunction)
{
    /*typedef map<boost::tuple<int, size_t,int>, float>:: const_iterator mapIterator;
    for (mapIterator iter=newValueFunction.valueFunctionMap.begin(); iter!=newValueFunction.valueFunctionMap.end(); iter++) {
        this->setValueFunction(iter->first.get<0>(),iter->first.get<1>(),iter->first.get<2>(),iter->second);
    }*/
    valueFunctionMap=newValueFunction.valueFunctionMap;
    return *this;
}


