import pylab
from nest import raster_plot

raster_plot.from_file("spike_detector-503-0.gdf",hist=True)

pylab.show()
