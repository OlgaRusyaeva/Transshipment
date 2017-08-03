//
//  ValueFunction.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//


#include "ValueFunction.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

/*ValueFunction::ValueFunction()
{
    initialize();
}
    
void ValueFunction::initialize()
{
    map<std::pair<int, size_t>, float> valueFunctionMap;
}*/
    
float ValueFunction::getValueFunction(const int& row_id, const size_t& col_id) const
{
    map<std::pair<int, size_t>, float>:: const_iterator mapIterator = valueFunctionMap.find(std::make_pair(row_id, col_id));
    
    if (mapIterator!=valueFunctionMap.end()) {
        return mapIterator->second;
    }
    else
        return 0;
    
    /*if (valueFunctionMap.count(boost::make_tuple(row_id, col_id,cluster_id))>0)
        return valueFunctionMap.find(boost::make_tuple(row_id, col_id,cluster_id))->second;
    else
        return 0;*/
    
    
}
    
void ValueFunction::setValueFunction(const int& row_id, const size_t& col_id, const float& value)
{
    valueFunctionMap[std::make_pair(row_id, col_id)]=value;
}
    
void ValueFunction::printVFunction(ofstream &file)const
{
    typedef map<std::pair<int, size_t>, float>:: const_iterator mapIterator;
    for (mapIterator iter=valueFunctionMap.begin(); iter!=valueFunctionMap.end(); iter++) {
        // time-hash_state_id-value
        file << iter->first.first<<" "<<iter->first.second<<" "<<iter->second<<'\n';
    }
    file<<"-------------newVF------------"<<'\n';
}
    
    
void ValueFunction::inputVFunction(ifstream &inputFile)
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
            size_t hash_id;
            float value;
            
            std::stringstream ss(line);
                
            ss>>time;
            ss>>hash_id;
            ss>>value;
                
            //valueFunctionMap[boost::make_tuple(time, hash_id,cluster)]=value;
            valueFunctionMap.insert(std::pair<std::pair<int, size_t>, float>(std::make_pair(time, hash_id),value));
                
        }
            
    }
        
}




const ValueFunction& ValueFunction::operator=(const ValueFunction& newValueFunction)
{
    /*typedef map<std::pair<int, size_t>, float>:: const_iterator mapIterator;
    for (mapIterator iter=newValueFunction.valueFunctionMap.begin(); iter!=newValueFunction.valueFunctionMap.end(); iter++) {
        this->setValueFunction(iter->first.first,iter->first.second,iter->second);
    }*/
    valueFunctionMap=newValueFunction.valueFunctionMap;
    return *this;
}


    
    