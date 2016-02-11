#!/usr/bin/env python
from __future__ import print_function
import roslib
import sys
import rospy
import cv2
import os
from std_msgs.msg import String
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
from stereo_image_publisher.msg import stereo_camera
from camera_calibration_parsers import readCalibration
from subprocess import call
import subprocess
tst=subprocess.Popen(['v4l2-ctl', '--list-devices'],stdout=subprocess.PIPE)
comInfo = tst.stdout.read()
info = comInfo.split('USB Camera')[1:]
camString = ['0-5', '0-6']
def cam_sorder(camString,info):
    found={}
    for lst in info:
        for idx in camString:
            if lst.find(idx) is not -1:
                found[idx]=lst[lst.find('video')+5]
    return found
cams=cam_sorder(camString,info)
left_camera=cams['0-5']
right_camera=cams['0-6']

class image_converter(object):
  def __init__(self):
    self.pub = rospy.Publisher("aras_stereo_camera",stereo_camera)
    self.pub_ImageLeft = rospy.Publisher("aras_stereo_camera/left_image_raw",Image)
    self.pub_ImageRight = rospy.Publisher("aras_stereo_camera/right_image_raw",Image)

    self.message=stereo_camera()
    self.script_dir = os.path.dirname(os.path.realpath(__file__))
    self.bridge = CvBridge()
    self.leftCap = cv2.VideoCapture(int(left_camera))
    self.rightCap = cv2.VideoCapture(int(right_camera))
    self.leftCap.set(cv2.CAP_PROP_FPS,50)
    self.rightCap.set(cv2.CAP_PROP_FPS,50)
    os.system(os.path.join(self.script_dir,'camConfig.sh')) #Set the camera Gain and Exposure Parameters
    left_camera_name, self.left_camera_info = readCalibration(os.path.join(self.script_dir,'left.yaml'))
    right_camera_name, self.right_camera_info = readCalibration(os.path.join(self.script_dir,'right.yaml'))
  def start(self):
    while not rospy.is_shutdown():
        ret, leftFrame = self.leftCap.read()
        ret, rightFrame = self.rightCap.read()
        leftFrame = cv2.cvtColor(leftFrame, cv2.COLOR_BGR2GRAY)
        rightFrame = cv2.cvtColor(rightFrame, cv2.COLOR_BGR2GRAY)
        try:
          self.message.leftCamera=self.bridge.cv2_to_imgmsg(leftFrame, "mono8")
          self.message.rightCamera=self.bridge.cv2_to_imgmsg(rightFrame, "mono8")
          self.message.leftCameraInfo=self.left_camera_info
          self.message.rightCameraInfo=self.right_camera_info
          self.pub.publish(self.message)
          self.pub_ImageLeft.publish(self.message.leftCamera)
          self.pub_ImageRight.publish(self.message.rightCamera)
        except CvBridgeError as e:
          print(e)


ic = image_converter()
rospy.init_node('stereo_image_publisher', anonymous=True)
print("Node Initialized")
try:
    ic.start()
except KeyboardInterrupt:
    print("Shutting down")
    cv2.destroyAllWindows()

