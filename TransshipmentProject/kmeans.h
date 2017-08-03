#ifndef KMEANS_H

#define KMEANS_H

// This code snippets was written by John W. Ratcliff on December 1, 2010
// http://codesuppository.blogspot.de/2010/12/k-means-clustering-algorithm.html
//
// This code snippet is open source under the MIT license.
//
// Feedback to: jratcliffscarab@gmail.com
//
// This piece of sample code implements the basic 'k-means' clustering algorithm.
//
// In simple terms, it means it will take a collection of data points and want to group them into the
// (N) most significant 'clusters'.
//
// The K-Means algorithm is a bit brute force and there are more elegant solutions such as building an octree out of the
// input data set.  However, K-means works, is very straightforward and easy to understand.
//
// This algorithm starts out making a guess from the set of input data points what the best clumps/clusters might be.
// It then iterates through each input point and figures out which clump that point is closest too; remembering the distance error
// After one pass it has an accmulated error of the distance of every point relative to that points's cluster.
// It also moves the centroid of the cluster to be the average distance of all of the points assigned to that cluster.
// It continues doing this until the distance error stops converging.
//
// The output is a set of cluster points and an array of indices tell you which clump each of your original input points is now assigened too.
//
// This code also takes care of pruning, which the standard implementations of k-means does not.
//
// Let's say you have 100 data points that you want to put into 10 clusters.  However, all of those data points are right next to each other.
// The standard K-means algorithm will still produce 10 clusters.  However, this implementation allows you to specify a tolerance threshold
// that says if two clusters are practically next to each other, simply merge them.
//
// The code snippet is designed to be easy to modify for your own use and extremely clear for understand.
//
// Provides function prototypes to compute keman clusters on 1d, 2d, 3d, and 4d data for both doubles and floats
//
// The implmenentation uses a function template, which also might be useful for developers to learn from.
//
// Assumes that your input data and output data has a stride equal to the point size.
//
// Example:  const float *inputPoints3d = x1,y1,z1, x2,y2,z2, ....

// ** References
// ** - J. MacQueen, "Some methods for classification and analysis
// **   of multivariate observations", Fifth Berkeley Symposium on
// **   Math Statistics and Probability, 281-297, 1967.
// ** - I.S. Dhillon and D.S. Modha, "A data-clustering algorithm
// **   on distributed memory multiprocessors",
// **   Large-Scale Parallel Data Mining, 245-260, 1999.

namespace KMEANS
{
    
    // Returns the number of clumps, which can be less than the 'clumpCount' specified.
    // For example, if you asked for 10 clumps, but only passed in 5 data points, then you will only get 5 clumps out.
    // Also, based on the 'collapseDistance' (if it is non-zero) it may collapse clumps if they are very close together.
    size_t	kmeans_cluster1d(const float *input,				// an array of input 1d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             float	*outputClusters,		// The output array of clumps 1d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             float errorThreshold=0.01f,	// The error threshold to converge towards before giving up.
                             float collapseDistance=0.01f); // distance so small it is not worth bothering to create a new clump.
    
    size_t	kmeans_cluster2d(const float *input,				// an array of input 2d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             float	*outputClusters,		// The output array of clumps 2d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             float errorThreshold=0.01f,	// The error threshold to converge towards before giving up.
                             float collapseDistance=0.01f); // distance so small it is not worth bothering to create a new clump.
    
    size_t	kmeans_cluster3d(const float *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             float	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             float errorThreshold=0.01f,	// The error threshold to converge towards before giving up.
                             float collapseDistance=0.01f); // distance so small it is not worth bothering to create a new clump.
    
    size_t	kmeans_cluster4d(const float *input,				// an array of input 4d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             float	*outputClusters,		// The output array of clumps 4d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             float errorThreshold=0.01f,	// The error threshold to converge towards before giving up.
                             float collapseDistance=0.01f); // distance so small it is not worth bothering to create a new clump.
    
    size_t	kmeans_cluster1d(const double *input,				// an array of input 1d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             double	*outputClusters,		// The output array of clumps 1d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             double errorThreshold=0.01f,	// The error threshold to converge towards before giving up.
                             double collapseDistance=0.01f); // distance so small it is not worth bothering to create a new clump.
    
    size_t	kmeans_cluster2d(const double *input,				// an array of input 2d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             double	*outputClusters,		// The output array of clumps 2d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             double errorThreshold=0.01f,	// The error threshold to converge towards before giving up.
                             double collapseDistance=0.01f); // distance so small it is not worth bothering to create a new clump.
    
    size_t	kmeans_cluster3d(const double *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             double	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             double errorThreshold=0.01f,	// The error threshold to converge towards before giving up.
                             double collapseDistance=0.01f); // distance so small it is not worth bothering to create a new clump.
    
    size_t	kmeans_cluster4d(const double *input,				// an array of input 4d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             double	*outputClusters,		// The output array of clumps 4d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             double errorThreshold=0.01f,	// The error threshold to converge towards before giving up.
                             double collapseDistance=0.01f); // distance so small it is not worth bothering to create a new clump.
    
    
}; // end of the Kmeans namespace

#endif