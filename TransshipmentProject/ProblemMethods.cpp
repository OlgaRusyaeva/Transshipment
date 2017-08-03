//
// Common methods that used both Reactive methods and DP
//

#include "ProblemMethods.h"
#include "NumbersOperations.h"
#include "Cost.h"
#include "State.h"
#include "Actions.h"
#include "Demand.h"
#include "PoissonDistribution.h"
#include "UniformDistirbution.h"
#include "NegativeBinomialDistribution.h"
#include "BinomialDistribution.h"
#include "DiscreteDistribution.h"
//#include "VectorsOperations.h"
#include <boost/shared_ptr.hpp>
#include "iDistribution.h"
#include "LeadTimes.h"

using namespace std;
using namespace numbersOperations;

namespace problemMethods {
    
    //get cost for current inventories
    float getCost(const row_t& cost,const State& state)
    {
        const int L=state.getLength();
        const int W=state.getWidth();
        float ct =0;
        for (int i=0; i<L; i++)
        {
            for (int j=0; j<W; j++) {
                ct+=cost[i]*state.getElement(i,j);
            }

        }
        
        return ct;
    }
    
    //get transshipment cost for making transfering of goods between locations
    float getTransshipmentCost(const Cost& cost, const Actions& transshipmentMatrix)
    {
        float tr_cost =0;
        const int size=(int)transshipmentMatrix.getSize();
        for (int i=0; i<=size-1; i++) 
        {
            for (int j=0; j<=size-1; j++) {
                tr_cost+=cost.getTransshipmentCost()[i]*cost.getDistance(i,j)*transshipmentMatrix.getElement(i,j);//transshipment cost depends on the distances betweeen locations
            }
        }
        return tr_cost;
        
    }
    
    
    //get reward for the demand satisfaction
    float getReward(const row_t& salesCost, const State& state, const Demand& demand)
    {
        const int L=state.getLength();
        const int W=state.getWidth();
        float reward = 0;
        for (int i=0; i<L; i++) {
            for (int j=0; j<W; j++) {
                int s=0;
                for (int k=0; k<j; k++) {
                    s+=state.getElement(i,k);
                }
                reward += salesCost[i]*getMinValue(state.getElement(i,j),getMaxValue(demand.getElement(i)-s,0));
            }
        }
        return reward;
        
    }
    
    
   
    
    //TODO - change discrete distribution implementation
    vector<boost::shared_ptr<iDistribution> > initializeDemandDistribution(const vector_t& dType, map<int, pair<float, float> > demandParameters, const int& NumberOfLocations,const int& seed)
    {
        vector<boost::shared_ptr<iDistribution> > distributionVector;
        static const float probBimod[5]={0.5, 0, 0, 0, 0.5};
        static const float probUnimod[5]={0.1, 0.2, 0.4, 0.2, 0.1};
        static const float probUniform[5]={0.2, 0.2, 0.2, 0.2, 0.2};

        for (int i=0; i<=NumberOfLocations-1; i++) {
        switch (dType[i]) {
            case 1:
                distributionVector.push_back(initializeUniformDemandDistribution(demandParameters,i,seed));
                break;
            case 2:
                distributionVector.push_back(initializePoissonDemandDistribution(demandParameters,i,seed));
                break;
            case 3:
                distributionVector.push_back(initializeNegBinomDemandDistribution(demandParameters,i,seed));
                break;
            case 4:
                distributionVector.push_back(initializeBinomDemandDistribution(demandParameters,i,seed));
                break;
            case 5:
                distributionVector.push_back(initializeDiscreteDemandDistribution(i,seed,row_t(probBimod, probBimod + sizeof(probBimod) / sizeof(probBimod[0]) )));
                break;
            case 6:
                distributionVector.push_back(initializeDiscreteDemandDistribution(i,seed,row_t(probUnimod, probUnimod + sizeof(probUnimod) / sizeof(probUnimod[0]) )));
                break;
            case 7:
                distributionVector.push_back(initializeDiscreteDemandDistribution(i,seed,row_t(probUniform, probUniform + sizeof(probUniform) / sizeof(probUniform[0]) )));
                break;
        }
                
        }
        
        return distributionVector;
        
    }
    
    boost::shared_ptr<iDistribution> initializeUniformDemandDistribution(map<int, pair<float, float> > demandParameters, const int& locationNumber,const int& seed)
    {
        return boost::shared_ptr<iDistribution>(new UniformDistirbution(demandParameters[locationNumber].first, demandParameters[locationNumber].second,locationNumber+seed));
    }
    
    boost::shared_ptr<iDistribution> initializePoissonDemandDistribution(map<int, pair<float, float> > demandParameters, const int& locationNumber,const int& seed)
    {
        return boost::shared_ptr<iDistribution>(new PoissonDistribution(demandParameters[locationNumber].first,locationNumber+seed));
    }
    
    boost::shared_ptr<iDistribution> initializeNegBinomDemandDistribution(map<int, pair<float, float> > demandParameters, const int& locationNumber,const int& seed)
    {
        return boost::shared_ptr<iDistribution>(new NegativeBinomialDistribution(demandParameters[locationNumber].first, demandParameters[locationNumber].second,locationNumber+seed));
    }
    
    boost::shared_ptr<iDistribution> initializeBinomDemandDistribution(std::map<int, std::pair<float, float> > demandParameters,const int& locationNumber,const int& seed )
    {
        return boost::shared_ptr<iDistribution>(new BinomialDistribution(demandParameters[locationNumber].first, demandParameters[locationNumber].second,locationNumber+seed));
    }
    
    boost::shared_ptr<iDistribution> initializeDiscreteDemandDistribution(const int& locationNumber,const int& seed, const row_t& probabilities)
    {
        return boost::shared_ptr<iDistribution>(new DiscreteDistribution(locationNumber+seed,probabilities));
    }
    
    void resetSimulationParameters(std::vector<boost::shared_ptr<iDistribution> >& distribution,int& seed, const int& NumberOfLocations,const int& n)
    {
        for (int i=0; i<=NumberOfLocations-1; i++) {
        	distribution[i]->changeSeed(seed+(n+1)*2);
            distribution[i]->resetPRNG();
        }
    }
    
    boost::shared_ptr<iDistribution> makeConvolutionDistribution(const float& mean)
    {
        boost::shared_ptr<iDistribution> tmp(new PoissonDistribution(mean,20));
        return tmp;
    }
    
    //calculate state after demand satisfaction
    void getNewState(State& state, const Demand& demand)
    {
        const int L=(int)state.getLength();
        const int W=(int)state.getWidth();
        for (int i=0; i<L; i++) {
            for (int j=0; j<W; j++) {
                int s=0;
                for (int k=0; k<j; k++) {
                    s+=state.getElement(i,k);
                }
                state.setElement(i,j,getMaxValue(state.getElement(i,j)-getMaxValue(demand.getElement(i)-s,0),0));
            }
        }
    }
    
    void  getNewState(State& state)
    {
        const int sizeL=state.getLength();
        const int sizeD=state.getDepth();
        const int sizeW=state.getWidth();
        if (sizeD>1) {
        	for (int i=0; i<sizeL; i++) {
            	for (int t=1; t<=sizeD; t++) {
                	if (t==1) {
                   	 state.setElement(i,t-1,state.getElement(i,t-1)+state.getElement(i,t));
                	}
                	else
                	{
                    	if (t==sizeD) {
                        	state.setElement(i,t-1,0);
                    	}
                    	else
                        	state.setElement(i,t-1,state.getElement(i,t));
                	}

            	}
        	}
        }
        
        if (sizeW>1) {
            for (int i=0; i<sizeL; i++){
                for (int j=1; j<sizeW; j++) {
                    state.setElement(i,j-1,state.getElement(i,j));
                }
                state.setElement(i,sizeW-1,0);
            }
        }
    }
    
    void getNewState(State& state, const Actions& action)
    {
        getNewState(state,action,0);
    }
    
    void  getNewState(State& state, const Actions& action, const int& lifePeriod)
    {
        const int L=(int)state.getLength();
        //State newState(L);
        for (int i=0; i<=L-1; i++) {
            for (int j=0; j<=L-1; j++) {
                state.setElement(i,lifePeriod,state.getElement(i,lifePeriod)-action.getElement(i,j)+action.getElement(j,i));
            }
        }
    }
    
    void getNewState(State& state, const Actions& action, const LeadTimes& leadTimes)
    {
        const int L=(int)state.getLength();
        //State newState(L);
        for (int i=0; i<=L-1; i++) {
            for (int j=0; j<=L-1; j++) {
                int t=leadTimes.getLeadTimeBetweenLocations(i,j);
                state.setElement(i,0,state.getElement(i,0)-action.getElement(i,j));
                state.setElement(i,t,state.getElement(i,t)+action.getElement(j,i));
            }
        }
        
    }
    
    float probabilityOfDemand(const int& demand, boost::shared_ptr<iDistribution>  distribution, const bool& flag)
    {
        float probability=0;
        if (flag) {
            //cout<<"pdf for demand: "<< demand <<" equal "<<distribution->pdf(demand)<<'\n';
            probability=distribution->pdf(demand);
        }
        else
        {
            //cout<<"pdf for demand more or equal: "<< demand <<" equal "<<distribution->complement_cdf(demand-1)<<'\n';
            probability=distribution->complement_cdf(demand-1);//for discrete distribution (demand-1)
        }
        return probability;
        
    }



}


