import csv
import math
import numpy as np

with open('gbif.csv') as f:
    reader = csv.reader(f, delimiter='\t', quoting=csv.QUOTE_NONE)
    next(reader)
    file1 = open('gbif_loc.txt', 'w')
    for line in reader: 
        lat =  np.deg2rad(float(line[21]))
        lon =  np.deg2rad(float(line[22]))
        x = math.cos(lat) * math.cos(lon)
        y = math.cos(lat) * math.sin(lon)
        z = math.sin(lat)
        file1.write(str(x)+'\t'+str(y)+'\t'+str(z)+'\n')
    file1.close()
