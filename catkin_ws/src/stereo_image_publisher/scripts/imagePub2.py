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
import numpy as np
from pyclustering.cluster.bsas import bsas

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
left_camera=0 #cams['0-5']
right_camera=1 #cams['0-6']

class markerExteractor(object):
    def __init__(self):
        self.max_clusters = 8
        self.threshold = 20
        self.blubParams = cv2.SimpleBlobDetector_Params()
        self.blubParams.minThreshold = 50;
        self.blubParams.maxThreshold = 255;
        self.blubParams.filterByArea = True
        self.blubParams.minArea = 0
        self.blubParams.filterByCircularity = True
        self.blubParams.minCircularity = 0.3
        self.blubParams.filterByConvexity = True
        self.blubParams.minConvexity = 0.7
        self.blubParams.filterByInertia = True
        self.blubParams.minInertiaRatio = 0.1
        self.blubParams.blobColor = 255
        ver = (cv2.__version__).split('.')
        if int(ver[0]) < 3 :
            self.blubDetector = cv2.SimpleBlobDetector(self.blubParams)
        else : 
            self.blubDetector = cv2.SimpleBlobDetector_create(self.blubParams)
    def detect(self,frame):
        self.cms=[]
        self.image_ROIs=[]
        self.keypoints=[]
        img_gray=cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        ret,img_thresh = cv2.threshold(img_gray,100,255,cv2.THRESH_TOZERO)
        #Find the clusters
        self.nonzro_samples = cv2.findNonZero(img_thresh)
        if self.nonzro_samples is None:
            return None
        else:
            self.nonzro_samples=self.nonzro_samples.reshape(-1, 2).astype('float32')
        bsas_instance = bsas(self.nonzro_samples, self.max_clusters, self.threshold)
        bsas_instance.process()
        clusters = bsas_instance.get_clusters()
        #Calculate the center of the clusters and the Regions of Interests
        self.ROIs=np.zeros((len(clusters),4))
        for i,cluster in enumerate(clusters):
            current_batch=self.nonzro_samples[cluster]
            self.cms.append(np.sum(current_batch,axis=0)/current_batch.shape[0])
            row_max=np.max(current_batch[:,1],axis=0)+6
            row_min=np.min(current_batch[:,1],axis=0)-6
            col_max=np.max(current_batch[:,0],axis=0)+6
            col_min=np.min(current_batch[:,0],axis=0)-6
            self.ROIs[i,:]=[row_min,row_max,col_min,col_max]
        for roi in self.ROIs.astype('int32'):
            self.image_ROIs.append(img_thresh.copy()[roi[0]:roi[1],roi[2]:roi[3]])
        #Return The Results
        marker_points=[]
        for i,roi in enumerate(self.image_ROIs):
            keys_in_roi=self.blubDetector.detect(roi)
            for key in keys_in_roi:
                #Calculate the global coordinate of marker points. The points are returned in (X(Col),Y(Row)) coordinate. 
                marker_points.append([key.pt[0]+self.ROIs.astype('float32')[i,2],key.pt[1]+self.ROIs.astype('float32')[i,0]])
        return np.array(marker_points)
markerExteractor_inst=markerExteractor()

class image_converter(object):
  def __init__(self):
    self.pub = rospy.Publisher("aras_stereo_camera2",stereo_camera)
    self.pub_ImageLeft = rospy.Publisher("aras_stereo_camera/left_image_raw1",Image)
    self.pub_ImageRight = rospy.Publisher("aras_stereo_camera/right_image_raw1",Image)

    self.message=stereo_camera()
    self.script_dir = os.path.dirname(os.path.realpath(__file__))
    self.bridge = CvBridge()
    self.leftCap = cv2.VideoCapture(int(left_camera))
    self.rightCap = cv2.VideoCapture(int(right_camera))
    self.leftCap.set(cv2.CAP_PROP_FPS,50)
    self.rightCap.set(cv2.CAP_PROP_FPS,50)
#    os.system(os.path.join(self.script_dir,'camConfig.sh')) #Set the camera Gain and Exposure Parameters
    left_camera_name, self.left_camera_info = readCalibration(os.path.join(self.script_dir,'left.yaml'))
    right_camera_name, self.right_camera_info = readCalibration(os.path.join(self.script_dir,'right.yaml'))
  def start(self):
    while not rospy.is_shutdown():
        ret, leftFrame = self.leftCap.read()
        ret, rightFrame = self.rightCap.read()
	points_left=markerExteractor_inst.detect(leftFrame)
	points_right=markerExteractor_inst.detect(rightFrame)
        leftFrame = cv2.cvtColor(leftFrame, cv2.COLOR_BGR2GRAY)
        rightFrame = cv2.cvtColor(rightFrame, cv2.COLOR_BGR2GRAY)
	print(points_right,points_left)
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
rospy.init_node('stereo_image_publisher2', anonymous=True)
print("Node Initialized")
try:
    ic.start()
except KeyboardInterrupt:
    print("Shutting down")
    cv2.destroyAllWindows()

