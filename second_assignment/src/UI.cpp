#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Spawn.h"
#include "sensor_msgs/LaserScan.h"
#include "second_assignment/Accelerate.h"
#include "second_assignment/Acc.h"
#include "std_srvs/Empty.h"

// Publisher to the custom topic
ros::Publisher pub;

// Client to the custom service to get the response for updateing the velocity
// factor.
ros::ServiceClient client1;


// CallBack needed for the UI_node to always get the info
void robotCallback(/*const sensor_msgs::LaserScan::ConstPtr& msg*/)
{
	// ROS_INFO("subscriber@[%f]", msg->ranges[360]);
	
	// Custom message with which it'll send the updated data to the controller.
	second_assignment::Accelerate acc_srv;
		
    // Input to retrive the input from the user.
	char in;
	std::cout<<"input 'a' to accelerate, 's' to slow down or 'r' to RESET:\n";
	std::cin>>in;
		
    // The request is set as the value input by the user.
	acc_srv.request.input= in;
	client1.waitForExistence();
	client1.call(acc_srv);
	
    // Declaration of the castom message message
	second_assignment::Acc my_acc;
	
    // Assigning the value of the response of the service to the custom message
    // value
	my_acc.a = acc_srv.response.val;
    
    // Publishing the value
	pub.publish(my_acc);
	
}


int main (int argc, char **argv) 
{
	// Initialize the node, setup the NodeHandle for handling the communication
    // with the ROS system
	ros::init(argc, argv, "UI"); 
	ros::NodeHandle nh;
    
	// Define the subscriber to robot surroundings detection.
	// ros::Subscriber sub = nh.subscribe("/base_scan", 1000,robotCallback);
	
    // Initialization of the publisher on the custom topic.
	pub = nh.advertise<second_assignment::Acc> ("/acc", 1);
    
    // Initialization of the client to the service.
	client1 = nh.serviceClient<second_assignment::Accelerate>("/accelerate");
    
    while(ros::ok()){
        
        robotCallback();
        // Callback of the callback function.
        ros::spinOnce();
    }
	
	return 0;
}
