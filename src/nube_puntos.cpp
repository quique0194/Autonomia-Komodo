/*
    Este codigo muestra la forma de:
        - Leer la nube de puntos del komodo
*/

#include <pthread.h>

#include <iostream>
using namespace std;

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include "sensor_msgs/PointCloud2.h"
#include "pcl_ros/point_cloud.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


geometry_msgs::Twist msg;

void* wasd(void* arg) {

}

void odomPubCallback(const nav_msgs::Odometry::ConstPtr& msg) {

}

void MatFromPointCloud(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud, cv::Mat& result) {
    // cout << cloud->isOrganized() << " " << cloud->empty() << endl;
    if (cloud->isOrganized()) {
        result = cv::Mat(cloud->height, cloud->width, CV_8UC3);

        if (!cloud->empty()) {

            for (int h=0; h<result.rows; h++) {
                for (int w=0; w<result.cols; w++) {
                    pcl::PointXYZRGB point = cloud->at(w, h);

                    Eigen::Vector3i rgb = point.getRGBVector3i();

                    result.at<cv::Vec3b>(h,w)[0] = point.z*25;//rgb[2];
                    result.at<cv::Vec3b>(h,w)[1] = point.z*25;//rgb[1];
                    result.at<cv::Vec3b>(h,w)[2] = point.z*25;//rgb[0];
                }
            }
        }
    }
}

void depthCallback(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& msg) {
    cv::Mat mat;
    MatFromPointCloud(msg, mat);
    cv::imshow("My window", mat);
    cout << "############" << endl;
}
/*
void depthCallback(const sensor_msgs::PointCloud2::ConstPtr& msg) {
    for (int i = 0; i < msg->fields.size(); ++i) {
        cout << msg->fields[i].name << " ";
        cout << msg->fields[i].offset << " ";
        cout << (unsigned int)msg->fields[i].datatype << " ";
        cout << msg->fields[i].count << endl;
    }
    //cout << msg->height << " " << msg->width << endl;
    //cout << msg->data.size() << endl;
    // cout << msg->data << endl;
}
*/
int main(int argc, char** argv) {
    ros::init(argc, argv, "movimiento");
    
    cv::namedWindow("My window");
    cv::startWindowThread();

    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("/komodo_1/komodo_1_Asus_Camera/depth_registered/points", 10, depthCallback);
    ros::spin();
    return 0;
}

