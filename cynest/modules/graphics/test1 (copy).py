# 
# First python script to interactively simulate nest with opengl in-and-output.
# 
# @authors: csaba.ero@epfl.ch, ?jonny.quarta@epfl.ch?
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 


import os
import sys

# in case nest is not on the default python path
#~ sys.path.append('../LIB/nest/lib64/python2.6/site-packages')
#sys.path.append('../LIB/nest-r9837/lib64/python2.6/site-packages')
import cynest as nest

from PyQt4 import QtCore
from PyQt4 import QtGui
from PyQt4 import QtOpenGL
from OpenGL import GLU
from OpenGL.GL import *

import numpy as np
from pylab import *
from time import time, sleep


# window size
W = 1200.0
H = 750.0
# drawable surface size
Wreal = W
Hreal = H

# view angles
theta = 0.0
phi   = 0.0

# fabulous texture for the brilliant neurons
auratex = -1

# neuron 3D positions
pos   = []
# neuron 2D positions (projection on the screen)
pos2d = []
# alpha value of neurons (is increased by each spike, decreases after each spike)
val   = []






# returns a 2D projection position from a 3D position 
def get_2D_pos_from_3D(pos_):
    modelview = glGetDoublev(GL_MODELVIEW_MATRIX);
    projection = glGetDoublev(GL_PROJECTION_MATRIX );
    viewport = glGetIntegerv(GL_VIEWPORT );
    P2D = GLU.gluProject(pos_[0],pos_[1],pos_[2],modelview,projection,viewport);
    return np.array([-1.0 + 2.0*P2D[0]/Wreal, -1.0 + 2.0*P2D[1]/Hreal, P2D[2]])



# returns a 3D position from a 2D projection position, will be used to create neurons from a mouse click
#~ V3 get_3D_pos_from_2D(V3 blendtarget, V3 camera_rl, V3 camera_ud, V3 camera_pos, V2 pos3D, int W, int H, float FOV){
#~ 
#~ return blendtarget + camera_rl*(float(W)/float(H))*(tan((FOV/2.0)*M_PI/180.0))*(blendtarget-camera_pos).norm()*2.0*((float(pos3D.x))) - camera_ud*(tan((FOV/2.0)*M_PI/180.0))*(blendtarget-camera_pos).norm()*2.0*((float(pos3D.y)));
#~ 
#~ }

def normalize(vect):
	norm = math.sqrt(vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2])
	return [vect[0]/norm, vect[1]/norm, vect[2]/norm]


# prepare NEST
nest.ResetKernel()
nest.sr("M_WARNING setverbosity")
nest.SetKernelStatus({"overwrite_files":True})
nest.SetKernelStatus({"local_num_threads": 2})


# create N neurons, only the 3D positions have to be specifically specified
for i in range(20):
    nest.Create('aeif_cond_exp', 1)
    pos.append(2.0*(np.random.rand(3)*2.0-1.0))
    pos2d.append(np.array([0.0, 0.0]))
    val.append(0.0)
 

# give'em random external currents
nest.SetStatus(range(1,len(pos)+1), "I_e", list(750.0 + np.random.rand(len(pos))*600.0))

nest.RandomDivergentConnect([1], list(range(2,20)), 4, 1.0, 0.01, "static_synapse")

print nest.GetConnections([1])

# the opengl class
class GLWidget(QtOpenGL.QGLWidget):
    def __init__(self, parent=None):
        self.parent = parent
        QtOpenGL.QGLWidget.__init__(self, parent)

    def initializeGL(self):
        self.qglClearColor(QtGui.QColor(0.0, 0.0, 0.0))
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_1D);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
        glShadeModel(GL_SMOOTH);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    def resizeGL(self, width, height):
        global Wreal, Hreal
        Wreal = width
        Hreal = height
        glViewport(0, 0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        aspect = width / float(height)

        GLU.gluPerspective(45.0, aspect, 1.0, 100.0)
        glMatrixMode(GL_MODELVIEW)
    
    
    # draw !!!
    def paintGL(self):
        
        nest.Simulate(0.1)
        Vs = nest.GetStatus(range(1,len(pos)+1), "V_m")
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        # camera coordinates
        GLU.gluLookAt(5.0 * cos(theta), 5.0 * sin(theta), 0.0,  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);
        glPushMatrix()
        
        # draw points (these should be drawn nicer using shaders)
        glBindTexture(GL_TEXTURE_2D, 0)
        glPointSize(10.0)
        glBegin(GL_POINTS)
        for ip,p in enumerate(pos):
            glColor4f(1.0,1.0,1.0, 0.2 + val[ip]*0.3)
            glVertex3f(p[0],p[1],p[2])
        glEnd()
        glPopMatrix()
        
        
        #draw connections
        glPushMatrix()
        
        glBegin(GL_TRIANGLES)
        for ip,p in enumerate(pos):
            glColor4f(0.0,1.0,0.0, 0.2 + val[ip]*0.3)
            src = p
            for d in nest.GetConnections([ip]):
                dest = pos[d[1]]
                direction  = normalize((dest - src))
                normal1 = [0.0, 0.0, 1.0]
                normal2 = [0.0, 1.0, 0.0]
                normal1[0] = direction[2]/direction[0]
                normal2[0] = direction[1]/direction[0]
                normal1 = normalize(normal1)
                normal2 = normalize(normal2)    
                coef = 0.1
                glVertex3f(src[0] + coef*normal1[0], src[1] + coef*normal1[1], src[2] + coef*normal1[2])
                glVertex3f(src[0] + coef*normal2[0], src[1] + coef*normal2[1], src[2] + coef*normal2[2])
                glVertex3f(dest[0],dest[1],dest[2])
        glEnd()
        glPopMatrix()
        
        
        # draw brilliance auras
        glPushMatrix()
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix()
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        Bsize = 0.32
        glBindTexture(GL_TEXTURE_2D, texx)
        glBegin(GL_QUADS)
        for ip,p2d in enumerate(pos2d):
            global Wreal, Hreal
            HW = float(Hreal)/float(Wreal)
            scneu = (Vs[ip] + 65.0) / 40.0
            glColor4f(1.0,1.0,1.0, val[ip]*0.3)
            glTexCoord2d(0.0,0.0)
            glVertex2f(p2d[0]-Bsize*HW, p2d[1]-Bsize)
            glTexCoord2d(1.0,0.0);
            glVertex2f(p2d[0]+Bsize*HW, p2d[1]-Bsize)
            glTexCoord2d(1.0,1.0);
            glVertex2f(p2d[0]+Bsize*HW, p2d[1]+Bsize)
            glTexCoord2d(0.0,1.0);
            glVertex2f(p2d[0]-Bsize*HW, p2d[1]+Bsize)
            val[ip] += (1.0/100.0) * (-val[ip])
            if Vs[ip]>-45.0:val[ip] = 1.0
        glEnd()
        glMatrixMode(GL_PROJECTION);
        glPopMatrix()
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_DEPTH_TEST);
        glPopMatrix()
        
        # get 2D projection positions from 3D positions
        for ip,p in enumerate(pos):
            pos2d[ip] = get_2D_pos_from_3D(p)
        
    
    def draw(self):
        #~ self.parent.statusBar().showMessage('StatusBar example: rotation %f' % self.yRotDeg)
        self.updateGL()




# main window class (implement widgets and keyboard shortcuts here)
class MainWindow(QtGui.QMainWindow):

    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        
        global W, H
        self.resize(W, H)
        self.setWindowTitle('Neurob')

        self.initActions()
        self.initMenus()

        glWidget = GLWidget(self)
        self.setCentralWidget(glWidget)
        
        QtGui.QShortcut(QtGui.QKeySequence("Left"),  self, self.goleft)
        QtGui.QShortcut(QtGui.QKeySequence("Right"), self, self.goright)
        QtGui.QShortcut(QtGui.QKeySequence("Up"),    self, self.goup)
        QtGui.QShortcut(QtGui.QKeySequence("Down"),  self, self.godown)
        
        timer = QtCore.QTimer(self)
        timer.setInterval(4)
        QtCore.QObject.connect(timer, QtCore.SIGNAL('timeout()'), glWidget.draw)
        timer.start()
    
    def goleft(self): global theta;theta += 0.1
    def goright(self):global theta;theta -= 0.1
    def goup(self):   global phi;  phi   += 0.1
    def godown(self): global phi;  phi   -= 0.1

    def initActions(self):
        self.exitAction = QtGui.QAction('Quit', self)
        self.exitAction.setShortcut('Esc')
        self.exitAction.setStatusTip('Exit application')
        self.connect(self.exitAction, QtCore.SIGNAL('triggered()'), self.close)

    def initMenus(self):
        menuBar = self.menuBar()
        fileMenu = menuBar.addMenu('&File')
        fileMenu.addAction(self.exitAction)

    def close(self):
        QtGui.qApp.quit()




app = QtGui.QApplication(sys.argv)

win = MainWindow()
win.show()


# texture loading 
def TexFromPNG(filename):
    import Image
    img = Image.open(filename)
    img_data = np.array(list(img.getdata()), np.uint16)
    texture = glGenTextures(1)
    glPixelStorei(GL_UNPACK_ALIGNMENT,1)
    glBindTexture(GL_TEXTURE_2D, texture)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.size[0], img.size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data)
    return texture



texx = TexFromPNG("aura_round.png")


sys.exit(app.exec_())

