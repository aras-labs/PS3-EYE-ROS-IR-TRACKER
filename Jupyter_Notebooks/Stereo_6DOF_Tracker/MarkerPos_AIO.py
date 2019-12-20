#!/usr/bin/env python
from __future__ import print_function
import roslib
import sys
import rospy
import cv2
from std_msgs.msg import String
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
import numpy as np
from pyclustering.cluster.bsas import bsas, bsas_visualizer

def start():
    pipe.grab()
    pipe.seqInit()
    while not rospy.is_shutdown():
        try:
          pipe.grab()
          pipe.run()
          pipe.release()
        except CvBridgeError as e:
          print(e)
    #pipe.release()
    #cv2.imwrite('image.jpg', frame)

class IREF(object):
    def __init__(self, out, pub_node, br):
        self.cap = cv2.VideoCapture(0)
        self.frame=None
        self.videoStream = out
        self.markers = None
        self.camCounter = 0
        self.log = []
        self.publisher = pub_node
        self.bridge = br
        self.max_clusters=8
        self.threshold=20
        self.cls = []
        self.bnd = []
        self.params = cv2.SimpleBlobDetector_Params()
        self.params.minThreshold = 0;
        self.params.maxThreshold = 150;
        self.params.filterByArea = True
        self.params.minArea = 0
        self.params.filterByCircularity = True
        self.params.minCircularity = 0.1
        self.params.filterByConvexity = True
        self.params.minConvexity = 0.5
        self.params.filterByInertia = True
        self.params.minInertiaRatio = 0.1
        self.params.blobColor = 255
        ver = (cv2.__version__).split('.')
        if int(ver[0]) < 3 :
            self.detector = cv2.SimpleBlobDetector(self.params)
        else : 
            self.detector = cv2.SimpleBlobDetector_create(self.params)
    def seqInit(self):
        cv2.imwrite('image.jpg', cv2.cvtColor(self.frame, cv2.COLOR_GRAY2BGR))
        #self.gray = cv2.cvtColor(self.frame, cv2.COLOR_BGR2GRAY)
        
        print("Initialization Sequence Finished, Running Geometrical Analysis now...")
        
    def run(self):
        #self.gray = cv2.cvtColor(self.frame, cv2.COLOR_BGR2GRAY)
        ret,img = cv2.threshold(cv2.cvtColor(self.frame, cv2.COLOR_GRAY2BGR),230,255,cv2.THRESH_BINARY)
        sample = cv2.findNonZero(cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)).reshape(-1, 2)
        bsas_instance = bsas(sample, self.max_clusters, self.threshold)
        bsas_instance.process()
        clusters = bsas_instance.get_clusters()
        representatives = bsas_instance.get_representatives()
        self.cls=[]
        self.bnd=[]
        for cluster in clusters:
            sum=np.array([0, 0], dtype=np.float32)
            pts=[]
            for i in cluster:
                pts.append(list(sample[i]))
                sum+=sample[i]
            cls_pts = np.vstack((pts[np.where(np.array(pts)[:, 0] == np.max(np.array(pts)[:, 0]))[0][0]],
                                 pts[np.where(np.array(pts)[:, 0] == np.min(np.array(pts)[:, 0]))[0][0]],
                                 pts[np.where(np.array(pts)[:, 1] == np.max(np.array(pts)[:, 1]))[0][0]],
                                 pts[np.where(np.array(pts)[:, 1] == np.min(np.array(pts)[:, 1]))[0][0]]))
            self.bnd.append([np.min(cls_pts[:, 0]), np.min(cls_pts[:, 1]),
                        np.max(cls_pts[:, 0]) - np.min(cls_pts[:, 0]),
                        np.max(cls_pts[:, 1]) - np.min(cls_pts[:, 1])])
        for i in range(len(self.bnd)):
            img = self.frame[(self.bnd[i][1]-6):self.bnd[i][1]+self.bnd[i][3]+6, self.bnd[i][0]-6:(self.bnd[i][0]+6+self.bnd[i][2])]
            img = cv2.medianBlur(img,5)
            cv2.imwrite('deb.jpg', img)
            keypoints = self.detector.detect(img)
            for key in keypoints:
                print(list(np.array(key.pt)+np.array([self.bnd[i][0], self.bnd[i][1]])))
                self.cls.append(list(np.array(key.pt)+np.array([self.bnd[i][0], self.bnd[i][1]])))
        self.cls = np.array(self.cls).reshape(-1, 2)
        print("%%%%%%%%%%%%%%")
        #self.log.append(self.LED.reshape(1, -1))
        if self.camCounter==1000:
            for _ in range(10):
                print("RELEASED")
            self.videoStream.release()
            #np.savetxt('log.csv', self.log)
        #else:
            #self.videoStream.write(cv2.cvtColor(self.frame, cv2.COLOR_GRAY2BGR))
        self.camCounter = self.camCounter + 1
        
    def release(self):
        obs_frame = np.zeros((self.frame.shape[0], self.frame.shape[1]), dtype=np.uint8)
        for i in range(len(self.cls)):
            cv2.circle(obs_frame,(int(self.cls[i][0]), int(self.cls[i][1])), 2, (255,0,255), -1)
        for bound in self.bnd:
            cv2.rectangle(obs_frame,(bound[0],bound[1]),(bound[0]+bound[2],bound[1]+bound[3]),(255,255,2555),3)
        if self.camCounter<1000:
            self.videoStream.write(cv2.cvtColor(obs_frame, cv2.COLOR_GRAY2BGR))
        self.publisher.publish(self.bridge.cv2_to_imgmsg(obs_frame, "mono8"))
        
    def grab(self):
        ret, self.frame = self.cap.read()
        self.frame = cv2.cvtColor(self.frame[:, :, 0], cv2.COLOR_BAYER_BG2GRAY)
if __name__ == '__main__':
    bridge = CvBridge()
    rospy.init_node('MarkerPos_AIO', anonymous=True)
    publisher = rospy.Publisher("obs",Image)
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter('debug.avi', fourcc, 20.0, (640,480))
    criteria = (cv2.TERM_CRITERIA_EPS, 60, 0.001)
    pipe = IREF(out, publisher, bridge)
    print("Initialization Sequence Started")
    start()
