#include "ros/ros.h"
#include <opencv/cv.hpp>
#include "../include/markerdetector.h"
#include "../include/stereoProcess.h"
#include "camera_calibration_parsers/parse.h"
#include "sensor_msgs/CameraInfo.h"
#include "sensor_msgs/Image.h"
#include "cv_bridge/cv_bridge.h"
#include <ros/package.h>
#include <stereo_image_publisher/stereo_camera.h>
#include <geometry_msgs/Pose.h>
#include <tf/transform_broadcaster.h>

#include <ir_tracker_node_3D/irtracker.h>
static  ir_tracker_node_3D::irtracker tracker;

using namespace cv;

static Mat leftCamF;
static Mat rightCamF;
static int initilized_flag=0;
static double leftCamX,rightCamX,leftCamY,rightCamY;
static double posX,posY,posZ;
static markerDetector leftCamMarker;
static markerDetector rightCamMrker;
static camParameters_type camParameters;
static stereoProcess *stereoProcessor;
static  geometry_msgs::Pose pos_res;
static ros::Publisher pos_pub;

void ImageCalback(const stereo_image_publisher::stereo_camera::ConstPtr& msg)
{
  cv_bridge::CvImagePtr cv_ptr_left ,cv_ptr_right;
  try
  {
    cv_ptr_left = cv_bridge::toCvCopy(msg->leftCamera, sensor_msgs::image_encodings::MONO8);
    cv_ptr_right = cv_bridge::toCvCopy(msg->rightCamera, sensor_msgs::image_encodings::MONO8);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
  leftCamF =  cv_ptr_left->image;
  rightCamF =  cv_ptr_right->image;
  if(initilized_flag==0)
  {
    initilized_flag=1;
    camParameters.leftCamMatrix =Mat(3, 3, CV_64FC1, (void *) msg->leftCameraInfo.K.data());
    camParameters.leftCamDistor =Mat(1, 5, CV_64FC1, (void *) msg->leftCameraInfo.D.data());
    camParameters.leftR =Mat(3, 3, CV_64FC1, (void *) msg->leftCameraInfo.R.data());
    camParameters.projLeft =Mat(3, 4, CV_64FC1, (void *) msg->leftCameraInfo.P.data());

    camParameters.rightCamMatrix =Mat(3, 3, CV_64FC1, (void *) msg->rightCameraInfo.K.data());
    camParameters.rightCamDistor =Mat(1, 5, CV_64FC1, (void *) msg->rightCameraInfo.D.data());
    camParameters.rightR =Mat(3, 3, CV_64FC1, (void *) msg->rightCameraInfo.R.data());
    camParameters.projRight =Mat(3, 4, CV_64FC1, (void *) msg->rightCameraInfo.P.data());
    stereoProcessor = new stereoProcess(camParameters);
  }
  leftCamMarker.getPixelPos(leftCamF,&leftCamX,&leftCamY);
  rightCamMrker.getPixelPos(rightCamF,&rightCamX,&rightCamY);
  stereoProcessor->Process2(rightCamX,rightCamY,leftCamX,leftCamY,&posX,&posY,&posZ);
  tracker.pose.position.x=posX;
  tracker.pose.position.y=posY;
  tracker.pose.position.z=posZ;
  tracker.header.stamp=ros::Time::now();
  pos_pub.publish(tracker);

  //pos_res.position.x=posX;
  //pos_res.position.y=posY;
  //pos_res.position.z=posZ;
  //pos_pub.publish(pos_res);
  static tf::TransformBroadcaster br;
  tf::Transform transfrm;
  transfrm.setOrigin(tf::Vector3(posX,posY,posZ));
  transfrm.setRotation(tf::Quaternion(tf::Vector3(1,0,0),0));
  br.sendTransform(tf::StampedTransform(transfrm,ros::Time::now(),"ir_tracker","marker"));
  //ROS_INFO("x=%f\ty=%f\tz=%f",posX,posY,posZ);
}
int main(int argc, char *argv[])
{
  ros::init(argc, argv, "ir_tracker_node_3d");
  ros::NodeHandle nh;
  ros::Subscriber sub1 = nh.subscribe("/aras_stereo_camera", 100, ImageCalback);
  pos_pub=nh.advertise<ir_tracker_node_3D::irtracker>("\ir_tracker", 1000);

  //std::string pkg_path = ros::package::getPath("ir_tracker_node_3D");
  //sensor_msgs::CameraInfo leftCameraInfo;
  //sensor_msgs::CameraInfo rightCameraInfo;
  //string leftCamName("narrow_stereo/left");
  //string rightCamName("narrow_stereo/right");
  //camera_calibration_parsers::readCalibration(pkg_path+"/left.yaml",leftCamName,leftCameraInfo);
  //camera_calibration_parsers::readCalibration(pkg_path+"/right.yaml",rightCamName,rightCameraInfo);
  ros::spin();
  return 1;
}
