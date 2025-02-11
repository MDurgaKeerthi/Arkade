// All rights reserved 

#pragma once

#include <owl/owl.h>
#include <owl/common/math/AffineSpace.h>
#include <owl/common/math/random.h>

#define FLOAT_MIN 1.175494351e-38
#define FLOAT_MAX 3.402823466e+38
// #define KN 10
using namespace owl;
using namespace std;

namespace owl {

	typedef struct maximumDistanceLogger
	{
		int ind;
		float dist;
	}Neigh;
	
  // ==================================================================
  /* the raw geometric shape of a sphere */
  // ==================================================================
  struct Sphere {
    vec3f center;
  };

  struct SpheresGeom {
    Sphere *data_pts;
    float rad;
  };

  // ==================================================================
  /* and finally, input for raygen */
  // ==================================================================
  struct RayGenData
  {
    OptixTraversableHandle world;
    Sphere *query_pts; 
  };

  struct MyGlobals 
  {	
    Neigh *frameBuffer;
    int k;
  };

  struct NeighGroup
  {
		Neigh res[KN];
	};

}
