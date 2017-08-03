//
//  OutputGnuplot.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "Output.h"
#include <fstream>
#include "State.h"
#include "Cost.h"
#include <boost/lexical_cast.hpp>
#include "gnuplot.h"

using namespace std;

namespace output {
    void outputNumericalResults(const string& fileName, const int& number)
    {
        Gnuplot g;
        g.set("terminal x11");
        g.set("key font',18'");
        g.set("grid");
        g.set("xlabel 'Methods'");
        g.set("ylabel 'Profit'");
        g.set("xtics 0,11,16");
        g.set_xrange(0,number);
        g.set("bars 25.0");
        g.set("style fill empty border -1");
        g.plotResults(fileName);
        g.print();
        
        
    }
    
    void outputMap(const State& state, const Cost& distance)
    {
        const int L=state.getLength();
        
        std::vector<int> x,y,IL,number;
        for (int i=0; i < L; i++) {  
            number.push_back(i+1);
            IL.push_back(state.getElement(i,0));
            x.push_back(distance.getXCoordinates(i));  
            y.push_back(distance.getYCoordinates(i)); 
		}
        
        Gnuplot g;
        g.set("terminal x11");
        g.set("key font ',18'");
        g.set("grid");
        g.set_xrange(0,120);
        g.set_yrange(0,120);
        g.set("points");
        g.plotCoordinates(x,y,IL,number,"Coordinates of locations with initial IL");
        g.print();

    }
    
    void outputClustersMap(const Cost& distance,const matrixInt_t& clusters)
    {
        std::vector<float> x,y,color;  
        
        for (int i=0; i<(int)clusters.size(); i++) {
            for (int j=0; j<(int)clusters[i].size(); j++) {
                int locationID=clusters[i][j];
                x.push_back(distance.getXCoordinates(locationID));
                y.push_back(distance.getYCoordinates(locationID));
                color.push_back(i);
            }
        }
        
        Gnuplot g;
        g.set("terminal x11");
        g.set("points");
        g.set("key font ',18'");
        g.set("grid");
        g.set_xrange(0,120);
        g.set_yrange(0,120);
        g.plotClusters(x, y,color,"Clusters");
        g.print();

        
    }
    
    
    void cleanFile(const string& fileName)
    {
        
        // Does file exist?
        fstream f( fileName.c_str(), ios::in );
        if (f)
        {
            // Yes, truncate it
            f.close();
            f.open( fileName.c_str(), ios::out | ios::trunc );
        }
        
        f.close();
        
    }
    
    void outputValueFunction(const int& size1, const int& size2,const std::string& fileName,const int& time)
    {
        
		Gnuplot g;
        g.set("terminal x11");
        g.set("dgrid",size1,size2);
        g.set("ticslevel 0");
        g.set("palette defined (-1 'red', 0 'white', 1 'blue')");
        g.set("pm3d at b");
		g.plot2dGraph(fileName,boost::lexical_cast<std::string>(time));

    }
    


    

}




