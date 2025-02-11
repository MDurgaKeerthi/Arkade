import csv
import math
import numpy as np

with open('Tweets.csv') as f:
    reader = csv.reader(f, delimiter=',')
    file1 = open('tweets_loc.txt', 'w')
    for line in reader: 
        lat =  np.deg2rad(float(line[0]))
        lon =  np.deg2rad(float(line[1]))
        x = math.cos(lat) * math.cos(lon)
        y = math.cos(lat) * math.sin(lon)
        z = math.sin(lat)
        file1.write(str(x)+'\t'+str(y)+'\t'+str(z)+'\n')
    file1.close()
