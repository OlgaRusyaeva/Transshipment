//
//  Generator of all possible actions from each location
//  


#include "ActionsGenerator.h"
#include <assert.h>
#include "State.h"
#include "Actions.h"

namespace actions {
    void init(vector_t& v, const int& index,const int& i)
    {
        v[index] = i;
        //for(int i=1; i<(int)v.size(); ++i) v[i]=0;
    }
    
    bool valid(const vector_t& v, const int& j)
    {
        unsigned s = 0;
        for(int i=0; i<(int)v.size(); ++i) s += v[i];
        return s==j;
    }
    
    bool last(const vector_t& v, const int& i, const vector_t& stock_out)
    {
        //assert(valid(v,i));
        
        return v[stock_out[(int)stock_out.size()-1]]==i;
    }
    
    void next(vector_t& v, const int& i, vector_t stock_out)
    {
        //assert(valid(v,i) && !last(v,i));
        
        vector_t::iterator it;
        
        if(v[stock_out[0]]>0)
        {
            v[stock_out[0]]--;
            v[stock_out[1]]++;
        }
        else
        {
            it = stock_out.begin();
            while(v[*it]==0) ++it;
            //assert(i<(int)v.size()-1); 
            v[stock_out[0]] = v[*it]-1;
            v[*it] = 0;
            ++it;
            v[*it]++;
        }
    }
    
    // index of vector - number of location
    // element of vector - matrix of all possible action from the location(index of vector)
    vecMatrixInt transhipmentFromState(const State& state, const int& lifeTime, const bool& outOfStock)
    {
        //boost::math::factorial<float>(2);
        int flag=0;
        int L=state.getLength();
        
        vecMatrixInt actions = vecMatrixInt(L);
        for (int i=0;i<=L-1;i++) 
        {
            flag=state.getElement(i,lifeTime);
            vector_t v=vector_t(L);
            //init(v,flag);
            actions[i].push_back(v);
            if (flag!=0) {  
                vector_t stock_out;
                if (outOfStock) {
                    for (int j=0; j<=L-1; j++) {
                        if ((state.getElement(j,lifeTime)==0)||(j==i)) {
                            stock_out.push_back(j);
                        }
                    }
                }
                else
                {
                    for (int j=0; j<=L-1; j++) {
                        stock_out.push_back(j);
                    } 
                }
                init(v, stock_out[0],flag);
                while(true)
                {
                    actions[i].push_back(v);
                    
                    if(last(v,flag,stock_out)) break;
                    next(v,flag,stock_out);
                }
            }
        }
        return actions;
        
    }
    
    vecMatrixInt transhipmentFromState(const State& state, const matrixInt_t& map)
    {
        //boost::math::factorial<float>(2);
        int flag=0;
        int L=state.getLength();
        
        vecMatrixInt actions = vecMatrixInt(L);
        for (int i=0;i<=L-1;i++) 
        {
            flag=state.getElement(i,0);
            vector_t v=vector_t(L);
            //init(v,flag);
            if (flag!=0) {  
                vector_t stock_out;
                for (int j=0; j<=L-1; j++) {
                    if ((!map[i].empty()&&(find(map[i].begin(), map[i].end(), j)!=map[i].end()))||(j==i)) {
                        stock_out.push_back(j);
                    }
                }
                init(v, stock_out[0],flag);
                while(true)
                {
                    actions[i].push_back(v);
                    
                    if(last(v,flag,stock_out)) break;
                    next(v,flag,stock_out);
                }
            }
            else
                actions[i].push_back(vector_t(L));
        }
        return actions;
        
    }

}


