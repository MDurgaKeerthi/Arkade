// All rights reserved by 
// Durga Keerthi Mandarapu, Vani Nagarajan, Artem Pelenitsyn, and Milind Kulkarni. 2024. 
// Arkade: k-Nearest Neighbor Search With Non-Euclidean Distances using GPU Ray Tracing. 

// public owl API
#include <owl/owl.h>
#include <owl/DeviceMemory.h>   
#include "deviceCode.h"          


#define FLOAT_MAX 3.402823466e+38
#define FLOAT_MIN 1.175494351e-38

extern "C" char deviceCode_ptx[];

std::vector<Sphere> Spheres_data;
std::vector<Sphere> Spheres_query;
std::vector<Neigh> neighbors;

int main(int ac, char **argv)
{
  std::string line;
  std::ifstream myfile;
	myfile.open(argv[1]);
  int npoints = atoi(argv[2]);
  int nsearchpoints = atoi(argv[3]);
  float radius = atof(argv[4]); 

  if(!myfile.is_open())
  {
    perror("Error open");
    exit(EXIT_FAILURE);
  }
  int linenum = 0;
  while(getline(myfile, line)) 
	{
	  std::stringstream ss(line);
	  float i;
    std::vector<float> vect;
	  while (ss >> i)
	  {
      vect.push_back(i);
      if (ss.peek() == ',')
          ss.ignore();
	  }

    if(linenum < npoints)
      Spheres_data.push_back(Sphere{vec3f(vect[0],vect[1],vect[2])});
      // Spheres_data.push_back(Sphere{vec3f(vect[0],vect[1],0.0f)});
    else
      Spheres_query.push_back(Sphere{vec3f(vect[0],vect[1],vect[2])});
      // Spheres_query.push_back(Sphere{vec3f(vect[0],vect[1],0.0f)});
    
    linenum++;
	}	
  myfile.close();

  if(linenum < (npoints+nsearchpoints)){
    printf("Insufficient file size\n");
    return 0;
  }

	//Init neighbors array
	for(int j=0; j<nsearchpoints; j++){
    for(int i = 0; i < KN; i++)
		  neighbors.push_back(Neigh{-1,FLOAT_MAX});
  }	
  vector<int> n_neighbors(nsearchpoints, 0);

  std::cout << radius << std::endl;
  std::cout << KN << std::endl;
  std::cout << npoints << std::endl;
  std::cout << nsearchpoints << std::endl;

	//Frame Buffer -- just one thread for a single query point
	const int fbSize = nsearchpoints;

  // init owl

  OWLContext context = owlContextCreate(nullptr,1);
  OWLModule  module  = owlModuleCreate(context,deviceCode_ptx);

  // set up all the *GEOMETRY* graph we want to render
  OWLVarDecl SpheresGeomVars[] = {
    { "data_pts",  OWL_BUFPTR, OWL_OFFSETOF(SpheresGeom,data_pts)},
    { "rad", OWL_FLOAT, OWL_OFFSETOF(SpheresGeom,rad)},
    { /* sentinel to mark end of list */ }
  };

  OWLGeomType SpheresGeomType
    = owlGeomTypeCreate(context,
                        OWL_GEOMETRY_USER,
                        sizeof(SpheresGeom),
                        SpheresGeomVars,-1);
  owlGeomTypeSetIntersectProg(SpheresGeomType,0,
                              module,"Spheres");
  owlGeomTypeSetBoundsProg(SpheresGeomType,
                           module,"Spheres");

  owlBuildPrograms(context);

  OWLBuffer frameBuffer
    = owlHostPinnedBufferCreate(context,OWL_USER_TYPE(neighbors[0]),
                            neighbors.size());

  // vector<long long int> intersecs(nsearchpoints, 0);
  // OWLBuffer intersecBuffer
  //   = owlManagedMemoryBufferCreate(context,OWL_USER_TYPE(intersecs[0]),
  //                           intersecs.size(), intersecs.data());
                            
  OWLBuffer n_neighBuffer
    = owlHostPinnedBufferCreate(context, OWL_INT,
                            n_neighbors.size());                      
 
  OWLBuffer dataSpheresBuffer
    = owlDeviceBufferCreate(context,OWL_USER_TYPE(Spheres_data[0]),
                            Spheres_data.size(),Spheres_data.data());

  OWLBuffer querySpheresBuffer
    = owlDeviceBufferCreate(context,OWL_USER_TYPE(Spheres_query[0]),
                            Spheres_query.size(),Spheres_query.data());

  OWLGeom SpheresGeom
    = owlGeomCreate(context,SpheresGeomType);
  owlGeomSetPrimCount(SpheresGeom,Spheres_data.size());
  owlGeomSetBuffer(SpheresGeom,"data_pts",dataSpheresBuffer);
  owlGeomSet1f(SpheresGeom,"rad",radius);

  // Params
  int round = 0;

  OWLVarDecl myGlobalsVars[] = {
	{"frameBuffer", OWL_BUFPTR, OWL_OFFSETOF(MyGlobals, frameBuffer)},
  {"num_neighbors", OWL_BUFPTR, OWL_OFFSETOF(MyGlobals, num_neighbors)},
	{"round", OWL_INT, OWL_OFFSETOF(MyGlobals, round)},
  // {"intersections", OWL_BUFPTR, OWL_OFFSETOF(MyGlobals, intersections)},
	{ /* sentinel to mark end of list */ }
	};

	OWLParams lp = owlParamsCreate(context,sizeof(MyGlobals),myGlobalsVars,-1);
	owlParamsSetBuffer(lp,"frameBuffer",frameBuffer);	
  owlParamsSetBuffer(lp,"num_neighbors",n_neighBuffer);	
	owlParamsSet1i(lp,"round",round);		
  // owlParamsSetBuffer(lp,"intersections",intersecBuffer);

  OWLGeom  userGeoms[] = {
    SpheresGeom
  };

	auto start_b = std::chrono::steady_clock::now();
  OWLGroup spheresGroup
    = owlUserGeomGroupCreate(context,1,userGeoms 
                            ,OPTIX_BUILD_FLAG_ALLOW_UPDATE
                            );
  owlGroupBuildAccel(spheresGroup);

  OWLGroup world
    = owlInstanceGroupCreate(context,1,&spheresGroup
                            ,nullptr, nullptr,OWL_MATRIX_FORMAT_OWL,
                            OPTIX_BUILD_FLAG_ALLOW_UPDATE
                            );
  owlGroupBuildAccel(world);

  auto end_b = std::chrono::steady_clock::now();
	auto elapsed_b = std::chrono::duration_cast<std::chrono::microseconds>(end_b - start_b);
	std::cout << elapsed_b.count()/1000000.0 << std::endl;

	// set up ray gen program
	OWLVarDecl rayGenVars[] = {
    {"query_pts",       OWL_BUFPTR, OWL_OFFSETOF(RayGenData,query_pts)},
		{ "world",         OWL_GROUP,  OWL_OFFSETOF(RayGenData,world)},	
		{ /* sentinel to mark end of list */ }
	};

	OWLRayGen rayGen
		= owlRayGenCreate(context,module,"rayGen",
			                sizeof(RayGenData),
			                rayGenVars,-1);

  // ----------- set variables  ----------------------------
  owlRayGenSetBuffer(rayGen,"query_pts",        querySpheresBuffer);
	owlRayGenSetGroup (rayGen,"world",        world);
	
  // build shader binding table required to trace the groups
  owlBuildPrograms(context);
	owlBuildPipeline(context);
  owlBuildSBT(context);

  int repeat = 1;
  // int in_rounds = atoi(argv[6]);
  auto start = std::chrono::steady_clock::now();	 
  double overhead_time; 
  // while(repeat && round < in_rounds)
  while(repeat)
  {
    // printf("round: %d\n", round);
    owlLaunch2D(rayGen,nsearchpoints,1,lp);

    const int *nb = (const int*)owlBufferGetPointer(n_neighBuffer,0);
    repeat = 0;
    for(int q=0; q<nsearchpoints; q++)
    {
      if (nb[q] < KN)
      {
        repeat = 1;
        
        round++;
        owlParamsSet1i(lp,"round",round);		
        radius *= 2;
        owlGeomSet1f(SpheresGeom, "rad", radius);
        owlGroupRefitAccel(spheresGroup);
        owlGroupRefitAccel(world);
        break;
      }
    }
  }

  auto end = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << elapsed.count()/1000000.0<< std::endl;

  // printf("Complete Search, writing output to file...\n");
  // const Neigh *fb = (const Neigh*)owlBufferGetPointer(frameBuffer,0);
  // std::ofstream outfile;
  // outfile.open(argv[5]);
  // // std::cout<<setprecision(6);
	// for(int j=0; j<nsearchpoints; j++){
  //   for(int i = 0; i < KN; i++)
  //   {            
  //     outfile<<fb[j*KN+i].ind<<'\t'<<fb[j*KN+i].dist<<endl;
  //   }
  // }
  // outfile.close();

  // and finally, clean up
  owlContextDestroy(context);

}
