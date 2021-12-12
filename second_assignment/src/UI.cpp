#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Spawn.h"
#include "sensor_msgs/LaserScan.h"
#include "second_assignment/Accelerate.h"
#include "second_assignment/Acc.h"
#include "std_srvs/Empty.h"

// Publisher to the custom topic.
ros::Publisher pub;

// Client to the custom service to get the response for updateing the velocity factor.
ros::ServiceClient client1;

// Colors for konsole prints.
const char *red = "\033[0;31m";
const char *green = "\033[1;32m";
const char *yellow = "\033[1;33m";
const char *reset = "\033[0m";

// Function needed for the UI_node to always be ready to ask the user for encreasing or decreasing the robot's speed.
void Interaction()
{
	
	// Custom message with which will send the updated data to the controller.
	second_assignment::Accelerate acc_srv;
		
    // Section due to retriving input from the user.
	char in;
	std::printf("Input %s[a]%s to %saccelerate%s | Input %s[s]%s to %sdecelerate%s | Input %s[r]%s to %sRESET%s:\n",green,reset,green,reset,yellow,reset,yellow,reset,red,reset,red,reset);
	
	std::cin>>in;
		
    // The request is set as the user's input value.
	acc_srv.request.input= in;
	client1.waitForExistence();
	client1.call(acc_srv);
	
    // Declaration of the castom message object.
	second_assignment::Acc my_acc;
	
    // Assigning the value of the response of the service to the custom message value.
	my_acc.a = acc_srv.response.val;
    
    // Publishing the value to the correct topic.
	pub.publish(my_acc);
	
}


int main (int argc, char **argv) 
{
	// Initialize the node, setup the NodeHandle for handling the communication with the ROS system.
	ros::init(argc, argv, "UI"); 
	ros::NodeHandle nh;
    
	
    // Initialization of the the custom topic's publisher.
	pub = nh.advertise<second_assignment::Acc> ("/acc", 1);
    
    // Initialization of the client to the custom service.
	client1 = nh.serviceClient<second_assignment::Accelerate>("/accelerate");
    
    // Loop of the function that asks the user what to do.
    while(ros::ok()){
        
        Interaction();
        
        // Function to loop the callback function.
        ros::spinOnce();
    }
	
	return 0;
}
