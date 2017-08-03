//
//  ActionsMap.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 7/31/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ActionsMap_h
#define TransshipmentProject_ActionsMap_h

#include "types.h"
#include "Actions.h"
#include <boost/shared_ptr.hpp>

class ActionsMap {
public:
    ActionsMap(const int& location, const int& time);
    ~ActionsMap(){}
    
    void setActionPerPeriodBetweenLocations(const int& time, const int& locationA, const int& locationB, const int& value);
    
    int getActionPerPeriodBetweenLocations(const int& time, const int& locationA, const int& locationB) const;
    
    Actions getActionPerPeriod(const int& time) const;
    
    const ActionsMap& operator=(const ActionsMap& newActionsMap);
    
    int getTimeParameter()const;
    int getLocationParameter()const;
    
private:
    typedef boost::shared_ptr<Actions> actions_ptr;
    std::vector<actions_ptr> actionMap;
    
    int numberTimePeriods;
    int numberLocations;
};

#endif
