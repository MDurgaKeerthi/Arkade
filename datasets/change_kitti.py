
import numpy as np
import csv
import os
import sys


with open('glove_normalized_3d.txt') as f:
    of = open('norm_glove3d.txt', 'w')
    reader = csv.reader(f, delimiter=' ')
    for line in reader: 
        temp = [float(word) for word in line[:-1]]
        of.write("%f\t%f\t%f\n" % (temp[0], temp[1], temp[2]))
    
