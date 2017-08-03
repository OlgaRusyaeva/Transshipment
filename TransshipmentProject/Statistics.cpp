//
//  Statistics.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/6/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Statistics.h"

using namespace std;

row_t getStatistics(row_t data)
{
    row_t output=row_t(5);
    int size=(int)data.size();
    float sum=0;
    float squareSum=0;
    
    std:sort(data.begin(),data.end());
    
    output[0]=data[0];
    output[4]=data[size-1];
    output[1]=data[(ceil)((size+1)*0.25)];
    output[3]=data[(floor)((size+1)*0.75)];
    
    for (int i=0; i<size; i++) 
    {
        sum+=data[i];
        squareSum+=pow(data[i], 2);
    }
    output[2]=sum/size;
    
    cout<<"mean: "<<output[2]<<'\n';
    cout<<"sd: "<<sqrtf(squareSum/size-pow(output[2], 2))<<'\n';
    
                    
   return output;
}

row_t getComparison(row_t data1, row_t data2)
{
    row_t merge_data;
    for (int i=0; i<data1.size(); i++) {
        merge_data.push_back(data1[i]-data2[i]);
    }
    return getStatistics(merge_data);
}



