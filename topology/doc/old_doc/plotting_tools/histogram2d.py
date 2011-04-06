# Generate data for a 2D histogram
import numpy as np

## Histogram function
##
## Input:
## coordinates - 2D data points
## min/max - lower left and upper right corner - [x, y]
## bins - number of histogram bins - [x_number, y_number]
##
## Output:
## x and y: 2D matrices showing the center positions 
## of the histogram bins
## data: 2D matrix giving the number of data points within
## each histogram bin
def histogram2d(coordinates, min, max, bins):
    # Create histogram data matrix.
    data = np.zeros((bins[0],bins[1]), int)

    # Find width and height of histogram bins.
    resolution = [float(max[0]-min[0])/bins[0], float(max[1]-min[1])/bins[1]]

    # Update histogram data matrix.
    for i in coordinates:
        x_bin = int(float(i[0]-min[0])/resolution[0])
        y_bin = int(float(i[1]-min[1])/resolution[1])
        if x_bin == bins[0] or y_bin == bins[1]:
            print "Data points are not allowed on max limit."
        else:
            data[y_bin][x_bin] = data[y_bin][x_bin] + 1
            
    ## Create x/y grid
            
    # Set step size in x/y grid
    delta = [(max[0]-min[0])/bins[0], (max[1]-min[1])/bins[1]]

    min = [min[0]+delta[0]/2, min[1]+delta[1]/2]

    # Create x/y grid from min to max with steps of delta
    x, y = np.mgrid[min[0]:max[0]:delta[0], min[1]:max[1]:delta[1]]

    return [x, y, data]
