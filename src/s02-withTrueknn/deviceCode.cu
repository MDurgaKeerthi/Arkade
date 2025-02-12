// All rights reserved by 
// Durga Keerthi Mandarapu, Vani Nagarajan, Artem Pelenitsyn, and Milind Kulkarni. 2024. 
// Arkade: k-Nearest Neighbor Search With Non-Euclidean Distances using GPU Ray Tracing. 

#include "deviceCode.h"
#include <optix_device.h>

using namespace owl;

__constant__ MyGlobals optixLaunchParams;

// bounding box program
template<typename SphereGeomType>
inline __device__ void boundsProg(const void *geomData,
                                  box3f &primBounds,
                                  const int primID)
{   
  const SphereGeomType &self = *(const SphereGeomType*)geomData;
  const Sphere sphere = self.data_pts[primID];
	primBounds = box3f()
		.extend(sphere.center - self.rad)
		.extend(sphere.center + self.rad);	
}


OPTIX_BOUNDS_PROGRAM(Spheres)(const void  *geomData,
  box3f       &primBounds,
  const int    primID)
{ boundsProg<SpheresGeom>(geomData,primBounds,primID); }

// intersect program
OPTIX_INTERSECT_PROGRAM(Spheres)()
{ 
	
	const int primID = optixGetPrimitiveIndex();
	const SpheresGeom &selfs = owl::getProgramData<SpheresGeom>();
	Sphere self = selfs.data_pts[primID];
	const vec3f org = optixGetWorldRayOrigin();		
  NeighGroup &param = owl::getPRD<NeighGroup>();
  
  // optixLaunchParams.intersections[qID]++;

  int check = 0;
  if(optixLaunchParams.round > 0 )
  {
    for (int i = 0; i < KN; i++) {
      if (param.res[i].ind == primID) {
        check = 1;
        break;
      }
    }
  }
	
  if (check == 0)
  {
    float distance = 0.0;
    #if (NORM == -1)  //dot
      distance = 1.00 - self.center.x * org.x
                - self.center.y * org.y
                - self.center.z * org.z;
    #elif (NORM == 0) //linfty
      double x  = std::abs(self.center.x - org.x);
      double y = std::abs(self.center.y - org.y);
      double z = abs(self.center.z - org.z);
      if(x > y )
        distance = x;
      else
        distance = y;
      if(distance < z)
        distance = z;
    #elif (NORM > 0)
        distance = std::pow(std::abs(self.center.x - org.x), NORM) 
          + std::pow(std::abs(self.center.y - org.y), NORM)
          + std::pow(std::abs(self.center.z - org.z), NORM);
    #endif	


    int max_idx=0;
    for (int i = 1; i < KN; i++)
    {
      if (param.res[i].dist > param.res[max_idx].dist)
        max_idx = i;
    }
    if (distance < param.res[max_idx].dist){
      param.res[max_idx].dist = distance;
      param.res[max_idx].ind = primID;
    }
  }

}	


OPTIX_RAYGEN_PROGRAM(rayGen)()
{
  
  int xID = optixGetLaunchIndex().x;
  
  if(optixLaunchParams.num_neighbors[xID] < KN)
  {
    const RayGenData &self = owl::getProgramData<RayGenData>();    
    NeighGroup param;
    for(int i=0; i<KN; i++)
    {
      param.res[i].ind = -1;
      param.res[i].dist = FLOAT_MAX;
    }
    owl::Ray ray(self.query_pts[xID].center, vec3f(0,0,1), 0, 1.e-16f);
    owl::traceRay(self.world, ray, param);

    int num_neighbors = 0; 
    for(int i=0; i<KN; i++){
      
      if (param.res[i].ind != -1)
      {
        num_neighbors++;
        optixLaunchParams.frameBuffer[xID*KN+i].ind = param.res[i].ind;
        optixLaunchParams.frameBuffer[xID*KN+i].dist = param.res[i].dist;
      }  
    }
    optixLaunchParams.num_neighbors[xID] = num_neighbors;
  }
  
}
