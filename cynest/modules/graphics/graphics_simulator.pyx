# 
# First python script to interactively simulate nest_engine.cynest with opengl in-and-output.
# 
# @authors: csaba.ero@epfl.ch, ?jonny.quarta@epfl.ch?
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 


import os
import sys
import socket
import subprocess

graphics_simulator_sender = graphics_simulator_listener = None
graphics_simulator_spike_detectors = []
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


def graphics_simulator_initOperations():
    global graphics_simulator_spike_detectors, graphics_simulator_ids
    
    size = int(nest_engine.cynest.GetKernelStatus()['network_size'])
    graphics_simulator_spike_detectors += nest_engine.cynest.Create("spike_detector")
    
    graphics_simulator_ids = list(set(list(range(1,size))) - set(graphics_simulator_spike_detectors))
    
    nest_engine.cynest.ConvergentConnect(graphics_simulator_ids, [graphics_simulator_spike_detectors[-1]])
    



def graphics_simulator_sendPositions():
    for i in graphics_simulator_ids:
        status = nest_engine.cynest.GetStatus([i])
        if "pos-x" in status and "pos-y" in status and "pos-z" in status:
            msg = "[" + str(i) + "," + str(status["pos-x"]) + "," + str(status["pos-y"]) + "," + str(status["pos-z"]) + "]" # [id, x,y,z]
        else:
            msg = "[-]"
            
        graphics_simulator_sender.send(msg)
        graphics_simulator_receiveKeyword("ok")
        
    graphics_simulator_sender.send("end")
    graphics_simulator_receiveKeyword("ok")
    
    
def graphics_simulator_sendConnections():    
    for i in graphics_simulator_ids:
        msg = str(list(set([x[1] for x in nest_engine.cynest.GetConnections([i])]))) # [id1,id2,id3...], no multiple values
        graphics_simulator_sender.send("[" + str(i) + "," + str(len(msg)) + "]") # [id, length next msg]
        graphics_simulator_receiveKeyword("ok")
        graphics_simulator_sender.send(msg)
        graphics_simulator_receiveKeyword("ok")
        
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
        nest_engine.cynestsps(float(0.1))
        nest_engine.cynestsr('ms Simulate')
        
        ev = nest_engine.cynest.GetStatus([graphics_simulator_spike_detectors[-1]], "events") # {[times], [senders]}
        graphics_simulator_sender.send(str(t) + str(ev["senders"]))
        nest_engine.cynest.SetStatus([graphics_simulator_spike_detectors[-1]], {"events":[]})
        
        t += 0.1


def graphics_simulator_close():
    graphics_simulator_sender.send("close")
    graphics_simulator_receiveKeyword("ok")
    graphics_simulator_sender.close()
    graphics_simulator_listener.close()
    






def graphics_simulate(t):
    graphics_simulator_init()
    graphics_simulator_simulate(t)
    graphics_simulator_close()


    
    
    
