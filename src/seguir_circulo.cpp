/*
    Este codigo muestra la forma de:
        - Seguir un circulo blanco en la pared
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
    char ARRIBA = 82;
    char ABAJO = 84;
    char DERECHA = 83;
    char IZQUIERDA = 81;
    char CERO = -80;

    cout << "Mover con WASD, presionar enter luego de cada comando" << endl;
    char c;
    while (true) {
        c = cv::waitKey(1);
        if (c == ARRIBA) {
            msg.linear.x += 0.5;
        } else if (c == ABAJO) {
            msg.linear.x -= 0.5;
        } else if (c == DERECHA) {
            msg.angular.z -= 1;
        } else if (c == IZQUIERDA) {
            msg.angular.z += 1;
        } else if (c != -1) {
            msg.linear.x = 0.0;
            msg.angular.z = 0.0;
        }
    }
}

// void depthCallback(const sensor_msgs::Image::ConstPtr& msg) {
//     cv_bridge::CvImageConstPtr cvImg = cv_bridge::toCvShare(msg);
//     // cout << cvImg->image.size() << endl;
//     cv::Mat mat;
//     cv::convertScaleAbs(cvImg->image, mat, 25);
//     cv::imshow("My window", mat);
// }

void imgCallback(const sensor_msgs::Image::ConstPtr& msg) {
    cv_bridge::CvImageConstPtr cvImg = cv_bridge::toCvShare(msg);
    // cout << cvImg->image.size() << endl;
    cv::Mat mat = cvImg->image;
    // cv::convertScaleAbs(cvImg->image, mat, 25);
    cv::imshow("My window", mat);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "movimiento");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/komodo_1/cmd_vel", 10);
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/komodo_1/komodo_1_Asus_Camera/depth/image", 1, imgCallback);
    ros::Rate loop_rate(100);
    
    cv::namedWindow("My window");
    cv::startWindowThread();

    pthread_t t1;
    pthread_create(&t1, NULL, &wasd, NULL);

    while (ros::ok()) {
        //ROS_INFO("Linear.x: %f, Angular.z: %f", msg.linear.x, msg.angular.z);
        pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
    }

    msg.linear.x = 0.0;
    msg.angular.z = 0.0;
    //ROS_INFO("Linear.x: %f, Angular.z: %f", msg.linear.x, msg.angular.z);
    pub.publish(msg);
    ros::spinOnce();
    return 0;
}

