# python3 pca.py  outputfile_name

import os
import numpy as np
import sys
import csv
from sklearn.utils import shuffle
from sklearn.preprocessing import StandardScaler

filename = sys.argv[1]          
nsearchpoints = 10000 

data = []
with open(filename) as f:
    reader = csv.reader(f, delimiter='\t')
    for line in reader: 
        temp = [float(word) for word in line]
        data.append(temp)

data = np.asarray(data, dtype=np.float32)
# np.random.shuffle(data)

# xb = data[:-nsearchpoints]
# train = StandardScaler().fit_transform(xb)
# xq = data[-nsearchpoints:]
# test = StandardScaler().fit_transform(xq)
# xb = train / np.linalg.norm(train, axis=1)[:, np.newaxis]
# xq = test / np.linalg.norm(test, axis=1)[:, np.newaxis]

# outfile = "norm_" + filename
# with open(outfile, "w") as outFile:
#     for line in xb:
#         outFile.write('%f\t%f\t%f\n' % (line[0], line[1], line[2]))
#     for line in xq:
#         outFile.write('%f\t%f\t%f\n' % (line[0], line[1], line[2]))

# outfile = "norm_" + filename
# with open(outfile, "w") as outFile:
#     for line in xb:
#         outFile.write('%f\t%f\n' % (line[0], line[1]))
#     for line in xq:
#         outFile.write('%f\t%f\n' % (line[0], line[1]))

outfile = "rounded_" + filename
with open(outfile, "w") as outFile:
    for line in data:
        outFile.write('%f\t%f\t%f\n' % (line[0], line[1], line[2]))