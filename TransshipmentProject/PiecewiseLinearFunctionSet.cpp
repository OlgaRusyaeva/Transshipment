//
//  PiecewiseLinearFunctionSet.cpp
//  TransshipmentProject
//
//  Created by Olga on 25/08/15.
//
//

#include "PiecewiseLinearFunctionSet.h"

PiecewiseLinearFunction& PiecewiseLinearFunctionSet::get(const int& indexH, const int& indexW)
{
    /*if (indexH==3) {
        std::cout<<"here"<<'\n';
    }*/
    return set(indexH,indexW);
}

int PiecewiseLinearFunctionSet::getSizeH() const
{
    return set.sizeH();
}

int PiecewiseLinearFunctionSet::getSizeW() const
{
    return set.sizeW();
}

void PiecewiseLinearFunctionSet::printSlopes()
{
    int sizeH=set.sizeH();
    int sizeW=set.sizeW();
    
    for (int t=0; t<sizeH; t++) {
        std::cout<<"-------Period: "<<t<<"--------"<<'\n';
        for (int j=0; j<sizeW; j++) {
            std::cout<<"Location: "<<j<<'\n';
                for (int i=0; i<set(t,j).getIndex(); i++) {
                    std::cout<<set(t,j).getSlope(i)<<" ";
                }
                std::cout<<'\n';
        }
    }
}


void PiecewiseLinearFunctionSet::printBreakpoints()
{
    int sizeH=set.sizeH();
    int sizeW=set.sizeW();
    
    for (int t=0; t<sizeH; t++) {
        std::cout<<"-------Period: "<<t<<"--------"<<'\n';
        for (int j=0; j<sizeW; j++) {
            std::cout<<"Location: "<<j<<'\n';
                for (int i=0; i<set(t,j).getIndex(); i++) {
                    std::cout<<set(t,j).getBreakpoint(i)<<" ";
                }
                std::cout<<'\n';
        }
    }
}

void PiecewiseLinearFunctionSet::printIndexes()
{
    int sizeH=set.sizeH();
    int sizeW=set.sizeW();
    
    for (int t=0; t<sizeH; t++) {
        std::cout<<"-------Period: "<<t<<"--------"<<'\n';
        for (int j=0; j<sizeW; j++) {
            std::cout<<"Location: "<<j<<" ";
                std::cout<<set(t,j).getIndex()<<'\n';
            }
    }
}