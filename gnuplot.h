//
//  gnuplot.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 5/8/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_gnuplot_h
#define TransshipmentProject_gnuplot_h

#include <string>
#include <iostream>
using namespace std;

class GnuplotException : public std::runtime_error
{
public:
    GnuplotException(const std::string &msg) : std::runtime_error(msg){}
};

class Gnuplot {
public:
    Gnuplot() ;
    ~Gnuplot();
    void set(const string & command);
    void set(const string & command, const double param1, const double param2);
    void set_xrange(const double iFrom, const double iTo);
    void set_yrange(const double iFrom, const double iTo);
	void plot(const string & command);
    void print();
    template<typename X, typename Y, typename Label>
    void plotCoordinates(const X& x, const Y& y, const Label& l1, const Label& l2, const string & command);
    template<typename X, typename Y, typename Color>
    void plotClusters(const X& x, const Y& y, const Color& l,const string & command);
    
    void plotfile_xy_withLabel(const std::string &filename,
                                   const unsigned int column_x = 1,
                                   const unsigned int column_y = 2,
                                   const unsigned int column_z = 3,
                                	const unsigned int column_d = 4,
                                   const std::string &title = "");
    void plotfile_xy_withColor(const std::string &filename,
                               const unsigned int column_x = 1,
                               const unsigned int column_y = 2,
                               const unsigned int column_z = 3,
                               const std::string &title = "");
    void plotResults(const std::string &filename);
    void plot2dGraph(const std::string &filename,const string & command);
protected:
    FILE *gnuplotpipe;
private:
    std::string    create_tmpfile(std::ofstream &tmp);  
    bool fileExist(const std::string &filename);
};

Gnuplot::Gnuplot() 
{
    gnuplotpipe=popen("/usr/local/bin/gnuplot -persist","w");  
    if (!gnuplotpipe) {
        throw GnuplotException("Gnuplot not found !"); }
}

Gnuplot::~Gnuplot() {
    //fprintf(gnuplotpipe,"quit\n");
    pclose(gnuplotpipe);
}

void Gnuplot::set(const string & command) {
    fprintf(gnuplotpipe,"set %s\n",command.c_str());
    // flush is necessary, nothing gets plotted else
}

void Gnuplot::set(const string & command, const double param1, const double param2)
{
    fprintf(gnuplotpipe,"set %s %f,%f\n",command.c_str(),param1,param2);
}

void Gnuplot::set_xrange(const double iFrom, const double iTo)
{
    fprintf(gnuplotpipe,"set xrange [%f:%f]\n",iFrom,iTo);
}

void Gnuplot::set_yrange(const double iFrom, const double iTo)
{
    fprintf(gnuplotpipe,"set yrange [%f:%f]\n",iFrom,iTo);
}

void Gnuplot::plot(const string & command) {
    fprintf(gnuplotpipe,"%s\n",command.c_str());
    // flush is necessary, nothing gets plotted else
}

void Gnuplot::print()
{
	fflush(gnuplotpipe);
}

template<typename X, typename Y, typename Label>
void Gnuplot::plotCoordinates(const X& x, const Y& y, const Label& l1, const Label& l2,const string & command)
{
    std::ofstream tmp;
    std::string name = create_tmpfile(tmp);

    for (unsigned int i = 0; i < x.size(); i++)
        tmp << x[i] << " " << y[i] <<" "<<l1[i]<<" "<<l2[i]<< std::endl;
    
    tmp.flush();
    tmp.close();
    
    plotfile_xy_withLabel(name, 1, 2, 3,4, command);

}

std::string Gnuplot::create_tmpfile(std::ofstream &tmp)
{
    
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    char name[] = "gnuplotiXXXXXX"; //tmp file in working directory
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    char name[] = "/tmp/gnuplotiXXXXXX"; // tmp file in /tmp
#endif
    
    tmp.open(name);
    
    return name;
}

void Gnuplot::plotResults(const std::string &filename)
{
    if (fileExist(filename)) {
        fprintf(gnuplotpipe,"plot '%s' u 1:4:4:4:4 w candlesticks lt -1 lw 2 notitle, ",filename.c_str());
        fprintf(gnuplotpipe,"'' using 1:3:2:6:5 w candlesticks notitle whiskerbars lt rgb '#99211D,");
    }
    else
       throw GnuplotException("file doesn't exist");
}

template<typename X, typename Y, typename Color>
void Gnuplot::plotClusters(const X& x, const Y& y, const Color& c,const string & command)
{
    std::ofstream tmp;
    std::string name = create_tmpfile(tmp);
    
    for (unsigned int i = 0; i < x.size(); i++)
        tmp << x[i] << " " << y[i] <<" "<<c[i]<< std::endl;
    
    tmp.flush();
    tmp.close();
    
    plotfile_xy_withColor(name, 1, 2, 3,command);
}

void Gnuplot::plot2dGraph(const std::string &filename,const string & command)
{
    if (fileExist(filename)) {
        fprintf(gnuplotpipe,"splot '%s' using 1:2:3 with lines title '%s'\n ",filename.c_str(),command.c_str());
    }
    else
        throw GnuplotException("file doesn't exist");
}

void Gnuplot::plotfile_xy_withLabel(const std::string &filename,
                                        const unsigned int column_x,
                                        const unsigned int column_y,
                                        const unsigned int column_z,
                                        const unsigned int column_d,
                                        const std::string &title)
{
	if (fileExist(filename)) {
        //draw labels
        fprintf(gnuplotpipe," plot '%s' using ($%d+5):($%d+4):%d w labels font ',20' notitle, ",filename.c_str(),column_x,column_y,column_z);
        //draw points
        fprintf(gnuplotpipe,"'' using %d:%d pt 6 ps 5 lc 9 lw 2 title '%s',",column_x,column_y,title.c_str());
        fprintf(gnuplotpipe," '' using ($%d):($%d):%d w labels font ',15' textcolor lt 1 notitle \n",column_x,column_y,column_d);
    }
    else
        throw GnuplotException("file doesn't exist");
    
}

void Gnuplot::plotfile_xy_withColor(const std::string &filename,
                                    const unsigned int column_x,
                                    const unsigned int column_y,
                                    const unsigned int column_z,
                                    const std::string &title)
{
	if (fileExist(filename)) {
        
        fprintf(gnuplotpipe,"plot '%s' using %d:%d:%d pt 7 ps 3 palette title '%s' \n",filename.c_str(),column_x,column_y,column_z,title.c_str());
    }
    else
        throw GnuplotException("file doesn't exist");
    
}

bool Gnuplot::fileExist(const std::string &filename)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    if (_access(filename.c_str(), 0) == 0)
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
        if (access(filename.c_str(), 0) == 0)
#endif
        {
            return true;
        }
    else
        return false;
};




#endif
