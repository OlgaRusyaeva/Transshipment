//
//  PLValueFunction.cpp
//  TransshipmentProject
//
//  Created by Olga on 25/08/15.
//
//

#include "PLValueFunction.h"

PLValueFunction::PLValueFunction(int sizeHH, int sizeWW, int sizeDD)
{
    tuningParameter=20;
    sizeH=sizeHH;
    sizeW=sizeWW;
    sizeD=sizeDD;
    for (int i=0; i<sizeD; i++) {
        set.push_back(boost::shared_ptr<PiecewiseLinearFunctionSet>(new PiecewiseLinearFunctionSet(sizeH,sizeW)));
    }
}


PiecewiseLinearFunction& PLValueFunction::get(const int& indexH, const int& indexW, const int& indexD)
{
    return set[indexD]->get(indexH,indexW);
}

int PLValueFunction::getSizeH() const
{
    return sizeH;
}

int PLValueFunction::getSizeW() const
{
    return sizeW;
}

int PLValueFunction::getSizeD() const
{
    return sizeD;
}

boost::shared_ptr<PiecewiseLinearFunctionSet>& PLValueFunction::getPiecewiseLinearFunctionFor(const int& indexD)
{
    return set[indexD];
}

void PLValueFunction::printSlopes()
{
    for (int t=0; t<sizeH; t++) {
        std::cout<<"-------Period: "<<t<<"--------"<<'\n';
        for (int j=0; j<sizeW; j++) {
            std::cout<<"Location: "<<j<<'\n';
            for (int l=0; l<sizeD; l++) {
                std::cout<<"In Lead Time: "<<l<<'\n';
                for (int i=0; i<set[l]->get(t,j).getIndex(); i++) {
                    std::cout<<set[l]->get(t,j).getSlope(i)<<" ";
                }
                std::cout<<'\n';
            }
        }
    }
}


void PLValueFunction::printBreakpoints()
{
    for (int t=0; t<sizeH; t++) {
        std::cout<<"-------Period: "<<t<<"--------"<<'\n';
        for (int j=0; j<sizeW; j++) {
            std::cout<<"Location: "<<j<<'\n';
            for (int l=0; l<sizeD; l++) {
                std::cout<<"In Lead Time: "<<l<<'\n';
                for (int i=0; i<set[l]->get(t,j).getIndex(); i++) {
                    std::cout<<set[l]->get(t,j).getBreakpoint(i)<<" ";
                }
                std::cout<<'\n';
            }
        }
    }
}

void PLValueFunction::printIndexes() 
{
    for (int t=0; t<sizeH; t++) {
        std::cout<<"-------Period: "<<t<<"--------"<<'\n';
        for (int j=0; j<sizeW; j++) {
            std::cout<<"Location: "<<j<<" ";
            for (int l=0; l<sizeD; l++) {
                std::cout<<"In Lead Time: "<<l<<": ";
                std::cout<<set[l]->get(t,j).getIndex()<<'\n';
            }
        }
    }
}

void PLValueFunction::update(PiecewiseLinearFunction& pl, const float& new_slopePlus, const float& new_slopeMinus, const int& pos_A, const int& pos_B, const int& currentState, const float& alpha)
{
    for (int i=pos_A; i<=pos_B; i++) {
        if (pl.getBreakpoint(i)<currentState) {
            pl.setSlope(i, (1-alpha)*pl.getSlope(i)+alpha*new_slopeMinus);
        }
        else
            pl.setSlope(i, (1-alpha)*pl.getSlope(i)+alpha*new_slopePlus);
    }
}

void PLValueFunction::insert(PiecewiseLinearFunction& pl, const int& new_breakpoint, int& pos)
{
    
    int k=pl.getIndex();
    vector_t u=pl.getBreakpoints();
    row_t v=pl.getSlopes();
    
    for (int i=0; i<k; i++) {
        if (u[i]==new_breakpoint) {
            pos=i;
            break;
        }
        else
        {
            if (u[i]>new_breakpoint) {
                pl.insertBreakpoint(i,new_breakpoint);
                pl.insertSlope(i,v[i-1]);
                pl.addIndex(1);
                pos=i;
                break;
                
            }
            else
            {
                if (i==k-1) {
                    pl.addBreakpoint(new_breakpoint);
                    pl.addSlope(v[i]);
                    pl.addIndex(1);
                    pos=i+1;
                    break;
                }
            }
        }
    }
}

int PLValueFunction::nearestBreakpoint(const PiecewiseLinearFunction& pl, const int& value)
{
    int pos=0;
    int k=pl.getIndex();
    vector_t u=pl.getBreakpoints();
    
    for (int i=0; i<k; i++) {
        if (u[i]==value) {
            pos=-1;
            break;
        }
        else
        {
            if (u[i]>value) {
                pos=i-1;
                break;
                
            }
            else
            {
                if (i==k-1) {
                    pos=i;
                    break;
                }
            }
        }
    }
    
    return pos;
}



//Check the concavity for big size problems
void PLValueFunction::updateSlopeCave(const int& t, const int& loc, const int& leadTime,const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& L, const int& iteration, const int& step)
{
    float alpha=(float)tuningParameter/(2*tuningParameter+iteration);
    
    int left=0;
    int right=0;
    
    int pos_a=0;
    int pos_b=0;
    int pos_c=0;
    
    insert(set[leadTime]->get(t,loc),inventory,pos_b);
    
    //find min_k where the concave property will be broken
    //if the left side of min update interval<0 then pos_a=0
    if (inventory-step>=0) {
        
        int maxIndex=set[leadTime]->get(t,loc).getIndex();
        int min_k=-1;
        
        for (int j=0; j<pos_b; j++) {
            if (set[leadTime]->get(t,loc).getSlope(j)<=(1-alpha)*set[leadTime]->get(t,loc).getSlope(j+1)+alpha*gradientMinus) {
                min_k=j;
                break;
            }
        }
        
        //if min_k doesn't exist OR min{inventory-step,u[min_k]}=invnetory-step
        if ((min_k<0)||(inventory-step<set[leadTime]->get(t,loc).getBreakpoint(min_k))) {
            //take the left side of min update interval
            left=inventory-step;
            int j=nearestBreakpoint(set[leadTime]->get(t,loc),left);
            //insert breakpoint if it is required and define the left position for the update
            if ((j!=-1)&&(set[leadTime]->get(t,loc).getSlope(j)<(1-alpha)*set[leadTime]->get(t,loc).getSlope(j)+alpha*gradientMinus)) {
                pos_a=j;
            }
            else
            	insert(set[leadTime]->get(t,loc),left, pos_a);
            
            pos_b=pos_b+maxIndex-set[leadTime]->get(t,loc).getIndex();
        }
        else
        {
            //take the left side the breakpoint of min_k
            pos_a=min_k;
        }
        
        
    }
    
    //find max_k where the concave property will be broken
    int max_k=-1;
    
    for (int j=set[leadTime]->get(t,loc).getIndex()-1; j>pos_b; j--) {
        if ((j-1>=0)&&(1-alpha)*set[leadTime]->get(t,loc).getSlope(j-1)+alpha*gradientPlus<=set[leadTime]->get(t,loc).getSlope(j)) {
            max_k=j;
            break;
        }
    }
    
    if (max_k>=set[leadTime]->get(t,loc).getIndex()-1) {
        pos_c=get(t,loc,leadTime).getIndex()-1;
    }
    else
    {
        //define the right side of the update interval
        if ((max_k<0)||(inventory+step>set[leadTime]->get(t,loc).getBreakpoint(max_k))) {
            right=inventory+step;
            int j=nearestBreakpoint(set[leadTime]->get(t,loc),right);
            if ((j!=-1)&&((1-alpha)*set[leadTime]->get(t,loc).getSlope(j)+alpha*gradientPlus<set[leadTime]->get(t,loc).getSlope(j))) {
                pos_c=j;
            }
            else
            {
            	insert(set[leadTime]->get(t,loc), right,pos_c);
                pos_c--;
            }
            /*if ((pos_c!=get(t,loc,leadTime).getIndex()-1)&&(1-alpha)*set[leadTime]->get(t,loc).getSlope(pos_c)+alpha*gradientPlus<=set[leadTime]->get(t,loc).getSlope(pos_c+1)) {
                pos_c++;
            }*/
        }
        else
        {
            pos_c=max_k;
        }
    }
    
    update(set[leadTime]->get(t,loc), gradientPlus, gradientMinus, pos_a, pos_c, inventory, alpha);
    
    
    //to check concavity
    row_t v=set[leadTime]->get(t,loc).getSlopes();
     
     if (get(t,loc,leadTime).getIndex()>0) {
     	for (int i=1; i<get(t,loc,leadTime).getIndex(); i++) {
     		if (v[i-1]<v[i]) {
     			std::cout<<"concavity is broken"<<'\n';
     		}
     	}
     }
    
}


void PLValueFunction::updateSlopeLeveling(const int& t, const int& loc, const int& leadTime,const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& iteration)
{
    
    float alpha=(float)tuningParameter/(2*tuningParameter+iteration);
    
    int pos=0;
    
    //insert breakpoint if it is required
    insert(set[leadTime]->get(t,loc), inventory,pos);
    
    //update slopes around breakpoint
    if (pos>0) {
        update(set[leadTime]->get(t,loc), gradientPlus, gradientMinus, pos-1, pos, inventory, alpha);
        
        
        //if concavity is broken before new breakpoint, set the broken slope equal to the left slope of new breakpoint
        //we assume that gradientMinus >= gradientPlus
        for (int i=pos-1; i>=0; i--) {
            if (set[leadTime]->get(t,loc).getSlope(i)<set[leadTime]->get(t,loc).getSlope(pos-1))
            {
                set[leadTime]->get(t,loc).setSlope(i, set[leadTime]->get(t,loc).getSlope(pos-1));
            }
        }
    }
    else
        update(set[leadTime]->get(t,loc), gradientPlus, gradientMinus, pos, pos, inventory, alpha);
    
    
    //if concavity is broken after new breakpoint, set the broken slope equal to the right slope of new breakpoint
    for (int j=pos+1; j<set[leadTime]->get(t,loc).getIndex(); j++) {
        if (set[leadTime]->get(t,loc).getSlope(j)>set[leadTime]->get(t,loc).getSlope(pos)) {
            set[leadTime]->get(t,loc).setSlope(j, set[leadTime]->get(t,loc).getSlope(pos));
        }
    }
    
    
    //to check concavity
    row_t v=set[leadTime]->get(t,loc).getSlopes();
    
    if (get(t,loc,leadTime).getIndex()>0) {
        for (int i=1; i<get(t,loc,leadTime).getIndex(); i++) {
            if (v[i-1]<v[i]) {
                std::cout<<"concavity is broken"<<'\n';
            }
        }
    }
    
    
}

//Check the concavity for big size problems
void PLValueFunction::updateSlopeSpar(const int& t, const int& loc, const int& leadTime, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& iteration)
 {
	float alpha=(float)tuningParameter/(2*tuningParameter+iteration);
 
    int pos=0;
    int pos_l=0;
    int pos_r=0;
 
 	float leftUpdateSlope=0;
 	float rightUpdateSlope=0;
 
 	//insert breakpoint if it is required
 	insert(set[leadTime]->get(t,loc), inventory,pos);
 
 	//update slopes around breakpoint
 	if (pos>0)
 	{
 		update(set[leadTime]->get(t,loc), gradientPlus, gradientMinus, pos-1, pos, inventory, alpha);
 
 
 		if ((pos-2>=0)&&(set[leadTime]->get(t,loc).getSlope(pos-2)<set[leadTime]->get(t,loc).getSlope(pos-1)))
 		{
 			findUpdateIntervalLeft(leftUpdateSlope,pos-1,pos_l,set[leadTime]->get(t,loc).getSlopes());
 			pos_r=pos;
 			if (leftUpdateSlope<set[leadTime]->get(t,loc).getSlope(pos)) {
 				findUpdateIntervalLeft(leftUpdateSlope,pos,pos_l,set[leadTime]->get(t,loc).getSlopes());
 				pos_r=pos+1;
 			}
 
 			for (int i=pos_l; i<pos_r; i++) {
                set[leadTime]->get(t,loc).setSlope(i,leftUpdateSlope);
 			}
 		}
 
 	}
 	else
 	{
 		update(set[leadTime]->get(t,loc), gradientPlus, gradientMinus, pos, pos, inventory, alpha);
 
 	}
 
 
 	if ((pos+1<set[leadTime]->get(t,loc).getIndex())&&(set[leadTime]->get(t,loc).getSlope(pos)<set[leadTime]->get(t,loc).getSlope(pos+1))) {
 
 		findUpdateIntervalRight(rightUpdateSlope,pos,pos_r,set[leadTime]->get(t,loc).getSlopes(),set[leadTime]->get(t,loc).getIndex()-1);
 		pos_l=pos;
 		if ((pos-1>=0)&&(rightUpdateSlope>set[leadTime]->get(t,loc).getSlope(pos-1))) {
 			findUpdateIntervalRight(rightUpdateSlope,pos-1,pos_r,set[leadTime]->get(t,loc).getSlopes(),set[leadTime]->get(t,loc).getIndex()-1);
 			pos_l=pos-1;
 		}
 		for (int i=pos_l; i<pos_r; i++) {
            set[leadTime]->get(t,loc).setSlope(i,rightUpdateSlope);
 		}
 	}
     
     
 
 	//to check concavity
 	row_t v=set[leadTime]->get(t,loc).getSlopes();
 
 	if (get(t,loc,leadTime).getIndex()>0) {
 		for (int i=1; i<get(t,loc,leadTime).getIndex(); i++) {
 			if (v[i-1]<v[i]) {
 				std::cout<<"concavity is broken"<<'\n';
 			}
 		}
 	}
 
 
 }

void PLValueFunction::findUpdateIntervalRight(float& rightUpdateSlope,const int& pos,int& pos_r,const row_t& v, const int& k_max)
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
        
        rightUpdateSlope=(float)1/(k_max-pos+1)*c;
        pos_r=k_max+1;
    }
    
}

void PLValueFunction::findUpdateIntervalLeft(float& leftUpdateSlope,const int& pos,int& pos_l,const row_t& v)
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
        
        leftUpdateSlope=(float)1/(pos+1)*c;
        pos_l=0;
    }
    
    
}

