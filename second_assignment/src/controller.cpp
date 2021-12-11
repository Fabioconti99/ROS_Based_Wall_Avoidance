#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Spawn.h"
#include "sensor_msgs/LaserScan.h"
#include "second_assignment/Accelerate.h"
#include "second_assignment/Acc.h"
#include "std_srvs/Empty.h"

// Initiaslization of the publisher needed to send the velocity values through the
// /cmd_vel topic to the simulation node.
ros::Publisher pub;

// Twist message object for assigning the robot's speed.
geometry_msgs::Twist my_vel;

// Initialization of the coefficient added to the linear velocity param in order to encrise the total speed's value.
float acc=0;


// Function to retrive the lower value among a given span of the laser scan set.
float mini ( float arr[720], int ind=0, int size=720){


	float val = 30.0;
	for(int i= ind; i<ind+size; i++){
	
		if (val>=arr[i]){
			val=arr[i];
		}
	}
	return val;
}

// CallBack needed by the node to costantly retrieve data about the laser-scans.

// Subsctiption to the /base_scan  topic which contains the updated data about what the laser-scanners see.
void robotCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
	// Shell print of the just one of the updated values retrived by the laserScan set.
	ROS_INFO("Turtle subscriber@ [%f] ",msg->ranges[360]);
    
    
    
        // Assigning all the retrived values to a different array in order to exploit the collected data.
		float laser[721];
		for(int i=0;i<721;i++){
		laser[i]=msg->ranges[i];
		}
		
        // If the closest distance detected by the lasers'set in fornt of the robot is higher than 1.5 units the robot will
        // travel straight.
		if (mini(laser,310,100)>1.5 ){
		
			my_vel.linear.x = 1.0+acc;
			my_vel.angular.z = 0.0; 
				
		} 
		
    // If the closest distance detected by the set of lasers in fornt of the
    // robot is lower than 1.5 units:
		else{
            
            // the robot will check the sides and it will turn towards the opposit
            // direction of the closest wall.
            // Following this rule, the robot will avoid hitting the wall.
            
            // If the closest wall detected is on the RIGHT SIDE.
			if (mini(laser,0,100)<mini(laser,620,100) /*&& mini(laser,0,100)<mini(laser,200,100) && mini(laser,0,100)<mini(laser,420,100)*/){
			
				my_vel.linear.x = 0.1;
				my_vel.angular.z = 1.0; 
			}
			
            // If the closest wall detected is on the LEFT SIDE.
			if (mini(laser,620,100)<mini(laser,0,100) /*&& mini(laser,620,100)<mini(laser,200,100) && mini(laser,620,100)<mini(laser,420,100)*/){
			
				my_vel.linear.x = 0.1;
				my_vel.angular.z = -1.0; 
			
			}
            
            // If the closest wall detected is on the UPPER RIGHT SIDE.
			//if (mini(laser,200,100)<mini(laser,620,100) && mini(laser,200,100)<mini(laser,0,100) && mini(laser,200,100)<mini(laser,420,100)){
			
			//	my_vel.linear.x = 0.3;
			//	my_vel.angular.z = 1.0; 
			
			//}
            
            // If the closest wall detected is on the UPPER LEFT SIDE.
			//if (mini(laser,420,100)<mini(laser,200,100) && mini(laser,420,100)<mini(laser,620,100) && mini(laser,420,100)<mini(laser,0,100)){
			
			//	my_vel.linear.x = 0.3;
			//	my_vel.angular.z = -1.0; 
			
			//}
			
		}
		 
		// Pubblishing the velocity values
		pub.publish(my_vel);



}

// CallBack function to update the velocity factor.
void accelerator(const second_assignment::Acc::ConstPtr& msg1)
{
    // Assigning the updated accelleration value to the acc global variable.
	acc=msg1->a;
    
    // Returnung to the shell the value the newly-updated velocity coefficient.
    ROS_INFO("[%f]",acc+1.0);
}


int main (int argc, char **argv) 
{
	// Initialize the node, setup the NodeHandle for handling the communication with the ROS system.
	ros::init(argc, argv, "robot_controller"); 
	ros::NodeHandle nh;
    
    // Initialize the two subscribers for both the call backs.
	ros::Subscriber sub = nh.subscribe("/base_scan", 1000,robotCallback);
	ros::Subscriber sub1 = nh.subscribe("/acc", 1000,accelerator);
	
    // Initialize the publisher needed to publish the new velocity values to update the robot's speed.
	pub = nh.advertise<geometry_msgs::Twist> ("/cmd_vel", 1000);
    
    // Function to loop the callback functions.
	ros::spin();
	
	return 0;
}
