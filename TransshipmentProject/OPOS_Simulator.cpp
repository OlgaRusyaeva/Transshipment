//
//  OPOS_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 11/12/14.
//
//

#include <stdio.h>
#include "OPOS_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "iProblem.h"
#include "ProblemMethods.h"

extern"C"{int midaco(long int*,long int*,long int*,long int*,long int*,double*,
                     double*,double*,double*,double*,long int*,long int*,double*,
                     double*,long int*,long int*,long int*,char*);
    /**********************************************************************************/
    int midaco_print(int,long int,long int,long int*,long int*,double*,double*,
                     double*,double*,double*,long int,long int,long int,long int,
                     double*,long int,long int,double*,long int,char*); }

using namespace std;

float OPOS_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    int time = problem.getTimeHorizon();
    Cost cost = problem.getCost();
    vector_t source;
    vector_t receive;
    
    for (int i=0; i<location; i++) {
        for (int j=0; j<location; j++) {
            if (derivative(problem,distribution, i,j,currentState.getElement(i), currentState.getElement(j))>0) {
                source.push_back(i);
                receive.push_back(j);
            }
        }
    }
    
    if (source.size()!=0) {
        findTransshipments(problem,currentState,source,receive,distribution,maxTransshipmentMatrix);
        /*for (int i=0; i<(int)source.size(); i++) {
            int transshipAmount=findTransshipmentAmount(problem,distribution, source[i],receive[i],currentState.getElement(source[i]), currentState.getElement(receive[i]));
            //cout<<"from " <<i<<" to "<<j<<" : "<<transshipAmount<<"units"<<'\n';
            //maxTransshipmentMatrix.setElement(i,j, transshipAmount);
            currentState.setElement(source[i],currentState.getElement(source[i])-transshipAmount);
            currentState.setElement(receive[i],currentState.getElement(receive[i])+transshipAmount);
        }*/
    }
    
    problemMethods::getNewState(currentState,maxTransshipmentMatrix);
    
    float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
    
    return transshipmentCost;
}

float OPOS_Simulator::derivative(iProblem& problem, const std::vector<boost::shared_ptr<iDistribution> >& distribution, const int& location1, const int& location2, const double& inv1, const double& inv2)
{
    Cost cost = problem.getCost();
    float derivativeValue=(cost.getSalesCost()[location2]-cost.getSalesCost()[location1]+(cost.getSalesCost()[location1]+cost.getHoldingCost()[location1])*distribution[location1]->cdf(inv1)-(cost.getSalesCost()[location2]+cost.getHoldingCost()[location2])*distribution[location2]->cdf(inv2))-cost.getTransshipmentCost()[location1]*cost.getDistance(location1,location2);
    //cout<<"derivative "<< " from "<<location1<<" to "<<location2<<" equal "<<derivativeValue<<'\n';
    return derivativeValue;
}

void OPOS_Simulator::findTransshipments(iProblem& problem, const State& state, const vector_t& source, const vector_t& receive, const std::vector<boost::shared_ptr<iDistribution> >& distribution,Actions& maxTransshipmentMatrix)
{
    const int size_variables=(int)source.size();
    const int size_constraint=countConstraintSize(source);
    
    long int p,n,ni,m,me,maxeval,maxtime,printeval,save2file,i,iflag,istop,liw,lrw;
    
    n  = size_variables; /* Number of variables (in total) */
    ni = size_variables; /* Number of integer variables (0 <= ni <= n) */
    m  = size_constraint; /* Number of constraints (in total) */
    me = 0; /* Number of equality constraints (0 <= me <= m) */
    p=1;
    
    long int iw[2*n+p+1000+1]; double x[size_variables],xl[size_variables],xu[size_variables],g[size_variables],rw[200*n+2*m+1000+1],param[9],f[1];
    char key[] = "MIDACO_LIMITED_VERSION___[CREATIVE_COMMONS_BY-NC-ND_LICENSE]";
    
    /*  *******************************************************************************/
    
    /*  STEP 1.A: Define problem dimensions  */
    /*  ***********************************  */
    n  = size_variables; /* Number of variables (in total) */
    ni = size_variables; /* Number of integer variables (0 <= ni <= n) */
    m  = size_constraint; /* Number of constraints (in total) */
    me = 0; /* Number of equality constraints (0 <= me <= m) */
    
    /*  STEP 1.B: Define lower and upper bounds 'xl' & 'xu'  */
    /*  ***************************************************  */
    for( i=0; i<n; i++)
    {
        xl[i] = 0.0;
        xu[i] = state.getElement(source[i]);
    }
    
    /*  STEP 1.C: Define starting point 'X'  */
    /*  ***********************************  */
    for( i=0; i<n; i++)
    {
        x[i] = xl[i]; /* Here for example: starting point = lower bounds */
    }
    /*  *******************************************************************************/
    
    /*  STEP 2.A: Define stopping criteria */
    /*  ********************************** */
    maxeval = 10000;     /* Maximum number of function evaluation (e.g. 1000000) */
    maxtime = 60*60*24;  /* Maximum time limit in Seconds (e.g. 1 Day = 60*60*24) */
    
    /*  STEP 2.B: Choose printing options  */
    /*  *********************************  */
    printeval = 0;  /* Print-Frequency for current best solution (e.g. 1000) */
    save2file = 0;     /* Save SCREEN and SOLUTION to TXT-files [ 0=NO/ 1=YES] */
    
    /*  *******************************************************************************/
    
    /*  STEP 3: Choose MIDACO parameters  (ONLY FOR ADVANCED USERS)
     ********************************/
    param[0] =  0.0;   /*  ACCURACY      (default value is 0.001)      */
    param[1] =  0.0;   /*  RANDOM-SEED   (e.g. 1, 2, 3,... 1000)       */
    param[2] =  0.0;   /*  FSTOP                                       */
    param[3] =  0.0;   /*  AUTOSTOP      (e.g. 1, 5, 20, 100,... 500)  */
    param[4] =  0.0;   /*  ORACLE                                      */
    param[5] =  0.0;   /*  FOCUS         (e.g. +/- 10, 500,... 100000) */
    param[6] =  0.0;   /*  ANTS          (e.g. 2, 10, 50, 100,... 500) */
    param[7] =  0.0;   /*  KERNEL        (e.g. 2, 5, 15, 30,... 100)   */
    param[8] =  0.0;   /*  CHARACTER                                   */
    
    //constraints(&*g, &*x, source, receive, state);
    //value_function(&*f, &*x, problem,distribution,source,receive,state);
    
    /*  Note: The default value for all parameters is 0.   */
    /*        See the MIDACO User Manual for more details. */
    
    /*  *******************************************************************************/
    
    /*  Call MIDACO by Reverse Communication  */
    
    /*  *******************************************************************************/
    /* Workspace declarations */
    lrw=sizeof(rw)/sizeof(double);liw=sizeof(iw)/sizeof(long int);
    /* Print midaco headline and basic information */
    midaco_print(1,printeval,save2file,&iflag,&istop,&*f,&*g,&*x,&*xl,&*xu, n,ni,m,me,&*rw,maxeval,maxtime,&*param,1,&*key);
    while(istop==0) /*~~~ Start of the reverse communication loop ~~~*/
    {   /* Evaluate objective function */
        //problem_function( &*f, &*g, &*x);
        constraints(&*g, &*x, source, receive, state);
        value_function(&*f, &*x, problem,distribution,source,receive,state);
        /* Call MIDACO */
        midaco(&p,&n,&ni,&m,&me,&*x,&*f,&*g,&*xl,&*xu,&iflag,
               &istop,&*param,&*rw,&lrw,&*iw,&liw,&*key);
        /* Print current best solution after every *printeval* evaluation */
        midaco_print(2,printeval,save2file,&iflag,&istop,&*f,&*g,&*x,&*xl,&*xu, n,ni,m,me,&*rw,maxeval,maxtime,&*param,1,&*key);
    } /*~~~End of the reverse communication loop ~~~*/
    /*  *******************************************************************************/
    /* Independent check of MIDACO solution */
    //problem_function( &*f, &*g, &*x);
    constraints(&*g, &*x, source, receive, state);
    value_function(&*f, &*x, problem,distribution,source,receive,state);
    /* Print the solution */
    midaco_print(3,printeval,save2file,&iflag,&istop,&*f,&*g,&*x,&*xl,&*xu, n,ni,m,me,&*rw,maxeval,maxtime,&*param,1,&*key);
    /*  *******************************************************************************/
    
    for (int i=0; i<size_variables; i++) {
        maxTransshipmentMatrix.setElement(source[i],receive[i],x[i]);
    }
    
    
}

int OPOS_Simulator::countConstraintSize(const vector_t& source)
{
    int size_v=(int)source.size();
    int i=1;
    int count=0;
    int element=0;
    while (i!=size_v) {
        if (source[i]==source[i-1]) {
            element=source[i];
            count++;
            while ((i+1!=size_v)&&(source[i+1]==element)) {
                i++;
            }
        }
        i++;
    }
    return count;
}
        
void OPOS_Simulator::value_function(double *f, double *x, iProblem& problem, const std::vector<boost::shared_ptr<iDistribution> >& distribution,const vector_t& source, const vector_t& receive, const State& state)
{
    /* Objective function value F(X) (denoted here as f[0]) */
    f[0]=0;
    for (int i=0; i<(int)source.size(); i++) {
        f[0]+=derivative(problem,distribution, source[i],receive[i],state.getElement(source[i])-x[i], state.getElement(receive[i])+x[i])*derivative(problem,distribution, source[i],receive[i],state.getElement(source[i])-x[i], state.getElement(receive[i])+x[i]);
    }

}
        
void OPOS_Simulator::constraints(double *g, double *x, const vector_t& source, const vector_t& receive, const State& state)
{
    /* Equality constraints G(X) = 0 MUST COME FIRST in g[0:me-1] */
    /* Inequality constraints G(X) >= 0 MUST COME SECOND in g[me:m-1] */
    int size_v=(int)source.size();
    int i=1;
    int element=0;
    int count=0;
    
    while (i!=size_v) {
        if (source[i]==source[i-1]) {
            element=source[i];
            g[count]=state.getElement(element)-x[i-1]-x[i];
            while ((i+1!=size_v)&&(source[i+1]==element)) {
                g[count]-=x[i+1];
                i++;
            }
            count++;
        }
        i++;
    }
}

int OPOS_Simulator::findTransshipmentAmount(iProblem& problem, const std::vector<boost::shared_ptr<iDistribution> >& distribution, const int& location1, const int& location2, const int& inv1, const int& inv2)
{
    int x_begin=0;
    int x_end=inv1;
    int x_middle=(int)inv1/2;
    int step=0;
    
    while ((x_end-x_begin>1)&&(derivative(problem,distribution,location1,location2,inv1-x_middle,inv2+x_middle)!=0)) {
        step=(int)(x_end-x_begin)/2;
        x_middle=x_begin+step;
        if (derivative(problem,distribution,location1,location2,inv1-x_middle,inv2+x_middle)<0) {
            x_end=x_middle;
        }
        else
            x_begin=x_middle;
    }
    
    if (derivative(problem,distribution,location1,location2,inv1-x_middle,inv2+x_middle)==0) {
        return x_middle;
    }
    else
    {
        if (derivative(problem,distribution,location1,location2,inv1-x_middle-0.5,inv2+x_middle+0.5)>0) {
            return x_end;
        }
        else
            return x_begin;
    }
    
}