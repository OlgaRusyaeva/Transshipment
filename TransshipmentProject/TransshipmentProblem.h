//
// All data about the problem
//  


#ifndef project2_TransshipmentProblem_h
#define project2_TransshipmentProblem_h

#include "iProblem.h"
#include "types.h"

class Input;

class TransshipmentProblem: public iProblem
{
public:
    TransshipmentProblem(const Input& input, const int& scenarioNumber);
    
    ~TransshipmentProblem();
    
    const State& getState() const;
    
    const Demand& getDemand() const;
    
    const int getStateDimension() const;
    
    const int getTimeHorizon()const;
    
    const int getLifeTime() const;
    
    const LeadTimes& getLeadTimes() const;
    
    const Cost& getCost() const;
    
    const matrixInt_t getIndenticalLocations()const;
    
    void outputInput(const std::string& fileName)const;
    
    
private:
    int Time, Location, LifeTime;
    
    matrixInt_t actionPeriods;
    
    State *state;
    Demand *demand;
    Cost *cost;
    LeadTimes *leadTimes;
    matrixInt_t identical;
    //Actions *actions;
    
    
    //void readInput(const std::string& f_fileName);
    
    //void readInputCost(const std::string& f_fileName);
    
    //void readInputDemandParameters(const std::string& f_fileName);
    
    //void readInputIL(const std::string& f_fileName);
    
    void initialize(const Input& input, const int& scenarioNumber);
    
    void initializeLocationNumber(const int& numberLocations);
    
    void initializeTimePeriodsNumber(const int& numberPeriods);
    
    void initializeLifeTimeNumber(const int& maxLifeTime);
    
    void initializeLeadTimes(const matrixInt_t& leadTimeMatrix);
    
    void initializeActionPeriods(const std::vector<int>& periodsActionHappen);
    
    void initializeState(const vector_t& IL, const int& maxLifeTime);
    
    void initializeDemand(const row_t& param1, const row_t& param2, const vector_t& distribution);
    
    void initializeCost(const row_t& h, const row_t& p, const row_t& c, const row_t& od);
    
    void initializeCoordinates(const row_t& x, const row_t& y);
    
    void initializeDistances(const matrix_t& d);
    
    void initializeTimePeriodsNumber();
    
    void findIdenticalLocations();
    
    //void generateCoordinates(gen_uniform_int_AB& gen);
    
    //float distance(const int& stateA, const int& stateB)const;
    
    void outputInitialData(const std::string& fileName)const;

};

#endif
