#include "ros/ros.h"
#include "second_assignment/Accelerate.h"
#include "std_srvs/Empty.h"

// Global variable of the velocity factor that will be set as the value of the server's response.
float acc=0;

// Colors for konsole prints.
const char *red = "\033[0;31m";
const char *green = "\033[1;32m";
const char *yellow = "\033[1;33m";
const char *reset = "\033[0m";

// bool function of the server.
bool char_input (second_assignment::Accelerate::Request &req, second_assignment::Accelerate::Response &res)
{
    // Declaration of the server needed to call the reset function.
	std_srvs::Empty reset_srv;
    
    // If the input service is 'a', a .5 factor will be added to the acc global variable.
	if (req.input=='a'){
	
		std::printf("got %sa%s\n",green,reset);
		acc =acc+0.5;
		
		
	}
    
    // If the input service is 's', a .5 factor will be taken off the acc global variable.
	if (req.input == 's'){
	
		std::printf("got %ss%s\n",yellow,reset);
		acc =acc-0.5;
		
	}
    
    // If the input service is 'r', the position of the robot will be reset to the starting position.
	if (req.input == 'r'){
	
		std::printf("got %sr%s\n",red,reset);
		
		ros::service::call("/reset_positions", reset_srv);
        
        // Setting the acceleration factor to 0.
		acc = 0.0;
	}
	
    // If any other character is pressed an error message will show up.
	if (req.input != 's' && req.input != 'r' && req.input != 'a' ){
		std::printf("%swrong input%s\n",red,reset);
		
	}
	
    
	res.val = acc;
	
	return true; 
}


int main(int argc, char **argv) {

    // Initialize the node, setup the NodeHandle for handling the communication with the ROS system.
	ros::init(argc, argv, "input");
	ros::NodeHandle n;
    
    // Initialize the service.
	ros::ServiceServer service = n.advertiseService("/accelerate",char_input);
    
    
	ros::spin();
    
    // Function to loop the callback function.
	return 0; 
}
