#include "ros/ros.h"
#include "second_assignment/Accelerate.h"

float acc=0;



bool char_input (second_assignment::Accelerate::Request &req, second_assignment::Accelerate::Response &res)
{ 
	
	if (req.input=='a'){
	
		std::cout<<"got a\n";
		acc =acc+0.5;
		
		
	}
	else if (req.input == 's'){
	
	std::cout<<"got s\n";
		acc =acc-0.5;
		
	}
	
	else{
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
