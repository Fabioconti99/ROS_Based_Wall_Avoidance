#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Spawn.h"
#include "sensor_msgs/LaserScan.h"
#include "second_assignment/Accelerate.h"
#include "second_assignment/Acc.h"
#include "std_srvs/Empty.h"


ros::Publisher pub;
geometry_msgs::Twist my_vel;

float acc=0;



float mini ( float arr[720], int ind=0, int size=720){


	float val = 30.0;
	for(int i= ind; i<ind+size; i++){
	
		if (val>=arr[i]){
			val=arr[i];
		}
	}
	return val;
}


void robotCallback(const sensor_msgs::LaserScan::ConstPtr& msg/*, const second_assignment::Acc::ConstPtr& msg1*/)
{

	
	ROS_INFO("Turtle subscriber@ [%f] ",msg->ranges[360]);
		
		float laser[721];
		
		for(int i=0;i<721;i++){
		laser[i]=msg->ranges[i];
		}
		
		if (mini(laser,310,100)>1.5 ){
		
			my_vel.linear.x = 1.0+acc;
			my_vel.angular.z = 0.0; 
				
		} 
		
		else{
		
			if (mini(laser,0,100)<mini(laser,620,100)){
			
				my_vel.linear.x = 0.2;
				my_vel.angular.z = 1.0; 
			}
			
			else{
			
				my_vel.linear.x = 0.2;
				my_vel.angular.z = -1.0; 
			
			}
		
		
		}
		 
		 
		 if (mini(laser, 0,50)<0.2){
		 	my_vel.linear.y= 1.5;
		 }
		 else {
		 my_vel.linear.y= 0.0;
		 }
		 
		 
		 
		 if (mini(laser, 620,50)<0.2){
		 	my_vel.linear.y= -1.5;
		 }
		 else {
		 my_vel.linear.y= 0.0;
		 }
		
		pub.publish(my_vel);



}

void accelerator(const second_assignment::Acc::ConstPtr& msg1)
{
	acc=msg1->a;
	std::cout<<msg1->a<<"\n";
}


int main (int argc, char **argv) 
{
	// Initialize the node, setup the NodeHandle for handling the communication with the ROS //system
	ros::init(argc, argv, "robot_controller"); 
	ros::NodeHandle nh;


	ros::Subscriber sub = nh.subscribe("/base_scan", 1000,robotCallback); 
	ros::Subscriber sub1 = nh.subscribe("/acc", 1000,accelerator);
	
	pub = nh.advertise<geometry_msgs::Twist> ("/cmd_vel", 1000);

	ros::spin();
	
	return 0;
}
