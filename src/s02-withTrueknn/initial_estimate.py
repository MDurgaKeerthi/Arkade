import pandas as pd
import time
import sys
import csv
import numpy as np

filename = sys.argv[1]
npoints = int(sys.argv[2])

# Read inpout file. We use 3D datasets. Modify nrows to choose number of points to read
df = pd.read_csv(filename,sep='\t',header=None, nrows=npoints)
list1 = df.values.tolist()

#Choose random samples
from random import sample

start = time.time()
samples = sample(list1,100)
sample_time = time.time()-start
print("Sampling time: "+str(sample_time))

from sklearn.neighbors import NearestNeighbors
import numpy as np
X = np.array(samples)

#Run KNN
start = time.time()
nbrs = NearestNeighbors(n_neighbors=2, algorithm='ball_tree', metric='l1').fit(X)
# nbrs = NearestNeighbors(n_neighbors=2, algorithm='ball_tree').fit(X)
knn_time = time.time()-start
# print("KNN time: "+str(knn_time))

#Find min distance
start = time.time()
distances, indices = nbrs.kneighbors(X)


# index_min = np.argmin(distances, axis=0)[1]
# x2 = indices[index_min][1]
# mindot = 1- (X[index_min][0]*X[x2][0]) - (X[index_min][1]*X[x2][1]) - (X[index_min][2]*X[x2][2])
# print(mindot)

min1 = min(distances, key=lambda x: x[1])[1]
print("radius estimate = "+str(min1))
# print("Total time: "+str(time.time()-start+knn_time+sample_time))

