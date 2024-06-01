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
