#!/usr/bin/python

import nest

mcip = nest.Create('music_cont_in_proxy')
nest.SetStatus(mcip, {'port_name' : 'contdata'})

# Simulate and get vector data with a granularity of 10 ms:
time = 0
while time < 1000:
    nest.Simulate (10)
    data = nest.GetStatus (mcip, 'data')
    print data
    time += 10
