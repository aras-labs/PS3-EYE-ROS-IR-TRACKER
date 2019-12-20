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

def callback(data):
    global bridge
    global out
    global pipe
    global init
    frame = bridge.imgmsg_to_cv2(data, desired_encoding="passthrough")
    pipe.frame = frame
    pipe.run()
    #pipe.release()
    #cv2.imwrite('image.jpg', frame)
    
def listener():
    rospy.Subscriber("image_topic", Image, callback)
    rospy.spin()

class IREF(object):
    def __init__(self, out, pub_node, br):
        self.frame=None
        self.videoStream = out
        self.markers = None
        self.camCounter = 0
        self.log = []
        self.publisher = pub_node
        self.bridge = br
        self.max_clusters=5
        self.threshold=20
        self.cls = []
        self.bnd = []
    def seqInit(self, _frame):
        self.frame = _frame
        cv2.imwrite('image.jpg', cv2.cvtColor(self.frame, cv2.COLOR_GRAY2BGR))
        #self.gray = cv2.cvtColor(self.frame, cv2.COLOR_BGR2GRAY)
        
        print("Initialization Sequence Finished, Running Geometrical Analysis now...")
        
    def run(self):
        #self.gray = cv2.cvtColor(self.frame, cv2.COLOR_BGR2GRAY)
        ret,img = cv2.threshold(cv2.cvtColor(self.frame, cv2.COLOR_GRAY2BGR),180,255,cv2.THRESH_BINARY)
        sample = cv2.findNonZero(cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)).reshape(-1, 2)
        print(len(sample))
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
            sum = sum/len(cluster)
            self.cls.append(sum)
        self.cls = np.array(self.cls).reshape(-1, 2)
        print(len(self.cls))
        #self.log.append(self.LED.reshape(1, -1))
        if self.camCounter>1000:
            self.videoStream.release()
            #np.savetxt('log.csv', self.log)
        #else:
            #self.videoStream.write(cv2.cvtColor(self.frame, cv2.COLOR_GRAY2BGR))
        self.camCounter = self.camCounter + 1
        
    def release(self):
        obs_frame = np.zeros((self.frame.shape[0], self.frame.shape[1]), dtype=np.uint8)
        for i in range(len(self.cls)):
            cv2.circle(obs_frame,(int(self.cls[i][0]), int(self.cls[i][1])), 2, (255,0,255), -1)
        #cv2.circle(obs_frame,(int(self.cls[0][0]), int(self.cls[0][1])), 2, (255,0,255), -1)
        #cv2.circle(obs_frame,(int(self.cls[1][0]), int(self.cls[1][1])), 2, (255,0,255), -1)
        #cv2.circle(obs_frame,(int(self.cls[2][0]), int(self.cls[2][1])), 2, (255,0,255), -1)
        #cv2.circle(obs_frame,(int(self.cls[3][0]), int(self.cls[3][1])), 2, (255,0,255), -1)
        #cv2.circle(obs_frame,(int(self.cls[4][0]), int(self.cls[4][1])), 2, (255,0,255), -1)
        if self.camCounter<1000:
            self.videoStream.write(cv2.cvtColor(obs_frame, cv2.COLOR_GRAY2BGR))
        self.publisher.publish(self.bridge.cv2_to_imgmsg(obs_frame, "mono8"))
        
if __name__ == '__main__':
    bridge = CvBridge()
    rospy.init_node('VIO_listener', anonymous=True)
    publisher = rospy.Publisher("obs",Image)
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter('debug.avi', fourcc, 20.0, (752,480))
    criteria = (cv2.TERM_CRITERIA_EPS, 30, 0.001)
    pipe = IREF(out, publisher, bridge)
    print("Initialization Sequence Started")
    pipe.seqInit(bridge.imgmsg_to_cv2(rospy.wait_for_message("/image_topic", Image), desired_encoding="passthrough"))
    print("Initialization Image Retreived")
    listener()
