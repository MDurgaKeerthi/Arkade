import osmium as osm
import math
import numpy as np

class OSMHandler(osm.SimpleHandler):
    def __init__(self):
        osm.SimpleHandler.__init__(self)
        self.osm_data = []
        self.file1 = open('cali_loc.txt', 'w')

    def node(self, n):
        lat =  np.deg2rad(n.location.lat)
        lon =  np.deg2rad(n.location.lon)
        x = math.cos(lat) * math.cos(lon)
        y = math.cos(lat) * math.sin(lon)
        z = math.sin(lat)
        self.file1.write(str(x)+'\t'+str(y)+'\t'+str(z)+'\n')
    
    def __del__(self):
        self.file1.close()


osmhandler = OSMHandler()
# scan the input file and fills the handler list accordingly
osmhandler.apply_file("cali.osm")
