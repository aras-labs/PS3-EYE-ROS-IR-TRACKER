#!/usr/bin/env python
from __future__ import print_function
import roslib
import sys
import rospy
import cv2
import os
from std_msgs.msg import String
from sensor_msgs.msg import Image
from sensor_msgs.msg import CameraInfo
from cv_bridge import CvBridge, CvBridgeError
from camera_calibration_parsers import readCalibration
from subprocess import call
import subprocess
from camera_image_publisher.srv import set_gain, set_exposure
import threading
import time
from dynamic_reconfigure.server import Server
from camera_image_publisher.cfg import camera_cfgsConfig

def cfgs_callback(config, level):
    subprocess.Popen(['v4l2-ctl', '-d', '/dev/video'+str(camera_id),\
                      '-c', 'gain='+str(config.gain)],stdout=subprocess.PIPE)
    subprocess.Popen(['v4l2-ctl', '-d', '/dev/video'+str(camera_id),\
                      '-c', 'exposure='+str(config.exposure)],stdout=subprocess.PIPE)
    return config

def handle_set_gain(req):
    subprocess.Popen(['v4l2-ctl', '-d', '/dev/video'+str(camera_id),\
                      '-c', 'gain='+str(req.gain)],stdout=subprocess.PIPE)
    print('The gain for the camera'+str(camera_id)+' has been set to:'+str(req.gain))
    return True
def handle_set_exposure(req):
    subprocess.Popen(['v4l2-ctl', '-d', '/dev/video'+str(camera_id),\
                      '-c', 'exposure='+str(req.exposure)],stdout=subprocess.PIPE)
    print('The exposure for the camera'+str(camera_id)+' has been set to:'+str(req.exposure))
    return True
def cam_sorder(camString,info):
    found={}
    for lst in info:
        for idx in camString:
            if lst.find(idx) is not -1:
                found[idx]=lst[lst.find('video')+5]
    return found
class image_grabber(object):
  def __init__(self):
    self.pub = rospy.Publisher("camera/cameraInfo",CameraInfo,queue_size=1)
    self.pub_Image = rospy.Publisher("camera/image_raw",Image,queue_size=1)
    self.image=Image()
    self.bridge = CvBridge()
    self.Cap = cv2.VideoCapture(int(camera_id))
    self.Cap.set(cv2.CAP_PROP_FPS,fps)
    self.Cap.set(cv2.CAP_PROP_BUFFERSIZE,1)
    self.kill=False
    #self.Cap.set(cv2.CV_CAP_PROP_BUFFERSIZE, 1);
    #os.system(os.path.join(self.script_dir,'camConfig.sh')) #Set the camera Gain and Exposure Parameters
    if path_to_calibration_file is not None:
        camera_name, self.camera_info = readCalibration(os.path.join(self.script_dir,\
                                                                path_to_calibration_file))
    else:
        self.camera_info=CameraInfo() 
  def start(self):
    while self.kill is not True:
        ret, Frame = self.Cap.read()
        self.camera_info.header.stamp=rospy.Time.now()
        Frame = cv2.cvtColor(Frame[:,:,2], cv2.COLOR_BAYER_BG2BGRA)
        Frame = cv2.cvtColor(Frame, cv2.COLOR_BGR2GRAY)
        try:
          self.image=self.bridge.cv2_to_imgmsg(Frame, "mono8")
          self.image.header.stamp=self.camera_info.header.stamp
          self.pub.publish(self.camera_info)
          self.pub_Image.publish(self.image)
        except CvBridgeError as e:
          print(e)

rospy.init_node('camera_image_publisher')
print("Node Initialized")
gain_service=rospy.Service('set_gain',set_gain,handle_set_gain)
gain_service=rospy.Service('set_exposure',set_exposure,handle_set_exposure)
#Get the info for the video devices connected to the system
tst=subprocess.Popen(['v4l2-ctl', '--list-devices'],stdout=subprocess.PIPE)
comInfo = tst.stdout.read()
info = comInfo.split('USB Camera')[1:]

#get the parameters of the node
if rospy.has_param('~usb_port'):
    port=rospy.get_param('~usb_port')
else:
    print('No port id parameter was set. Using the defult port id for the camera.')
    port=['0-2']
if rospy.has_param('~fps'):
    fps=rospy.get_param('~fps')
else:
    print('No fps parameter was set.Using 30fps as defult.')
    fps=30
if rospy.has_param('~path_to_calib_file'):
    path_to_calibration_file=rospy.get_param('~path_to_calib_file')
else:
    path_to_calibration_file=None
#Find out what /dev/videox corresponds to the given port name in the parameters
cams=cam_sorder(port,info)
camera_id=int(cams[port[0]])
#Set the camera mode to manual
subprocess.Popen(['v4l2-ctl', '-d', '/dev/video'+str(camera_id),\
                      '-c', 'auto_exposure=1'],stdout=subprocess.PIPE)
subprocess.Popen(['v4l2-ctl', '-d', '/dev/video'+str(camera_id),\
                      '-c', 'gain_automatic=0'],stdout=subprocess.PIPE)
#Assign a thread for the image acquisition function
ig = image_grabber()
image_aqu=threading.Thread(target=ig.start)
server=Server(camera_cfgsConfig,cfgs_callback)
try:
    image_aqu.start()
    rospy.spin()
    #if the ros goes down, the image capruring thread will have to terminate
    ig.kill=True
    image_aqu.join()
except KeyboardInterrupt:
    print("Shutting down")

