//
//  test.cpp
//  TransshipmentProject
//
//  Created by Olga on 09/12/15.
//
//

#include "test.h"
#include "PLValueFunction.h"

void Test::checkConcaveUpdate()
{
    int time=1;
    int location=1;
    int leadTime=1;
    
    PLValueFunction VF(time,location,leadTime);
    
    /*for (int l=0; l<leadTime; l++) {
        for (int t=0; t<time; t++) {
            for (int i=0; i<location; i++) {
                int maxIndex=5;
                int slope=20;
                VF.get(l,t,i).setSlope(0,slope);
                for (int j=1; j<maxIndex; j++) {
                    VF.get(l,t,i).addSlope(slope-j*2);
                    VF.get(l,t,i).addBreakpoint(j+1);
                    VF.get(l,t,i).addIndex(1);
                }
            }
        }
    }*/
    
    VF.get(0,0,0).setSlope(0,64.9);
    
    VF.get(0,0,0).addSlope(64.5);
    VF.get(0,0,0).addBreakpoint(2);
    VF.get(0,0,0).addIndex(1);
    
   	VF.get(0,0,0).addSlope(42.5);
    VF.get(0,0,0).addBreakpoint(3);
    VF.get(0,0,0).addIndex(1);
    
    VF.get(0,0,0).addSlope(41.8);
    VF.get(0,0,0).addBreakpoint(5);
    VF.get(0,0,0).addIndex(1);
    
    VF.get(0,0,0).addSlope(41.8);
    VF.get(0,0,0).addBreakpoint(6);
    VF.get(0,0,0).addIndex(1);
    
    int newSlopeMinus=80;
    int newSlopePlus=80;
    //VF.updateSlopeCave(0,0,0,newSlopePlus,newSlopeMinus,3,location,4,1);
    //VF.updateSlopeLeveling(0,0,0,newSlopePlus+1,newSlopeMinus+1,6,3);
    VF.updateSlopeSpar(0,0,0,newSlopePlus-40,newSlopeMinus+10,4,3);
    
    
}