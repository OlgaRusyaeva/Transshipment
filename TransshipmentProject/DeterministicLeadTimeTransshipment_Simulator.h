//
//  DeterministicLeadTimeTransshipment_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#ifndef __TransshipmentProject__DeterministicLeadTimeTransshipment_Simulator__
#define __TransshipmentProject__DeterministicLeadTimeTransshipment_Simulator__

#include "SimulatorReplenishmentAndTransshipments.h"
#include "types.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class iDistribution;
class ActionsMap;
class iProblem;

typedef boost::shared_ptr<ActionsMap> actionMap_ptr;
typedef boost::shared_ptr<State> order_ptr;

class DeterministicLeadTimeTransshipment_Simulator: public SimulatorReplenishmentAndTransshipments
{
public:
    DeterministicLeadTimeTransshipment_Simulator(const int& numberOfIterations, const int& seedNumber,const std::vector<actionMap_ptr>& transshipments, const std::vector<order_ptr>& orders):SimulatorReplenishmentAndTransshipments(numberOfIterations,seedNumber){initializeTransshipments(transshipments);initializeOrders(orders);}
    ~DeterministicLeadTimeTransshipment_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix);
    
    State getInitialState(iProblem& problem,const int& iteration) ;
    
private:
    
    std::vector<actionMap_ptr> TR;
    std::vector<order_ptr> IL;
    
    void initializeOrders(const std::vector<order_ptr>& orders);
    
    void initializeTransshipments(const std::vector<actionMap_ptr>& transshipments);
    
    
    
};

#endif /* defined(__TransshipmentProject__DeterministicLeadTimeTransshipment_Simulator__) */
