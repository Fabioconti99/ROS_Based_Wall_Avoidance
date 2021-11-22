#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Spawn.h"
#include "sensor_msgs/LaserScan.h"
#include "second_assignment/Accelerate.h"
#include "second_assignment/Acc.h"
#include "std_srvs/Empty.h"

ros::Publisher pub;
ros::ServiceClient client1;


void robotCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
	ROS_INFO("subscriber@[%f]", msg->ranges[360]);
	
	
	second_assignment::Accelerate acc_srv;
		
	char in;
	std::cout<<"input 'a' to accelerate, 's' to slow down or 'r' to RESET:\n";
	std::cin>>in;
		
	acc_srv.request.input= in;
		
	client1.waitForExistence();
	client1.call(acc_srv);
	
	second_assignment::Acc my_acc;
	
	my_acc.a = acc_srv.response.val;
	
	pub.publish(my_acc);
	
}


int main (int argc, char **argv) 
{
	// Initialize the node, setup the NodeHandle for handling the communication with the ROS //system
	ros::init(argc, argv, "UI"); 
	ros::NodeHandle nh;
	// Define the subscriber to turtle's position

	ros::Subscriber sub = nh.subscribe("/base_scan", 1000,robotCallback); 
	
	pub = nh.advertise<second_assignment::Acc> ("/acc", 1);
	client1 = nh.serviceClient<second_assignment::Accelerate>("/accelerate");
	
	
	ros::spin();
	return 0;
}
