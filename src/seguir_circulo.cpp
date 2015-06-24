/*
    Este codigo muestra la forma de:
        - Seguir un circulo blanco en la pared
    Para ejecutar:
        - rosrun autonomia_komodo seguir_circulo _image_transport:=compressed
*/

#include <stdio.h>

#include <iostream>
#include <vector>

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

#include "PID.h"


geometry_msgs::Twist vel;

cv::Mat depth, img;
std::vector<cv::Vec3f> circles;
int circle_param_1 = 150, circle_param_2 = 50;
// double dist_to_obstacle = -1;
double dist_to_obstacle = 5;


namespace utils {
    double uniform () {
        return ((double) rand() / (RAND_MAX));
    }
}


void depthCallback (const sensor_msgs::Image::ConstPtr& msg) {
    cv_bridge::CvImageConstPtr cvImg = cv_bridge::toCvShare(msg);
    depth = cvImg->image; // decimals here

    if (circles.size() != 1) {
        dist_to_obstacle = -1;
        return;
    }

    cv::Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
    int radius = cvRound(circles[0][2]);

    int random_points = 5;
    double sum = 0;
    for (int i = 0; i < random_points; ++i) {
        double h = utils::uniform() * radius;
        double t = utils::uniform() * 2 * M_PI;
        sum += depth.at<double>(h*sin(t), h*cos(t));
    }
    dist_to_obstacle = sum/random_points;
    std::cout << "Dist to obstacle: " << dist_to_obstacle << std::endl;
}


void imgCallback (const sensor_msgs::Image::ConstPtr& msg) {
    cv_bridge::CvImageConstPtr cvImg = cv_bridge::toCvShare(msg);
    img = cvImg->image; // colors here
    // std::cout << img.size() << std::endl;
    cv::Mat drawing = img;

    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);

    cv::HoughCircles(img_gray,
                     circles,
                     CV_HOUGH_GRADIENT, 1,
                     img_gray.rows/8,
                     circle_param_1,
                     circle_param_2, 0, 0);

    for( size_t i = 0; i < circles.size(); i++ ) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        cv::circle(drawing, center, 3, cv::Scalar(0,255,0), -1, 8, 0);
        // circle outline
        cv::circle(drawing, center, radius, cv::Scalar(0,0,255), 3, 8, 0);
    }

    cv::imshow("My window", drawing);
}


void updateVel () {
    if (circles.size() != 1) {
        vel.linear.x = 0.0;
        vel.angular.z = 0.0;
        return;
    }

    PID pid_angular(0.01, 1);
    cv::Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
    double errorAngular = img.cols/2.0 - center.x;
    vel.angular.z = pid_angular.calculate(errorAngular);
    std::cout << "Update vel.angular: " << vel.angular.z << std::endl;

    if (dist_to_obstacle < 1) {
        vel.linear.x = 0.0;
        return;
    } else {
        vel.linear.x = 0.2;
    }
}


int main (int argc, char** argv) {
    srand(0);

    ros::init(argc, argv, "movimiento");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/komodo_1/cmd_vel", 10);
    image_transport::ImageTransport it(nh);
    char depth_topic[] = "/komodo_1/komodo_1_Asus_Camera/depth/image";
    // char rbg_topic[] = "/komodo_1/komodo_1_Asus_Camera/rgb/image_raw";
    char rbg_topic[] = "/webcam/image";
    image_transport::Subscriber sub1 = it.subscribe(rbg_topic, 1, imgCallback);
    image_transport::Subscriber sub2 = it.subscribe(depth_topic, 1, depthCallback);
    ros::Rate loop_rate(100);
    
    cv::namedWindow("My window");
    cv::createTrackbar("Circle param 1", "My window", &circle_param_1, 500, NULL);
    cv::createTrackbar("Circle param 2", "My window", &circle_param_2, 500, NULL);
    cv::startWindowThread();

    while (ros::ok()) {
        updateVel();
        pub.publish(vel);
        ros::spinOnce();
        loop_rate.sleep();
    }

    vel.linear.x = 0.0;
    vel.angular.z = 0.0;
    pub.publish(vel);
    ros::spinOnce();

    return 0;
}
