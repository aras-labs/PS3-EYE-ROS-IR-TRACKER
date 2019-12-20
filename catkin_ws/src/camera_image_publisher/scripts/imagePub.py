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
from camera_image_publisher.msg import mono_camera
from camera_calibration_parsers import readCalibration
from subprocess import call
camera_id=0

class image_converter(object):
  def __init__(self):
    self.pub = rospy.Publisher("aras_camera/cameraInfo",CameraInfo)
    self.pub_Image = rospy.Publisher("aras_camera/image_raw",Image)
    self.message=mono_camera()
    self.script_dir = os.path.dirname(os.path.realpath(__file__))
    self.bridge = CvBridge()
    self.Cap = cv2.VideoCapture(int(camera_id))
    self.Cap.set(cv2.CAP_PROP_FPS,120)
    #os.system(os.path.join(self.script_dir,'camConfig.sh')) #Set the camera Gain and Exposure Parameters
    camera_name, self.camera_info = readCalibration(os.path.join(self.script_dir,'cam_params.yaml'))
  def start(self):
    while not rospy.is_shutdown():
        ret, Frame = self.Cap.read()
        Frame = cv2.cvtColor(Frame, cv2.COLOR_BGR2GRAY)
        try:
          self.message.Camera=self.bridge.cv2_to_imgmsg(Frame, "mono8")
          #self.message.CameraInfo=self.camera_info
          self.pub.publish(self.camera_info)
          self.pub_Image.publish(self.message.Camera)
        except CvBridgeError as e:
          print(e)

ic = image_converter()
rospy.init_node('camera_image_publisher', anonymous=True)
print("Node Initialized")
try:
    ic.start()
except KeyboardInterrupt:
    print("Shutting down")
    cv2.destroyAllWindows()

