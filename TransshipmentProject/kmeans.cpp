#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include <math.h>

#include "kmeans.h"

namespace KMEANS
{
    
    template <class Type> struct Vec1d
    {
        inline Type distanceSquared(const Vec1d &a) const
        {
            Type dx = x-a.x;
            return dx*dx;
        }
        
        inline void operator+=(const Vec1d &v)
        {
            x+=v.x;
        }
        inline void operator*=(const Type v)
        {
            x*=v;
        }
        inline void zero(void) { x = 0; };
        Type x;
    };
    
    template <class Type > struct Vec2d
    {
        inline Type distanceSquared(const Vec2d &a) const
        {
            Type dx = x-a.x;
            Type dy = y-a.y;
            return dx*dx+dy*dy;
        }
        inline void operator+=(const Vec2d &v)
        {
            x+=v.x;
            y+=v.y;
        }
        inline void operator*=(const Type v)
        {
            x*=v;
            y*=v;
        }
        inline void zero(void) { x = 0; y = 0; };
        
        Type x;
        Type y;
    };
    
    template <class Type> struct Vec3d
    {
        inline Type distanceSquared(const Vec3d &a) const
        {
            Type dx = x-a.x;
            Type dy = y-a.y;
            Type dz = z-a.z;
            return dx*dx+dy*dy+dz*dz;
        }
        inline void operator+=(const Vec3d &v)
        {
            x+=v.x;
            y+=v.y;
            z+=v.z;
        }
        inline void operator*=(const Type v)
        {
            x*=v;
            y*=v;
            z*=v;
        }
        inline void zero(void) { x = 0; y = 0; z = 0; };
        
        Type x;
        Type y;
        Type z;
    };
    
    template <class Type> struct Vec4d
    {
        inline Type distanceSquared(const Vec4d &a) const
        {
            Type dx = x-a.x;
            Type dy = y-a.y;
            Type dz = z-a.z;
            Type dw = w-a.w;
            return dx*dx+dy*dy+dz*dz+dw*dw;
        }
        inline void operator+=(const Vec4d &v)
        {
            x+=v.x;
            y+=v.y;
            z+=v.z;
            w+=v.w;
        }
        inline void operator*=(const Type v)
        {
            x*=v;
            y*=v;
            z*=v;
            z*=v;
        }
        inline void zero(void) { x = 0; y = 0; z = 0; w = 0; };
        
        Type x;
        Type y;
        Type z;
        Type w;
    };
    
    template <class Vec,class Type >
    size_t	kmeans_cluster(const Vec *input,
                           size_t inputCount,
                           size_t clumpCount,
                           Vec *clusters,
                           size_t *outputIndices,
                           Type threshold, // controls how long it works to converge towards a least errors solution.
                           Type collapseDistance) // distance between clumps to consider them to be essentially equal.
    {
        size_t convergeCount = 64; // maximum number of iterations attempting to converge to a solution..
        //size_t ret = inputCount;
        size_t *counts = new size_t[clumpCount];
        Type error=0;
        if ( inputCount <= clumpCount ) // if the number of input points is less than our clumping size, just return the input points.
        {
            clumpCount = inputCount;
            for (size_t i=0; i<inputCount; i++)
            {
                outputIndices[i] = i;
                clusters[i] = input[i];
                counts[i] = 1;
            }
        }
        else
        {
            Vec *centroids = new Vec[clumpCount];
            
            // Take a sampling of the input points as initial centroid estimates.
            for (size_t i=0; i<clumpCount; i++)
            {
                size_t index = (i*inputCount)/clumpCount;
                assert( index >= 0 && index < inputCount );
                clusters[i] = input[index];
            }
            
            // Here is the main convergence loop
            Type old_error = FLT_MAX;	// old and initial error estimates are max Type
            error = FLT_MAX;
            do
            {
                old_error = error;	// preserve the old error
                // reset the counts and centroids to current cluster location
                for (size_t i=0; i<clumpCount; i++)
                {
                    counts[i] = 0;
                    centroids[i].zero();
                }
                error = 0;
                // For each input data point, figure out which cluster it is closest too and add it to that cluster.
                for (size_t i=0; i<inputCount; i++)
                {
                    Type min_distance = FLT_MAX;
                    // find the nearest clump to this point.
                    for (size_t j=0; j<clumpCount; j++)
                    {
                        Type distance = input[i].distanceSquared( clusters[j] );
                        if ( distance < min_distance )
                        {
                            min_distance = distance;
                            outputIndices[i] = j; // save which clump this point indexes
                        }
                    }
                    size_t index = outputIndices[i]; // which clump was nearest to this point.
                    centroids[index]+=input[i];
                    counts[index]++;	// increment the counter indicating how many points are in this clump.
                    error+=min_distance; // save the error accumulation
                }
                // Now, for each clump, compute the mean and store the result.
                for (size_t i=0; i<clumpCount; i++)
                {
                    if ( counts[i] ) // if this clump got any points added to it...
                    {
                        Type recip = 1.0f / (Type)counts[i];	// compute the average (center of those points)
                        centroids[i]*=recip;	// compute the average center of the points in this clump.
                        clusters[i] = centroids[i]; // store it as the new cluster.
                    }
                }
                // decrement the convergence counter and bail if it is taking too long to converge to a solution.
                convergeCount--;
                if (convergeCount == 0 )
                {
                    break;
                }
                if ( error < threshold ) // early exit if our first guess is already good enough (if all input points are the same)
                    break;
            } while ( fabs(error - old_error) > threshold ); // keep going until the error is reduced by this threshold amount.
            
            delete []centroids;
        }
        
        // ok..now we prune the clumps if necessary.
        // The rules are; first, if a clump has no 'counts' then we prune it as it's unused.
        // The second, is if the centroid of this clump is essentially  the same (based on the distance tolerance)
        // as an existing clump, then it is pruned and all indices which used to point to it, now point to the one
        // it is closest too.
        size_t outCount = 0; // number of clumps output after pruning performed.
        Type d2 = collapseDistance*collapseDistance; // squared collapse distance.
        for (size_t i=0; i<clumpCount; i++)
        {
            if ( counts[i] == 0 ) // if no points ended up in this clump, eliminate it.
                continue;
            // see if this clump is too close to any already accepted clump.
            bool add = true;
            size_t remapIndex = outCount; // by default this clump will be remapped to its current index.
            for (size_t j=0; j<outCount; j++)
            {
                Type distance = clusters[i].distanceSquared(clusters[j]);
                if ( distance < d2 )
                {
                    remapIndex = j;
                    add = false; // we do not add this clump
                    break;
                }
            }
            // If we have fewer output clumps than input clumps so far, then we need to remap the old indices to the new ones.
            if ( outCount != i || !add ) // we need to remap indices!  everything that was index 'i' now needs to be remapped to 'outCount'
            {
                for (size_t j=0; j<inputCount; j++)
                {
                    if ( outputIndices[j] == i )
                    {
                        outputIndices[j] = remapIndex; // 
                    }
                }
            }
            if ( add )
            {
                clusters[outCount] = clusters[i];
                outCount++;
            }
        }
        delete []counts;
        clumpCount = outCount;
        return clumpCount;
    };
    
    size_t	kmeans_cluster1d(const float *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             float	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             float errorThreshold,	// The error threshold to converge towards before giving up.
                             float collapseDistance) // distance so small it is not worth bothering to create a new clump.
    {
        const Vec1d< float > *_input = (const Vec1d<float> *)input;
        Vec1d<float> *_output = (Vec1d<float> *)outputClusters;
        return kmeans_cluster< Vec1d<float>, float >(_input,inputSize,clumpCount,_output,outputIndices,errorThreshold,collapseDistance);
    }
    
    size_t	kmeans_cluster2d(const float *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             float	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             float errorThreshold,	// The error threshold to converge towards before giving up.
                             float collapseDistance) // distance so small it is not worth bothering to create a new clump.
    {
        const Vec2d< float > *_input = (const Vec2d<float> *)input;
        Vec2d<float> *_output = (Vec2d<float> *)outputClusters;
        return kmeans_cluster< Vec2d<float>, float >(_input,inputSize,clumpCount,_output,outputIndices,errorThreshold,collapseDistance);
    }
    
    size_t	kmeans_cluster3d(const float *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             float	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             float errorThreshold,	// The error threshold to converge towards before giving up.
                             float collapseDistance) // distance so small it is not worth bothering to create a new clump.
    {
        const Vec3d< float > *_input = (const Vec3d<float> *)input;
        Vec3d<float> *_output = (Vec3d<float> *)outputClusters;
        return kmeans_cluster< Vec3d<float>, float >(_input,inputSize,clumpCount,_output,outputIndices,errorThreshold,collapseDistance);
    }
    
    
    size_t	kmeans_cluster4d(const float *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             float	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             float errorThreshold,	// The error threshold to converge towards before giving up.
                             float collapseDistance) // distance so small it is not worth bothering to create a new clump.
    {
        const Vec4d< float > *_input = (const Vec4d<float> *)input;
        Vec4d<float> *_output = (Vec4d<float> *)outputClusters;
        return kmeans_cluster< Vec4d<float>, float >(_input,inputSize,clumpCount,_output,outputIndices,errorThreshold,collapseDistance);
    }
    
    size_t	kmeans_cluster1d(const double *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             double	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             double errorThreshold,	// The error threshold to converge towards before giving up.
                             double collapseDistance) // distance so small it is not worth bothering to create a new clump.
    {
        const Vec1d< double > *_input = (const Vec1d<double> *)input;
        Vec1d<double> *_output = (Vec1d<double> *)outputClusters;
        return kmeans_cluster< Vec1d<double>, double >(_input,inputSize,clumpCount,_output,outputIndices,errorThreshold,collapseDistance);
    }
    
    size_t	kmeans_cluster2d(const double *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             double	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             double errorThreshold,	// The error threshold to converge towards before giving up.
                             double collapseDistance) // distance so small it is not worth bothering to create a new clump.
    {
        const Vec2d< double > *_input = (const Vec2d<double> *)input;
        Vec2d<double> *_output = (Vec2d<double> *)outputClusters;
        return kmeans_cluster< Vec2d<double>, double >(_input,inputSize,clumpCount,_output,outputIndices,errorThreshold,collapseDistance);
    }
    
    size_t	kmeans_cluster3d(const double *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             double	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             double errorThreshold,	// The error threshold to converge towards before giving up.
                             double collapseDistance) // distance so small it is not worth bothering to create a new clump.
    {
        const Vec3d< double > *_input = (const Vec3d<double> *)input;
        Vec3d<double> *_output = (Vec3d<double> *)outputClusters;
        return kmeans_cluster< Vec3d<double>, double >(_input,inputSize,clumpCount,_output,outputIndices,errorThreshold,collapseDistance);
    }
    
    
    size_t	kmeans_cluster4d(const double *input,				// an array of input 3d data points.
                             size_t inputSize,				// the number of input data points.
                             size_t clumpCount,				// the number of clumps you wish to product.
                             double	*outputClusters,		// The output array of clumps 3d vectors, should be at least 'clumpCount' in size.
                             size_t	*outputIndices,			// A set of indices which remaps the input vertices to clumps; should be at least 'inputSize'
                             double errorThreshold,	// The error threshold to converge towards before giving up.
                             double collapseDistance) // distance so small it is not worth bothering to create a new clump.
    {
        const Vec4d< double > *_input = (const Vec4d<double> *)input;
        Vec4d<double> *_output = (Vec4d<double> *)outputClusters;
        return kmeans_cluster< Vec4d<double>, double >(_input,inputSize,clumpCount,_output,outputIndices,errorThreshold,collapseDistance);
    }
    
    
}; // end of namespace