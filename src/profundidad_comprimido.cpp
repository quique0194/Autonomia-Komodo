/*
    Este codigo muestra la forma de:
        - Leer del sensor de profundidad comprimido
*/

#include <pthread.h>

#include <iostream>
using namespace std;

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include "sensor_msgs/CompressedImage.h"
#include "sensor_msgs/Image.h"
#include "image_transport/image_transport.h"
#include "cv_bridge/cv_bridge.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


geometry_msgs::Twist msg;

void* wasd(void* arg) {

}

void odomPubCallback(const nav_msgs::Odometry::ConstPtr& msg) {
}


void depthCallback(const sensor_msgs::Image::ConstPtr& msg) {
    cv_bridge::CvImageConstPtr cvImg = cv_bridge::toCvShare(msg);
    // cout << cvImg->image.size() << endl;
    cv::Mat mat;
    cv::convertScaleAbs(cvImg->image, mat, 25);
    cv::imshow("My window", mat);
}


int main(int argc, char** argv) {
    ros::init(argc, argv, "movimiento");
    
    cv::namedWindow("My window");
    cv::startWindowThread();

    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/komodo_1/komodo_1_Asus_Camera/depth/image", 1, depthCallback);
    ros::spin();
    cv::destroyWindow("My window");
    return 0;
}

