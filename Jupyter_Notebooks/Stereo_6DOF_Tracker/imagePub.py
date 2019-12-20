#!/usr/bin/env python
from __future__ import print_function
import roslib
import sys
import rospy
import cv2
from std_msgs.msg import String
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError

class image_converter(object):
  def __init__(self):
    self.image_pub = rospy.Publisher("image_topic",Image)
    self.bridge = CvBridge()
    self.cap = cv2.VideoCapture(2)
    #self.cap = cv2.VideoCapture('debug.avi')
  def start(self):
    while not rospy.is_shutdown():
        ret, frame = self.cap.read()
        frame = cv2.cvtColor(frame[:, :, 0], cv2.COLOR_BAYER_BG2GRAY)
        ret,frame = cv2.threshold(cv2.cvtColor(frame, cv2.COLOR_GRAY2BGR),230,255,cv2.THRESH_BINARY)
        frame = cv2.cvtColor(frame[:, :, 0], cv2.COLOR_BAYER_BG2GRAY)
        try:
          self.image_pub.publish(self.bridge.cv2_to_imgmsg(frame, "mono8"))
        except CvBridgeError as e:
          print(e)


ic = image_converter()
rospy.init_node('image_converter', anonymous=True)
print("Node Initialized")
try:
    ic.start()
except KeyboardInterrupt:
    print("Shutting down")
    cv2.destroyAllWindows()

