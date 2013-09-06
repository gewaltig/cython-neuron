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
import random

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
    s.setblocking(0);
    s.bind((host, port))
    s.listen(1)
    
    while True:
        try:
            conn, addr = s.accept()
            break
        except:
            pass

    return conn


def graphics_simulator_socket_speak(port, host = 'localhost'):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    return s






def graphics_simulator_receiveKeyword(key):
    while True:
        try:
            if graphics_simulator_listener.recv(len(key)) == key:
                break
        except:
            pass
        
def graphics_simulator_receiveKeywordNonBlocking(key):
    try:
        if graphics_simulator_listener.recv(len(key)) == key:
            return True
    except:
        pass
    
    return False


# Creates a new spike detector (if not already created) and 
# attaches it to the network, then connects everything to it
def graphics_simulator_initOperations():
    global graphics_simulator_spike_detector, graphics_simulator_ids
    
    nest_engine.cynest.sr("M_WARNING setverbosity")
    nest_engine.cynest.SetKernelStatus({"overwrite_files":True})
    
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
    
    port = random.randint(10000, 65500)
    
    subprocess.Popen([nest_engine.exec_dir + '/g_simulator', str(port)])
    
    graphics_simulator_listener = graphics_simulator_socket_listen(port)
    graphics_simulator_sender = graphics_simulator_socket_speak(port + 1)
    
    graphics_simulator_receiveKeyword("ready")
    
    graphics_simulator_initOperations()
    
    graphics_simulator_sendPositions()
    graphics_simulator_sendConnections()
    
    
    
def graphics_simulator_simulate(time):
    t = 0.0
    keepGoing = True
    graphics_simulator_receiveKeyword("simulate")
    
    while t < time:
        if graphics_simulator_receiveKeywordNonBlocking("quit"):
            return
        elif graphics_simulator_receiveKeywordNonBlocking("stop"):
            keepGoing = False
        elif graphics_simulator_receiveKeywordNonBlocking("resume"):
            keepGoing = True
        
        if keepGoing:
            nest_engine.cynest.sps(float(0.1)) # have to calculate time with respect to minimal delay
            nest_engine.cynest.sr('ms Simulate')
        
            ev = nest_engine.cynest.GetStatus(graphics_simulator_spike_detector, "events") # {[times], [senders]}
            msg = str(ev[0]["senders"])
            graphics_simulator_sender.send("[" + str([t]) + ","+ str(len(ev[0]["senders"])) + "," + len(msg) + "]" ) # [t, nb_spikes, length msg]
            graphics_simulator_receiveKeyword("ok")
            graphics_simulator_sender.send(msg) # [id1, id2, ...]
            nest_engine.cynest.SetStatus(graphics_simulator_spike_detector, [{"n_events": 0}])
        
            t += 0.1
            
    graphics_simulator_sender.send("finish")
    graphics_simulator_receiveKeyword("ok")
        

def graphics_simulator_close():
    graphics_simulator_sender.close()
    graphics_simulator_listener.close()
    






def graphics_simulate(t):
    graphics_simulator_init()
    graphics_simulator_simulate(t)
    graphics_simulator_close()


    
    
    
