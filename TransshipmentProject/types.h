//
//  types.h
//  


#ifndef project2_Utility_h
#define project2_Utility_h

#include <vector>
#include <list>
#include <boost/random.hpp>

typedef std::vector<float> row_t;
typedef std::vector<row_t> matrix_t;

typedef std::vector<int> vector_t;
typedef std::vector<vector_t> matrixInt_t;

typedef std::vector<unsigned long long> vector_long;
typedef std::vector<vector_long> matrixLong_t;

typedef std::vector<matrixInt_t> vecMatrixInt;
typedef std::vector<matrix_t> vecMatrixFloat;

typedef std::list<std::pair<int,float> > list_t;

typedef boost::mt19937 base_gen_type; // linear congruential generator
typedef boost::uniform_int<> uniform_int_AB; // the distribution (an int from A to B)
typedef boost::variate_generator<base_gen_type&, uniform_int_AB> gen_uniform_int_AB; // create a callable object

typedef boost::uniform_real<> uniform_real_AB;
typedef boost::variate_generator<base_gen_type&, uniform_real_AB > gen_uniform_real_AB;





#endif
