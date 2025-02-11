import csv
import math
import numpy as np

with open('stanford-gowalla.csv') as f:
    reader = csv.reader(f, delimiter='\t')
    file1 = open('gowalla_loc.txt', 'w')
    loc_id = set()
    for line in reader: 
        id = line[4]
        if id not in loc_id:
            loc_id.add(id)
            lat =  np.deg2rad(float(line[0]))
            lon =  np.deg2rad(float(line[1]))
            x = math.cos(lat) * math.cos(lon)
            y = math.cos(lat) * math.sin(lon)
            z = math.sin(lat)
            file1.write(str(x)+'\t'+str(y)+'\t'+str(z)+'\n')
    file1.close()
    print(len(loc_id))