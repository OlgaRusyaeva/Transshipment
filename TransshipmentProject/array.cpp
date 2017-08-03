//
//  Array.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

namespace arrays {
    //actually 1 dim
    template<typename T>
    class Array2DInt
    {
    private:
        const int width;
        const int height;
        T * data;
    public:
        int size() {return height;}
        void initialize() { for(int i=0;i<height;i++){for(int j=0;j<width;j++) data[i*width+j]=0;}}
        T& operator() (int x, int y) { return data[x*width + y]; }
        const T& operator=(const T& array){this->width=array.width;this->height=array.height; return *this;}
        Array2DInt(const int w, const int h) : width(w), height(h) { data = new T[w*h]; }
        ~Array2DInt() { delete [] data; }
    };
    
    //actually 2 dim
    //see C++ Multi-dimensional Arrays on the Heap - Stack Overflow
    template<typename T>
    class Array3DInt
    {
    private:
        const int width;
        const int height;
        const int depth;
        T ** data;
    public:
        T& operator() (int x, int y, int z) { return data[x][y*width + z]; }
        const T& operator=(const T& array){this->width=array.width;this->height=array.height;this->depth=array.depth; return *this;}
        Array3DInt(const int w, const int h, const int d) : width(w), height(h), depth(d) { data=new T*[w]; data[0]=new T[h*d];}
        ~Array3DInt() { delete [] data[0]; delete [] data; }
    };
    

};


