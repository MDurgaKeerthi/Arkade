Source code for paper Arkade: k-Nearest Neighbor Search With Non-Euclidean Distances using GPU Ray Tracing. 
https://doi.org/10.1145/3650200.3656601

High-performance implementations of k-Nearest Neighbor Search (kNN) in low dimensions use tree-based data structures. Tree algorithms are hard to parallelize on GPUs due to their irregularity. However, newer Nvidia GPUs offer hardware support for tree operations through ray-tracing cores. Recent works have proposed using RT cores to implement kNN search, but they all have a hardware-imposed constraint on the distance metric used in the search—the Euclidean distance. We propose and implement two reductions to support kNN for a broad range of distances other than the Euclidean distance: Arkade Filter-Refine and Arkade Monotone Transformation, each of which allows non-Euclidean distance-based nearest neighbor queries to be performed in terms of the Euclidean distance. With our reductions, we observe that kNN search time speedups range between 1.6x-200x and 1.3x-33.1x over various state-of-the-art GPU shader core and RT core baselines, respectively. In evaluation, we provide several insights on RT architectures’ ability to efficiently build and traverse the tree by analyzing the kNN search time trends.

## To run Arkade

The project is built with Optix 8 and cuda 12.2\
refer to owl (https://github.com/owl-project/owl) for all the dependencies\

```
mkdir build\
cd build\
cmake ../ -DKN=k -DNORM=0\
make s01-knn\
./s01-knn filename npoints nsearchpoints radius_estimate\
```

s01-knn performs knn search within a given radius. Sometimes, a query point may not be able to find all neighbors within this given radius, so we use Trueknn (https://github.com/vani-nag/OWLRayTracing) so that users do not have to worry about tuning the radius parameter for Arkade. s02-withTrueknn contains the combined code. To get the radius estimate, run initial_estimate.py first. num_samples can be as small as 1000.

```
cd src/s02-withTrueknn/
python initial_estimate.py <filename> <num_samples>

cd ../../build/
./sample02-withTrueknn ../datasets/sample_gowalla.txt 1000 10 <radius estimate from running python script>
```


### Dataset format

```./datasets/ ``` folder contains a sample dataset (sample_gowalla.txt) to run Arkade. The description of datasets and the links where we sourced them from are described in the evaluation section of the paper. Additionally, the datasets folder contains scripts to process those datasets in the format Arkade takes as input. The input format is space-separated x, y, and z coordinates of each point, and every point is in a new line. 

```
<x_1 y_1 z_1>
<x_2 y_2 z_2>
...
<x_n y_n z_n>
```
Example:
```text
0.1 0.1 0.1
0.2 0.1 0.2
```


## Publication

Please cite this paper if you find Arkade useful.

```bib
@inproceedings{10.1145/3650200.3656601,
author = {Mandarapu, Durga Keerthi and Nagarajan, Vani and Pelenitsyn, Artem and Kulkarni, Milind},
title = {Arkade: k-Nearest Neighbor Search With Non-Euclidean Distances using GPU Ray Tracing},
year = {2024},
isbn = {9798400706103},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
url = {https://doi.org/10.1145/3650200.3656601},
doi = {10.1145/3650200.3656601},
booktitle = {Proceedings of the 38th ACM International Conference on Supercomputing},
pages = {14–25},
numpages = {12},
keywords = {GPU Ray Tracing, Non-Euclidean Distances, k-Nearest Neighbor Search},
location = {Kyoto, Japan},
series = {ICS '24}
}
```
