#!/usr/bin/env python
# license removed for brevity
import rospy
from geometry_msgs.msg import Pose2D
import cv2
import numpy as np
import yaml
from pyclustering.cluster.bsas import bsas

class undistrodMarkers:
    def __init__(self,config_file_name):
        with open(config_file_name, 'r') as f:
            calib = yaml.safe_load(f.read())
        self.K = np.array(calib['camera_matrix']['data']).reshape(calib['camera_matrix']['rows'],calib['camera_matrix']['cols'])
        self.D = np.array(calib['distortion_coefficients']['data']).reshape(-1, 5)
        self.P = np.array(calib['projection_matrix']['data']).reshape(3, 4)
        self.R = np.array(calib['rectification_matrix']['data']).reshape(3, 3)
        self.img_width = calib['image_width']
        self.img_height = calib['image_height']
    def process(self,points):
        lpts_ud=cv2.undistortPoints(points.reshape(-1,1,2).astype(np.float32), self.K, self.D,P=self.P,R=self.R)
        return lpts_ud#cv2.convertPointsToHomogeneous(np.float32(lpts_ud))

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


pose_marker=Pose2D()

def node_process():
    with open('extrinsic_params.yaml','r') as f:
    	H=np.array(yaml.safe_load(f)).reshape(3,3)
    distorsionProcessor = undistrodMarkers('ost.yaml')
    markerExteractor_inst=markerExteractor()
    cap=cv2.VideoCapture('debug_video.avi')
    pub = rospy.Publisher('planar_pose', Pose2D, queue_size=10)
    rospy.init_node('aras_planar_tracker_node', anonymous=True)
    pose_marker=Pose2D()
    while not rospy.is_shutdown():
        ret,img=cap.read()
	if ret==True:
        	points=markerExteractor_inst.detect(img)
        	if points is not None:
            		for i in range(len(points)):
        			points_undistorded=distorsionProcessor.process(points[0])
        			marker_pose=np.matmul(H, np.hstack((points_undistorded[0][0], 1)))
        			marker_pose=marker_pose/marker_pose[2]
				pose_marker.x=marker_pose[0]
				pose_marker.y=marker_pose[1]
		else:
			pose_marker.x=-1
			pose_marker.y=-1
	else:
		break
	pub.publish(pose_marker)

if __name__ == '__main__':
    try:
        node_process()
    except rospy.ROSInterruptException:
        pass
