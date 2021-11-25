#include "ros/ros.h"
#include "second_assignment/Accelerate.h"
#include "std_srvs/Empty.h"

float acc=0;

bool char_input (second_assignment::Accelerate::Request &req, second_assignment::Accelerate::Response &res)
{ 
	std_srvs::Empty reset_srv;
	if (req.input=='a'){
	
		std::cout<<"got a\n";
		acc =acc+0.5;
		
		
	}
	if (req.input == 's'){
	
		std::cout<<"got s\n";
		acc =acc-0.5;
		
	}
	if (req.input == 'r'){
	
		std::cout<<"got r\n";
		
		ros::service::call("/reset_positions", reset_srv);
		
	}
	
	if (req.input != 's' && req.input != 'r' && req.input != 'a' ){
		std::cout<<"wrong char\n";
		
	}
	
	res.val = acc;
	std::cout<<res.val<<"\n";
	return true; 
}


int main(int argc, char **argv) {

	ros::init(argc, argv, "input");
	ros::NodeHandle n;
	ros::ServiceServer service = n.advertiseService("/accelerate",char_input);
	ros::spin();
	return 0; 
}
