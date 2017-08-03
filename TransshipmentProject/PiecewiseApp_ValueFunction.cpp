//
//  PiecewiseApp_ValueFunction.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/7/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "PiecewiseApp_ValueFunction.h"
#include <algorithm>

PiecewiseApp_ValueFunction::PiecewiseApp_ValueFunction(int time, int location):indexA(time),indexB(location)
{
    initialize();
}

void PiecewiseApp_ValueFunction::initialize()
{
    indexVector=matrixInt_t(indexA,vector_t(indexB));
    breakpoints = vecMatrixInt(indexA,matrixInt_t(indexB));
    slope= vecMatrixFloat(indexA,matrix_t(indexB));
    
    setInitialValue();
    
}

void PiecewiseApp_ValueFunction::setInitialValue()
{
    for (int t=0; t<indexA; t++) {
        for (int i=0; i<indexB; i++) {
            indexVector[t][i]=1;
            breakpoints[t][i].push_back(0);
            slope[t][i].push_back(0);
        }
    }
}


int PiecewiseApp_ValueFunction::getIndexVectorElement(const int& x_id, const int& y_id)const
{
    return indexVector[x_id][y_id];
}

int PiecewiseApp_ValueFunction::getBreakpointsElement(const int& x_id, const int& y_id, const int& z_id)const
{
    return breakpoints[x_id][y_id][z_id];
}

float PiecewiseApp_ValueFunction::getSlopeElement(const int& x_id, const int& y_id, const int& z_id)const
{
    //if there is no such breakpoint we need to return the slope of the nearest lowest breakpoint
    return slope[x_id][y_id][z_id];
}

void PiecewiseApp_ValueFunction::setIndexVectorElement(const int& x_id, const int& y_id, const int& value)
{
    indexVector[x_id][y_id]=value;
}

void PiecewiseApp_ValueFunction::setBreakpointsElement(const int& x_id, const int& y_id, const int& z_id, const int& value)
{
    breakpoints[x_id][y_id][z_id]=value;
}

void PiecewiseApp_ValueFunction::setSlopeElement(const int& x_id, const int& y_id, const int& z_id, const float& value)
{
    slope[x_id][y_id][z_id]=value;
}

vector_t PiecewiseApp_ValueFunction::getIndexVector(const int& index) const
{
    return indexVector[index];
}

matrixInt_t PiecewiseApp_ValueFunction::getBreakpoints(const int& index) const
{
    return breakpoints[index];
}

matrix_t PiecewiseApp_ValueFunction::getSlopes(const int& index) const
{
    return slope[index];
}

void PiecewiseApp_ValueFunction::insertBreakpointsElement(const int& x_id, const int& y_id, const int& pos, const int& value)
{
    breakpoints[x_id][y_id].insert(breakpoints[x_id][y_id].begin()+pos,value);
}

void PiecewiseApp_ValueFunction::insertSlopeElement(const int& x_id, const int& y_id, const int& pos, const float& value)
{
    slope[x_id][y_id].insert(slope[x_id][y_id].begin()+pos,value);
}

void PiecewiseApp_ValueFunction::push_backBreakpointsElement(const int& x_id, const int& y_id, const int& value)
{
    breakpoints[x_id][y_id].push_back(value);
}

void PiecewiseApp_ValueFunction::push_backSlopeElement(const int& x_id, const int& y_id, const float& value)
{
    slope[x_id][y_id].push_back(value);
}

vector_t PiecewiseApp_ValueFunction::getBreakpointsRow(const int& x_id, const int& y_id)const
{
    return breakpoints[x_id][y_id];
}

row_t PiecewiseApp_ValueFunction::getSlopeRow(const int& x_id, const int& y_id)const
{
    return slope[x_id][y_id];
}

//CHECK
const PiecewiseApp_ValueFunction& PiecewiseApp_ValueFunction::operator=(const PiecewiseApp_ValueFunction& newApproximation)
{
    for (int t=0; t<indexA; t++) {
        /*for (int i=0; i<indexB; i++) {
            indexVector[t][i]=newApproximation.getIndexVectorElement(t,i);
            for (int j=0; j<(int)newApproximation.getBreakpointsRow(t,i); j++) {
         		breakpoints[t][i].push_back(newApproximation.getBreakpointsElement(t,i,j));
            }
         	for(int j=0; j<(int)newApproximation.getSlopeRow(t,i));j++)
            {
         		slope[t][i].push_back(newApproximation.getSlopeElement(t,i,j));
         	}
        }*/
        indexVector[t]=newApproximation.getIndexVector(t);
        breakpoints[t]=newApproximation.getBreakpoints(t);
        slope[t]=newApproximation.getSlopes(t);
    }
    
    return *this;
}

int PiecewiseApp_ValueFunction::getBreakpointIndexClosestToElement(const int& x_id, const int& y_id, const int& elementToCheck)
{
    if (elementToCheck==0) {
        return 0;
    }
    else
    {
        if (elementToCheck==breakpoints[x_id][y_id][elementToCheck]) {
            return elementToCheck;
        }
        else
        {
    		vector_t::iterator low;
    		low=std::lower_bound(breakpoints[x_id][y_id].begin(),breakpoints[x_id][y_id].end(),elementToCheck);
    		return low-breakpoints[x_id][y_id].begin()-1;
        }
    }
}

void PiecewiseApp_ValueFunction::update(const int& t, const int& loc,  const float& new_slopePlus, const float& new_slopeMinus, const int& pos_A, const int& pos_B, const int& currentState, const float& alpha)
{
    for (int i=pos_A; i<pos_B; i++) {
        if (getBreakpointsElement(t,loc,i)<currentState) {
            setSlopeElement(t,loc,i, (1-alpha)*getSlopeElement(t,loc,i)+alpha*new_slopeMinus);
        }
        else
            setSlopeElement(t,loc,i, (1-alpha)*getSlopeElement(t,loc,i)+alpha*new_slopePlus);
    }
}

void PiecewiseApp_ValueFunction::insert(const int& t, const int& loc, const int& new_breakpoint, int& increase_index, int& pos)
{
    
    int k=getIndexVectorElement(t,loc);
    vector_t u=getBreakpointsRow(t,loc);
    row_t v=getSlopeRow(t,loc);
    
    for (int i=0; i<k+increase_index; i++) {
        if (u[i]==new_breakpoint) {
            pos=i;
            break;
        }
        else
        {
            if (u[i]>new_breakpoint) {
                insertBreakpointsElement(t,loc,i,new_breakpoint);
                insertSlopeElement(t,loc,i,v[i-1]);
                pos=i;
                increase_index++;
                break;
                
            }
            else
            {
                if (i==(k+increase_index)-1) {
                    push_backBreakpointsElement(t,loc,new_breakpoint);
                    push_backSlopeElement(t,loc,v[i]);
                    pos=i+1;
                    increase_index++;
                    break;
                }
            }
        }
    }
}




//Check the concavity for big size problems
void PiecewiseApp_ValueFunction::updateSlopeCave(const int& t, const int& loc, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& L, const int& iteration, const int& step)
{
    float alpha=(float)5/(5+iteration);
    
    int left=0;
    int right=0;
    
    int increment=0;
    int pos_a=0;
    int pos_b=0;
    int pos_c=0;
    
    insert(t,loc, inventory,increment,pos_b);
    setIndexVectorElement(t,loc,getIndexVectorElement(t,loc)+increment);
    
    //find min_k where the concave property will be broken
    //if the left side of min update interval<0 then pos_a=0
    if (inventory-step>=0) {
        
        int min_k=-1;
        
        for (int j=0; j<pos_b; j++) {
            if (getSlopeRow(t,loc)[j]<=(1-alpha)*getSlopeRow(t,loc)[j+1]+alpha*gradientMinus) {
                min_k=j;
                break;
            }
        }
        
        //if min_k doesn't exist OR min{inventory-step,u[min_k]}=invnetory-step
        if ((min_k<0)||(inventory-step<getBreakpointsRow(t,loc)[min_k])) {
            //take the left side of min update interval
            left=inventory-step;
        }
        else
        {
            //take the left side the breakpoint of min_k
            left=getBreakpointsRow(t,loc)[min_k];
        }
        
        increment=0;
        //insert breakpoint if it is required and define the left position for the update
        insert(t,loc, left,increment, pos_a);
        
        setIndexVectorElement(t,loc,getIndexVectorElement(t,loc)+increment);
        
    }
    pos_b=pos_b+increment;
    pos_a=pos_a-increment;
    //find max_k where the concave property will be broken
    int max_k=-1;
    
    for (int j=getIndexVectorElement(t,loc)-1; j>=pos_b; j--) {
        if ((j-1>=0)&&(1-alpha)*getSlopeRow(t,loc)[j-1]+alpha*gradientPlus<=getSlopeRow(t,loc)[j]) {
            max_k=j;
            break;
        }
    }
    
    if (max_k+1>=getIndexVectorElement(t,loc)) {
        right=-1;
    }
    else
    {
        //define the right side of the update interval
        if ((max_k<0)||(inventory+step>getBreakpointsRow(t,loc)[max_k+1])) {
            right=inventory+step;
        }
        else
        {
            right=getBreakpointsRow(t,loc)[max_k+1];
        }
    }
    
    //insert breakpoint if it is required and define right position for the update interval
    if (right>0) {
        increment=0;
        insert(t,loc, right,increment,pos_c);
        setIndexVectorElement(t,loc,getIndexVectorElement(t,loc)+increment);
    }
    else
        pos_c=getIndexVectorElement(t,loc);
    
    update(t,loc, gradientPlus, gradientMinus, pos_a, pos_c, inventory, alpha);
    
    
    //to check concavity
    /*row_t v=getSlopeRow(t,loc);
     
     if (getIndexVectorElement(t,loc)>0) {
     	for (int i=1; i<getIndexVectorElement(t,loc); i++) {
     		if (v[i-1]<v[i]) {
     			std::cout<<"concavity is broken"<<'\n';
     		}
        }
     }*/
    
}

void PiecewiseApp_ValueFunction::updateSlopeLeveling(const int& t, const int& loc, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& iteration)
{
 
    float alpha=(float)5/(5+iteration);
    
    int increment=0;
    int pos=0;
    
    //insert breakpoint if it is required
    insert(t,loc, inventory,increment,pos);
    
    //add new indexes
    setIndexVectorElement(t,loc,getIndexVectorElement(t,loc)+increment);
    
    //update slopes around breakpoint
    if (pos>0) {
        update(t,loc, gradientPlus, gradientMinus, pos-1, pos+1, inventory, alpha);

        
        //if concavity is broken before new breakpoint, set the broken slope equal to the left slope of new breakpoint
        for (int i=pos-1; i>=0; i--) {
        	if (getSlopeRow(t,loc)[i]<getSlopeRow(t,loc)[pos-1])
            {
                setSlopeElement(t,loc,i, getSlopeRow(t,loc)[pos-1]);
            }
        }
    }
	else
            update(t,loc, gradientPlus, gradientMinus, pos, pos+1, inventory, alpha);
    
    
    //if concavity is broken after new breakpoint, set the broken slope equal to the right slope of new breakpoint
    for (int j=pos; j<getIndexVectorElement(t,loc); j++) {
        if (getSlopeRow(t,loc)[j]>getSlopeRow(t,loc)[pos]) {
            setSlopeElement(t,loc,j, getSlopeRow(t,loc)[pos]);
        }
    }
    
    
    //to check concavity
    /*row_t v=getSlopeRow(t,loc);
     
     if (getIndexVectorElement(t,loc)>0) {
     	for (int i=1; i<getIndexVectorElement(t,loc); i++) {
     		if (v[i-1]<v[i]) {
     			std::cout<<"concavity is broken"<<'\n';
     		}
        }
     }*/

    
}

//Check the concavity for big size problems
/*void PiecewiseApp_ValueFunction::updateSlopeSpar(const int& t, const int& loc, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& iteration)
{
	float alpha=(float)5/(5+iteration);
    
    int increment=0;
    int pos=0;
    int pos_l=0;
    int pos_r=0;

    float leftUpdateSlope=0;
    float rightUpdateSlope=0;
    
    //insert breakpoint if it is required
    insert(t,loc, inventory,increment,pos);
    
    //add new indexes
    setIndexVectorElement(t,loc,getIndexVectorElement(t,loc)+increment);
    
    //update slopes around breakpoint
    if (pos>0)
    {
        update(t,loc, gradientPlus, gradientMinus, pos-1, pos+1, inventory, alpha);
        
        
        if ((pos-2>=0)&&(getSlopeRow(t,loc)[pos-2]<getSlopeRow(t,loc)[pos-1]))
        {
        	findUpdateIntervalLeft(leftUpdateSlope,pos-1,pos_l,getSlopeRow(t,loc));
            pos_r=pos;
        	if (leftUpdateSlope<getSlopeRow(t,loc)[pos]) {
            	findUpdateIntervalLeft(leftUpdateSlope,pos,pos_l,getSlopeRow(t,loc));
                pos_r=pos+1;
        	}
            
            for (int i=pos_l; i<pos_r; i++) {
                setSlopeElement(t,loc,i, leftUpdateSlope);
            }
        }
        
    }
    else
    {
        update(t,loc, gradientPlus, gradientMinus, pos, pos+1, inventory, alpha);
        
    }
    
    
    if ((pos+1<=getIndexVectorElement(t,loc)-1)&&(getSlopeRow(t,loc)[pos]<getSlopeRow(t,loc)[pos+1])) {

    	findUpdateIntervalRight(rightUpdateSlope,pos,pos_r,getSlopeRow(t,loc),getIndexVectorElement(t,loc)-1);
        pos_l=pos;
        if ((pos-1>0)&&(rightUpdateSlope>getSlopeRow(t,loc)[pos-1])) {
            findUpdateIntervalRight(rightUpdateSlope,pos-1,pos_r,getSlopeRow(t,loc),getIndexVectorElement(t,loc)-1);
            pos_l=pos-1;
        }
    	for (int i=pos_l; i<pos_r; i++) {
			setSlopeElement(t,loc,i, rightUpdateSlope);
    	}
    }
    
    //to check concavity
    row_t v=getSlopeRow(t,loc);
    
    if (getIndexVectorElement(t,loc)>0) {
        for (int i=1; i<getIndexVectorElement(t,loc); i++) {
            if (v[i-1]<v[i]) {
                std::cout<<"concavity is broken"<<'\n';
            }
        }
    }

    
}*/

void PiecewiseApp_ValueFunction::findUpdateIntervalRight(float& rightUpdateSlope,const int& pos,int& pos_r,const row_t& v, const int& k_max)
{
    float c=0;
    bool flag=false;
    rightUpdateSlope=v[k_max];
    for (int i=pos+2; i<=k_max; i++) {
        for (int j=pos; j<=i-1; j++) {
            c+=v[j];
        }
        if (v[i]<=(float)1/(i-pos)*c) {
            rightUpdateSlope=(float)1/(i-pos)*c;
            pos_r=i;
            flag=true;
            break;
        }
        c=0;
    }
    
    if (!flag) {
        for (int j=pos; j<=k_max; j++) {
            c+=v[j];
        }
        
        if (v[k_max]<(float)1/pos*c) {
            rightUpdateSlope=(float)1/pos*c;
        }
        pos_r=k_max+1;
    }

}

void PiecewiseApp_ValueFunction::findUpdateIntervalLeft(float& leftUpdateSlope,const int& pos,int& pos_l,const row_t& v)
{
    float c=0;
    bool flag=false;
    leftUpdateSlope=v[0];
    for (int i=pos-2; i>=0; i--) {
        for (int j=i+1; j<=pos; j++) {
            c+=v[j];
        }
        if (v[i]>=(float)1/(pos-i)*c) {
            leftUpdateSlope=(float)1/(pos-i)*c;
            pos_l=i+1;
            flag=true;
            break;
        }
        c=0;
    }
    
    if (!flag) {
        for (int j=0; j<=pos; j++) {
            c+=v[j];
        }
        
        if (v[0]>(float)1/pos*c) {
            leftUpdateSlope=(float)1/pos*c;
        }
        pos_l=0;
    }

    
}



