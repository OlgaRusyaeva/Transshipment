//
//  array.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 8/16/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Array_h
#define TransshipmentProject_Array_h

namespace arrays {
    //actually 1 dim
    template<typename T>
    class Array2D
    {
    private:
        int width;
        int height;
        T * data;
    public:
        int sizeH()const {return height;}
        int sizeW()const {return width;}
        void initialize() { for(int i=0;i<height;i++){for(int j=0;j<width;j++) data[i*width+j]=0;}}
        T& operator() (int x, int y)const { return data[x*width + y]; }
        const T& operator=(const T& array){this->width=array.width;this->height=array.height; this->data=array.data; return *this;}
        Array2D(const int w, const int h) : width(w), height(h) { data = new T[w*h]; }
        ~Array2D() { delete [] data; }
    };
    
    //actually 1 dim
    //see C++ Multi-dimensional Arrays on the Heap - Stack Overflow
    template<typename T>
    class Array3D
    {
    private:
        int width;
        int height;
        int depth;
        T * data;
    public:
        int sizeH()const {return height;}
        int sizeW()const {return width;}
        int sizeD()const {return depth;}
        void initialize() { for(int i=0;i<height;i++){for(int j=0;j<width;j++) for(int k=0;k<depth;k++){data[(i*width+j)*depth + k]=0;}}}
        T& operator() (int x, int y, int z) { return data[(x*width+y)*depth + z]; }
        const T& operator=(const T& array){this->width=array.width;this->height=array.height;this->depth=array.depth;  this->data=array.data;return *this;}
        Array3D(const int w, const int h, const int d) : width(w), height(h), depth(d){data=new T[w*d*h];}
        ~Array3D(){delete [] data;}
    };
    
    
};





#endif
