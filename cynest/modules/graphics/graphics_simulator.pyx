# 
# First python script to interactively simulate nest_engine.cynest with opengl in-and-output.
# 
# @authors: csaba.ero@epfl.ch, jonny.quarta@epfl.ch
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 


import os
import sys
import socket
import subprocess

# Note that every method begins with graphics_simulator_. This
# is done because of possible name redundancy
# in other parts of the kernel (everything is global)

# sockets
graphics_simulator_sender = graphics_simulator_listener = None
# list of added spike detectors (for event recording)
graphics_simulator_spike_detector = []
# available ids (without added spike detectors)
graphics_simulator_ids = []


def graphics_simulator_socket_listen(port, host = 'localhost'):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host, port))
    s.listen(1)
    conn, addr = s.accept()

    return conn


def graphics_simulator_socket_speak(port, host = 'localhost'):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    return s






def graphics_simulator_receiveKeyword(key):
    while graphics_simulator_listener.recv(len(key)) != key:
        pass


# Creates a new spike detector (if not already created) and 
# attaches it to the network, then connects everything to it
def graphics_simulator_initOperations():
    global graphics_simulator_spike_detector, graphics_simulator_ids
    
    size = int(nest_engine.cynest.GetKernelStatus()['network_size'])
    
    if(len(graphics_simulator_spike_detector) == 0):
        graphics_simulator_spike_detector = nest_engine.cynest.Create("spike_detector")
    
    
    graphics_simulator_ids = list(set(list(range(1,size))) - set(graphics_simulator_spike_detector))
    
    not_connected_ids = [x for x in graphics_simulator_ids if not graphics_simulator_spike_detector in [y[1] for y in nest_engine.cynest.GetConnections([x])] ]
    
    if len(not_connected_ids) > 0:
        nest_engine.cynest.ConvergentConnect(not_connected_ids, graphics_simulator_spike_detector)
    



def graphics_simulator_sendPositions():
    for i in graphics_simulator_ids:
        status = nest_engine.cynest.GetStatus([i])[0]

        if 'pos-x' in status and 'pos-y' in status and 'pos-z' in status:
            msg = "[" + str(i) + "," + str(status["pos-x"]) + "," + str(status["pos-y"]) + "," + str(status["pos-z"]) + "]" # [id, x,y,z]
        else:
            msg = "[" + str(i) + ",#]"
            
        graphics_simulator_sender.send(msg)
        graphics_simulator_receiveKeyword("ok")
        
    graphics_simulator_sender.send("end")
    graphics_simulator_receiveKeyword("ok")
    print "positions done"
    
    
def graphics_simulator_sendConnections():    
    for i in graphics_simulator_ids:
        listMsg = list(  set([x[1] for x in nest_engine.cynest.GetConnections([i])]) - set(graphics_simulator_spike_detector)  ) # [id1,id2,id3...], no multiple values
        msg = str(listMsg)

        graphics_simulator_sender.send("[" + str(i) + "," + str(len(listMsg)) + "," + str(len(msg)) + "]") # [id, nb connections, length next msg]
        graphics_simulator_receiveKeyword("param_ok")
        graphics_simulator_sender.send(msg)
        graphics_simulator_receiveKeyword("msg_ok")
        
    graphics_simulator_sender.send("end")
    graphics_simulator_receiveKeyword("ok")








def graphics_simulator_init():
    global graphics_simulator_sender, graphics_simulator_listener
    
    subprocess.Popen([nest_engine.exec_dir + '/g_simulator'])
    
    graphics_simulator_listener = graphics_simulator_socket_listen(50001)
    graphics_simulator_sender = graphics_simulator_socket_speak(50002)
    
    graphics_simulator_initOperations()
    
    graphics_simulator_receiveKeyword("ready")
    print ("ready received!!!")
    graphics_simulator_sendPositions()
    graphics_simulator_sendConnections()
    
    
    
def graphics_simulator_simulate(time):
    t = 0.0
    while t < time:
        nest_engine.cynest.sps(float(0.1))
        nest_engine.cynest.sr('ms Simulate')
        
        ev = nest_engine.cynest.GetStatus(graphics_simulator_spike_detector, "events") # {[times], [senders]}
        graphics_simulator_sender.send(str([t] + ev["senders"])) # [t, id1, id2, ...]
        nest_engine.cynest.SetStatus(graphics_simulator_spike_detector, {"events":[]})
        
        t += 0.1


def graphics_simulator_close():
    graphics_simulator_sender.send("close")
    graphics_simulator_receiveKeyword("ok")
    graphics_simulator_sender.close()
    graphics_simulator_listener.close()
    






def graphics_simulate(t):
    graphics_simulator_init()
    #graphics_simulator_simulate(t)
    #graphics_simulator_close()


    
    
    
