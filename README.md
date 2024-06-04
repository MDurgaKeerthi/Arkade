Source code for Arkade: Arkade: k-Nearest Neighbor Search With Non-Euclidean Distances using GPU Ray Tracing.

installation:\
refer to owl for all the dependencies\
mkdir build\
cd build\
cmake ../ -DKN=<number of neighbors> -DNORM=0\
make s01-knn\
./s01-knn filename npoints nsearchpoints radius_estimate\
