// ======================================================================== //
// Copyright 2019-2020 Ingo Wald                                            //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "deviceCode.h"
#include <optix_device.h>
#include "../../owl/include/owl/common/parallel/parallel_for.h"

using namespace owl;

__constant__ MyGlobals optixLaunchParams;

// bounding box programs 
template<typename SphereGeomType>
inline __device__ void boundsProg(const void *geomData,
                                  box3f &primBounds,
                                  const int primID)
{   
  const SphereGeomType &self = *(const SphereGeomType*)geomData;
  const Sphere sphere = self.data_pts[primID];
  primBounds = box3f().extend(sphere.center - self.rad)
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

	if(distance < selfs.rad*selfs.rad){
    NeighGroup &param = owl::getPRD<NeighGroup>();
    int max_idx=0;
    for (int i = 1; i < KN; i++)
    {
      if (param.res[i].dist > param.res[max_idx].dist)
      max_idx = i;
    }
    if ( distance < param.res[max_idx].dist){
      param.res[max_idx].dist = distance;
      param.res[max_idx].ind = primID;
    }
  }
}	


OPTIX_RAYGEN_PROGRAM(rayGen)()
{
  const RayGenData &self = owl::getProgramData<RayGenData>();
  int xID = optixGetLaunchIndex().x;
  NeighGroup param;
  for(int i=0; i<KN; i++){
    param.res[i].ind = -1;
    param.res[i].dist = FLOAT_MAX;
  }
  owl::Ray ray(self.query_pts[xID].center, vec3f(0,0,1), 0, 1.e-16f);
  owl::traceRay(self.world, ray, param);

  for(int i=0; i<KN; i++){
    optixLaunchParams.frameBuffer[xID*KN+i].ind = param.res[i].ind;
    optixLaunchParams.frameBuffer[xID*KN+i].dist = param.res[i].dist;
  }
}
