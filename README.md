 #  [Research_Track_1](https://unige.it/en/off.f/2021/ins/51201.html?codcla=10635) , [Robotics Engineering](https://courses.unige.it/10635) ([UNIGE](https://unige.it/it/)) : Second assignement
## Robot Operating System <img height="30" src="https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/ros.png"> & C++ <img height="30" src="https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/cpl.png">
### Professor. [Carmine Recchiuto](https://github.com/CarmineD8)


--------------------
Project objectives
--------------------
The aim of this project was to create a ROS Pakage capable of make a cubical robot equiped with laser scanners, lap around a given circuit. The package contains the C++ surces needed for the interaction with the robot. The given enviroment is a simple 3D model of the actual Monza's F1 circuit.
ROS (Robot Operating System) is a set of tools and libraries for autonomous, robust, efficient, and secure mobile robot navigation, which uses sensors to sense the environment and computer vision techniques as the navigation method.

The behavior of the robot has to stand by the following rules:
* Constantly driving around the environment in the clockwise direction,
* Avoiding to hit the walls, 
* Being able to either **accelerate** or **decelerate** if the user will ask to do so,
* And **Resetting** it's own position to the starting one if asked to. 

The user will be able to interact with the simulation through the use of **KeyBoard inputs**:
* Pressing the `r` key will RESET the initial position of the robot.
* Pressing the `s` key will SLOW DOWN the speed of the robot by **half unit** at every tap of the button.
* Pressing the `a` key will INCREASE the speed of the robot by **half unit** at every tap of the button.

Picture of the **Enviroment**:

![alt text](https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/map.png) 

With everything working correctly, the robot should lap around the circuit avoiding the walls at different velocities on an infinite loop. If the velocity would get too high, it may lead to the robot crashing into the wall but that would be perfectly reasonable. 

--------

Installing and running
----------------------

The simulation requires a ROS Noetic installation, and the clone of the [Teacher's repository](https://github.com/CarmineD8/second_assignment). The repo contains the workspace for running the simulation node. The source includes The *CmakeList* for compiling the node and the *World* file for building the scene.
The package I developed contains:

* A *Controller node* which will comand the movemnts and the accelleration of the robot.
* A *UI node* to let the user interact with the simulation with the KeyBoard Inputs. 
* A *Server node* that will process the KeyBoard Input and modify the acceleration factor accordingly to the pressed key. 
* ONE *custom service* needed by the server node to exchange data between the *request* and the *response*.
* ONE *custom message* needed to pass the acceleration factor from the service to the controller through the use of a topic.


## Running the simulation

Please follow these steps to run the Circuit scene:

1. Open a terminal and run the master, using the command:
```bash
roscore&
```
2. In the same terminal run the **world simulation** with the command:
```bash
rosrun stage_ros stageros $(rospack find second_assignment)/world/my_world.world
```
3. In another terminal run the **server node** with the command:
```bash
rosrun second_assignment input_node
```
4. In another terminal run the **controller node** with the command:
```bash
rosrun second_assignment UI_node
```
5. In another terminal run the **UI node** with the command:
```bash
rosrun second_assignment robot_controller_node
```

------
NODES Description
-------------------

## Simulation Node: StageRos

The node used for the simulation is the standard node `StageRos`. The stageros node wraps the Stage 2-D multi-robot simulator, via libstage. Stage simulates a world as defined in a .world file. This file tells stage everything about the world, from obstacles (usually represented via a bitmap to be used as a kind of background), to robots and other objects.

This node only exposes a subset of Stage's functionality via ROS. Specifically, it finds the Stage models of type laser, camera and position, and maps these models to the ROS topics given below. If at least one laser/camera and position model are not found, stageros exits.

Stageros only exposes models created by a subset of the `.world` file syntax, specifically `laser`, `position` and `camera` models.

* Parameters:
    * world : The Stage .world file to load.
    * -g : If set, this option will run stage as "headless," no GUI will be displayed.
    
* The node **subscribes** to the `cmd_vel geometry_msgs/Twist` Topic to manage the velocity commands to drive the position model of the robot.

* The node **publishes** to the following topics:

    * `odom (nav_msgs/Odometry)`: it pubs odometry data from the position model.
    * `base_scan (sensor_msgs/LaserScan)`:  it pubs scans from the laser model
    * `base_pose_ground_truth (nav_msgs/Odometry)`:  it pubs the ground truth pose.
    * `image (sensor_msgs/Image)`: it pubs the visual camera image.
    * `depth (sensor_msgs/Image)`: it pubs depth camera image.
    * `camera_info (sensor_msgs/CameraInfo)`:  it pubs camera calibration info.
    
    * Usage:
```bash
    rosrun stage_ros stageros [-g runs headless] <world> [standard ROS args]
```

------
## Controller Node: robot_controller_node

This node is used to control the robot's driving capabilities inside the circuit. It will command the robot to drive at a certain velocity inside the circuit. thanks to this node the robot will also be able to scan the walls of the track and not hit them. 
To do so, the robot robot subscibes and publishes to the following topics:

* The node **publishes** to the `cmd_vel geometry_msgs/Twist` Topic to change the *linear* and *angular* velocity's values related to the robot.

* The node **subscribes** to the following topics:

    * `base_scan (sensor_msgs/LaserScan)`:  which will provide the node with Data about the enviroment surrouding the robot.
    * `Acc (second_assignment/Acc)`:  Which will tell the node how much to increase the *linear* velocity of the robot on the `cmd_vel` Topic.
    
On the  `cmd_vel` Topic, the Simulation Node pubblishes all the information acquired by the laserScanner set. The robot is equiped with 721 lasers that retrive information about the relative distance between the center of the robot and the walls of the circuit. The lasers reach a 30 units distance and they are placed on a 180º degrees span infornt of the robot. The scanning Rate is 10 hz.
The following picture gives a visual rappresentation of the lasers inside the enviroment:

<p align="center">

</p>

Thanks to the subscription of the node to the `base_scan` topic, the *CallBack* function of the node will always be able to know what the robot sees infront of it. 
To let the robot make its way around the circuit, I implemented a simple code which exploits the Data acquired by the laser scans.
The Data comming from the scanners is composed by two different arrays. One related to the **intecity** of the lasers the other one to the **scan ranges**. I used the second array to collect the relaiable information to manage the robot's navigation speed. 
I devided the area covered by the lasers in three main areas each one composed by 100 lasers. The first one covers form the the index 0 to 100, the following one form 310 to 410 and the last one from 620 to 720. Thanks to the function `mini(float array [720], int starting_index, int n_lasers)`, the detecting areas will return the minimum distance value registerd by the lasers. 

* *INPUTS*:
    * `float arr[720]`: The ranges' array. 
    * `int ind=0`: The starting index from which the function will start checking for the minimum value.
    * `int size=720`: How may lasers the function will concider.

* *OUTPUTS*:
    *  `float val`: The minimum distance registered tin the assigned area.
    
```cpp
float mini ( float arr[720], int ind=0, int size=720){


    float val = 30.0;
    // Starting value equal to the maximum distance retrivable by the lasers.
    
    for(int i= ind; i<ind+size; i++){
    
        if (val>=arr[i]){
            val=arr[i];
        }
    }
    // Itreturns the minimum value
    return val;
}
```

The `robotCallBack` function implements a cycle that will determine what velocity to publish on the `cmd_vel`  topic.

As a "default state", the robot will move around the circuit at a costant velocity of 1.0 towards its relative X-Axis resulting in a motion. 
Once the middle detecting area will register a distance shorter than 1.5 units, the robot will stop driving forward and it will start checking on the other areas. These detecting zones are located at the sides of the robot. Thanks to an if-statemnt, the code will check if the shortest distance is detected on the right side or on the left side of the robot. Depending on this decision, the robot will decide to either turn left or right to get away of the wall. As a "turning state", the robot will spin around the Z-Axis with an angular velocity of 1.0 and a linear velocity of 0.1. To be sure to keep a distance from the walls, once the side detecting areas detect one, the code will set a linear velocity along the Y-Axis towads the opposite directions of the closest bareer. 
At the end of the cycle, the velocity will be published in the `cmd_vel` and it will be read by the symulation which will move the robot correctly.

`robotCallBack`:

```cpp

void robotCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
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
             if (mini(laser, 620,50)<0.2){
                 my_vel.linear.y= -1.5;
             }
             else {
             my_vel.linear.y= 0.0;
             }
         }
         
        pub.publish(my_vel);
}
```

`accellerator` is a *CallBack* that subscribes to the `Acc` Topic to retrive the accelleration factor set by the UI inteface. The *CallBack* will associate the new coefficent to the global  variable `acc` which will set the new speed at every cycle. 
* The following code shows the *callBack* `accelerator` of the node:

```cpp
void accelerator(const second_assignment::Acc::ConstPtr& msg1)
{
    acc=msg1->a;
    std::cout<<msg1->a<<"\n";
}
```
* Usage:
```bash
rosrun second_assignment robot_controller_node
```
* The following *FlowChart* shows grafically the implemented logic:

<p align="center">

<img height="1000" src="https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/flow2.png">

</p>

------
## Server Node: input_node

This node is used to manage the acceleration factor of the robot. The Server node exploits the *custom server* `second_assignment/Accelerate` to recive a **request** and produce a **response** in the UI node. 
The `input_node` implements the service that desides either to encrese or decrese the speed of the robot or to reset the loaction of the robot to the starting position.
The service is advertized inside of the *bool callBack*  `char_input`. The **request** associated to a `char` value will either:

* Send a **response** to *increase* the acceleration factor by 0.5 units if the `char` value is equal to `a`,
* Send a **response** to *decrease* the acceleration factor by 0.5 units if the `char` value is equal to `s`,
* Call the service `/reset_positions (std_srvs/Empty)` to RESET the position of the robot  if the `char` value is equal to `r`,
* Give an ERROR message if any other  `char` value is entered.

Once either the  `a` or the  `s` key are pressed, the gloabal variable  `acc` will either encrese or decrese by half a unit. This variable will later be assigned to the **response** of the service. 

* The following code shows the *bool callBack* `char_input` of the node:

```cpp

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
```

* Usage:
```bash
rosrun second_assignment input_node
```


------
## UI Node: UI_node

The UI_node is the interface through which the user is able to interact with the simulation.
The *CallBack* function will ask the user to enter a char variable to INCREMENT or DECREMENT the robot velocity or RESET its position. 
The `std::cin>>` will associate the `char` input to a `char` variable that will later be passed as a **request** of the *ros::ServiceClient client1*. The client will send the request to a service of type `/Accelerate`. The service will associate the read `char` to the increase or the decrease of the speed variable. The node will later associate the **response** of the service to the `float` variable of the `my_acc.a (second_assignment/Acc)` variable of the custom message `Acc`. At last, the `float` varable will be **published** to the Topic to pass athe information about the acceleration fator iside the controller. 

* The node **publishes** to the `Acc (second_assignment/Acc)` topic on which the node will tell publish the encrease of *linear* velocity of the robot.
* The node **subscribes**  to the `base_scan (sensor_msgs/LaserScan)` topic  which will provide the *CallBack* with capability of endlessly spin.

* The following code shows  the robotCallBack:

```cpp

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
```

* Usage:
```bash
rosrun second_assignment UI_node
```


Project Flowchart
--------------------

<p align="center">

<img height="500" src="https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/flow.png">

</p>


Summary of the Flowchart and 
reasoning behind the structure of the project
--------------------
The flowchart is composed by:

* **Green** and **yallow** boxs that rappresents the *Ros nodes* required to run the simulation,
* **Red** ovals rappresenting the *topics* needed by the nodes to exchange data and
* **Light blue** ovals to show all the requests and responses of *services*. 

The whole system is controlled by the **Controller Node**. This program deals with the navigation takeing care of the following tasks:

* It recives a costant flow of information about the  robot's surroundings thanks to the subscription to the */Base_scan* topic.
* This Data is managed by the *robotCallBack* to publish the proper robot's velocity parameters on the */Cmd_vel* topic. 
* Thanks to the */Acc* topic's subscription, the node will also take care of the change of the robot's speed choose by the user in the *UI node*.

The **UI Node** has the role of takeing KeyBoard inputs from the user and sending their responses to the system. The program comunicates the the other nodes as it follows: 

* It recives inforation from the user's input with a  `std::cin>>`.
* The Input will be sent as a riquest to the *Server Node* that will turn the given input into a resposne regarding the increase/decrease speed factor.
* This value will be sent to the controller node that will add it to the current linear velocity.

The **Input Node** works a a *service* for the requests sent by the *UI Node* for managing the robot's speed and the RESET condition of the system. Depending on what kind of input the node will recive, the service could either:

* INCREMENT the robot's speed if the given input is the `a` char or, 
* DECREMENT the robot's speed if the given input is the `s` char or, 
* RESET the robot's position through the use of the */reset_position* if the given input is the `r` char.

For any other input the node will just return an error message.

