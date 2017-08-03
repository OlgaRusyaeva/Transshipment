//
//  ActionsMap.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 7/31/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "ActionsMap.h"


ActionsMap::ActionsMap(const int& location, const int& time)
{
    numberTimePeriods=time;
    numberLocations=location;
    
    for (int t=0; t<numberTimePeriods; t++) {
        actions_ptr ptr(new Actions(numberLocations));
        actionMap.push_back(ptr);
    }
    //std::vector<boost::shared_ptr<Actions> > actionsMap(numberTimePeriods,boost::shared_ptr<Actions>(new Actions(numberLocations)));
}


void ActionsMap::setActionPerPeriodBetweenLocations(const int& time, const int& locationA, const int& locationB, const int& value)
{
    actionMap[time]->setElement(locationA,locationB,value);
}

int ActionsMap::getActionPerPeriodBetweenLocations(const int& time, const int& locationA, const int& locationB) const
{
    return actionMap[time]->getElement(locationA,locationB);
}

const ActionsMap& ActionsMap::operator=(const ActionsMap& newActionsMap)
{
    if (this == &newActionsMap) {
        return *this;
    }
    for (int t=0; t<numberTimePeriods; t++) {
        for (int i=0; i<numberLocations; i++) {
            for (int j=0; j<numberLocations; j++) {
                actionMap[t]->setElement(i,j,newActionsMap.actionMap[t]->getElement(i,j));
            }
        }
    }
    return *this;
}

Actions ActionsMap::getActionPerPeriod(const int& time) const
{
    return *actionMap[time];
}

int ActionsMap::getTimeParameter()const
{
    return numberTimePeriods;
}
int ActionsMap::getLocationParameter()const
{
    return numberLocations;
}