

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <set>
#include <iomanip>
#include <ios>
#include <thread>
#include <vector>
#include <cmath>
#include <stdio.h>
#define FLOAT_MIN 1.175494351e-38
#define FLOAT_MAX 3.402823466e+38

using namespace std;

std::vector<int> vect;
std::vector<int> neigh;
std::vector<int> neigh_d;
int npoints;
int nsearchpoints;
int k;
int lpnorm;
int thread_num = std::thread::hardware_concurrency();

void findn(int qID){
  std::vector<float> dists(k, FLOAT_MAX);
  std::vector<int> inds(k, -1);
  int n3 = npoints*3;
  float q[3];
  q[0] = vect[n3 + qID*3 ];
  q[1] = vect[n3 + qID*3 + 1];
  q[2] = vect[n3 + qID*3 + 2];
  
  for(int i=0; i<npoints; i++)
  {
    long int dist = 1000000000000 - (long int)vect[i*3]*(long int)q[0]
                - (long int)vect[(i*3)+1]*(long int)q[1]
                - (long int)vect[(i*3)+2]*(long int)q[2];

    // float dist = std::pow(std::abs(vect[i*3]-q[0]), lpnorm)
    //             + std::pow(std::abs(vect[(i*3)+1]-q[1]),lpnorm)
    //             + std::pow(std::abs(vect[(i*3)+2]-q[2]),lpnorm);

    // float dist;
    // double x = std::abs(vect[i*3]-q[0]);
    // double y = std::abs(vect[(i*3)+1]-q[1]);
    // double z = std::abs(vect[(i*3)+2]-q[2]);
    // if(x > y )
    //   dist = x;
    // else
    //   dist = y;
    // if(dist < z)
    //   dist = z;

    int max_idx=0;
    for (int i1 = 1; i1 < k; i1++)
    {
      if (dists[i1] > dists[max_idx])
        max_idx = i1;
    }
    if (dist < dists[max_idx])
    {
      dists[max_idx] = dist;
      inds[max_idx] = i;
    }
  }

  //sort distances

  for(int i=0; i<k; i++){
    neigh[(qID*k)+i] = inds[i];
    neigh_d[(qID*k)+i] = dists[i];
    // printf("here %d\n", inds[i]);
    }
  
  return;
}


void thdwork(int thdid)
{
  int workload =  ceil(nsearchpoints/(float)thread_num);
  int start = thdid*workload;
  
  for (int i=start; i<start+workload && i<nsearchpoints; i++)
  {
    findn(i);
    // printf("thdid %d, qid: %d\n", thdid, i);
  }
}

int main(int ac, char **argv)
{
  

  std::string line;
  std::ifstream myfile;
	myfile.open(argv[1]);

  if(!myfile.is_open())
  {
    perror("Error open");
    exit(EXIT_FAILURE);
  }
  
  while(getline(myfile, line)) 
	{
	  std::stringstream ss(line);
	  float i;
	  while (ss >> i)
	  {
      vect.push_back(1000000*i);
      if (ss.peek() == ',')
          ss.ignore();
	  }
	}	
  myfile.close();

  k = atoi(argv[2]);
  npoints = atoi(argv[3]);
  nsearchpoints = atoi(argv[4]);
  lpnorm = atoi(argv[5]);
  neigh.resize(nsearchpoints*k, -1);
  neigh_d.resize(nsearchpoints*k, -1);
  
  auto start_b = std::chrono::steady_clock::now();
  thread thds[thread_num];
  for(int i=0; i<thread_num; i++)
  {
    thds[i] = thread(&thdwork, i);
  }

  for(int i=0; i<thread_num; i++)
  {
    thds[i].join();
  }
  auto end_b = std::chrono::steady_clock::now();
	auto elapsed_b = std::chrono::duration_cast<std::chrono::microseconds>(end_b - start_b);
	// std::cout << elapsed_b.count()/1000000.0 << std::endl;

  // float t = 1000000 - 

  std::ofstream outfile;
  // string oname = "l"+ std::to_string(lpnorm) +"truth"+ std::to_string(nsearchpoints/1000) +"_" + string(argv[1]);
  string oname = "dottruth_1000_" + string(argv[1]);
  // string oname = "dottruth_glove.txt";
  outfile.open(oname.c_str());
	for(int j=0; j<nsearchpoints*k; j++)
  {
    // cout<<neigh[j]<<endl;
    outfile<<neigh[j]<<'\t'<<neigh_d[j]<<endl;
  }
  outfile.close();

  return 0;
}