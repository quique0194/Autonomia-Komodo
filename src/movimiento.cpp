#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"

#include <iostream>
#include <pthread.h>
using namespace std;

geometry_msgs::Twist msg;

void* wasd(void* arg) {
    char c;
    cout << "Mover con WASD, presionar enter luego de cada comando" << endl;
    while (true) {
        c = cin.get();
        cin.ignore(99999, '\n');
        if (c == 'w') {
            msg.linear.x += 0.1;
        } else if (c == 's') {
            msg.linear.x -= 0.1;
        } else if (c == 'd') {
            msg.angular.z -= 0.5;
        } else if (c == 'a') {
            msg.angular.z += 0.5;
        } else if (c == '0') {
            msg.linear.x = 0;
            msg.angular.z = 0;
        }
    }
}

void odomPubCallback(const nav_msgs::Odometry::ConstPtr& msg) {
    ROS_INFO("Odom_pub x: %f, y: %f, z: %f",
             msg->pose.pose.position.x,
             msg->pose.pose.position.y,
             msg->pose.pose.position.z);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "movimiento");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/komodo_1/cmd_vel", 10);
    ros::Subscriber sub = n.subscribe("/komodo_1/odom_pub", 10, odomPubCallback);
    ros::Rate loop_rate(100);
    
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

