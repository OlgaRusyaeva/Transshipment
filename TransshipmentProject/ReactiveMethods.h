//
//  Header ReactiveMethods.cpp
//  


#ifndef project3_ReactiveMethods_h
#define project3_ReactiveMethods_h

#include "CommonMethods.h"
#include "iProblem.h"

float gettransshipmentCostByReactiveMethod(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, const int& typeOfMethod,Actions& maxTransshipmentMatrix);

//get location with the lowest transshipment cost for the stock_out location
int getClosestLocation(row_t distanceVector, vector_t vectorOfStock_outLocations);

//get random location for the stock_out location
int getRandomLocation(const int& length, vector_t vectorOfStock_outLocations);

//get location with the highest inventory level for the stock_out location
int getLocationWithHighestInventoryLevel(const State& state,vector_t vectorOfStock_outLocations);

//get location with longest run out time
int getMaxRunOutTimeLocation(const State& state,vector<boost::shared_ptr<iDistribution> > distribution,const int& time,vector_t vectorOfStock_outLocations);


//get how many items to transship from the donor location
int getHowMuchToTransship(const int& stockOfDonorLocation, const int& divider);

int getRunOutTime(const int& state, const float& expectedDemand, const int& time);

vector_t getCloseToStockOutLocations(const State& state, const int& parameter);

#endif
