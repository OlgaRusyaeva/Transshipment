//
//  Header problemMethods.cpp
//

#ifndef project3_problemMethods_h
#define project3_problemMethods_h

#include"types.h"
#include <map>

class State;
class Actions;
class Demand;
class iDistribution;
class Cost;
class LeadTimes;

namespace problemMethods {
    //get cost for current inventories
    float getCost(const row_t& holdingCost,const State& state);
    
    //get post-decision state 
    void  getNewState(State& state, const Actions& action);
    
    void  getNewState(State& state, const Actions& action, const int& lifePeriod);
    
    //next period State
    void  getNewState(State& state);
    
    void getNewState(State& state, const Actions& action, const LeadTimes& leadTimes);
    
    //get transshipment cost for making transfering of goods between locations
    float getTransshipmentCost(const Cost& cost,const Actions& transshipmentMatrix);
    
    //get reward for the demand satisfaction
    float getReward(const row_t& salesCost, const State& state, const Demand& demand);
    
    //get pre-decision state 
    void getNewState(State& state, const Demand& demand);
    
    std::vector<boost::shared_ptr<iDistribution> > initializeDemandDistribution(const vector_t& dType, std::map<int, std::pair<float, float> > demandParameters,const int& NumberOfLocations,const int& seed );
    
    boost::shared_ptr<iDistribution> initializeUniformDemandDistribution(std::map<int, std::pair<float, float> > demandParameters,const int& locationNumber,const int& seed);
    boost::shared_ptr<iDistribution> initializePoissonDemandDistribution(std::map<int, std::pair<float, float> > demandParameters,const int& locationNumber,const int& seed );
    boost::shared_ptr<iDistribution> initializeNegBinomDemandDistribution(std::map<int, std::pair<float, float> > demandParameters,const int& locationNumber,const int& seed );
	boost::shared_ptr<iDistribution> initializeBinomDemandDistribution(std::map<int, std::pair<float, float> > demandParameters,const int& locationNumber,const int& seed );
    
    boost::shared_ptr<iDistribution> initializeDiscreteDemandDistribution(const int& locationNumber,const int& seed, const row_t& probabilities);
    
    boost::shared_ptr<iDistribution> makeConvolutionDistribution(const float& mean);
    
    float probabilityOfDemand(const int& demand, boost::shared_ptr<iDistribution> distribution, const bool& flag);
    
    void resetSimulationParameters(std::vector<boost::shared_ptr<iDistribution> >& distribution,int& seed, const int& NumberOfLocations,const int& n);
};





#endif
