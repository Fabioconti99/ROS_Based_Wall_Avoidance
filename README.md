 #  [Research_Track_1](https://unige.it/en/off.f/2021/ins/51201.html?codcla=10635) , [Robotics Engineering](https://courses.unige.it/10635) ([UNIGE](https://unige.it/it/)) : Second assignement
## Robot Operating System <img height="30" src="https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/ros.png"> & C++ <img height="30" src="https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/cpl.png">
### Professor. [Carmine Recchiuto](https://github.com/CarmineD8)


--------------------
Project objectives
--------------------
This project aimed to create a ROS Package capable of making a cubical robot equipped with laser scanners, a lap around a given circuit. The package contains the C++ sources needed for the interaction with the robot. The environment is a simple 3D model of the actual Monza's F1 circuit.
ROS (Robot Operating System) is a set of tools and libraries for autonomous, robust, efficient, and secure mobile robot navigation, which uses sensors to sense the environment and computer vision techniques as the navigation method.

The behavior of the robot has to stand by the following rules:
* Constantly driving around the environment in the clockwise direction,
* Avoiding to hit the walls, 
* Being able to either **accelerate** or **decelerate** if the user will ask to do so,
* And **Resetting** its position to the starting one if asked to. 

The user will be able to interact with the simulation through the use of **KeyBoard inputs**:
* Pressing the `r` key will RESET the initial position of the robot.
* Pressing the `s` key will SLOW DOWN the robot's speed by **half unit** at every tap of the button.
* Pressing the `a` key will INCREASE the robot's speed by **half unit** at every tap of the button.

Picture of the **Enviroment**:

![alt text](https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/map.png) 

With everything working correctly, the robot should lap around the circuit avoiding the walls at different velocities on an infinite loop. If the velocity
gets too high, it may lead to the robot crashing into the wall but that would be perfectly reasonable. 

--------

Installing and running
----------------------

The simulation requires a ROS Noetic installation, and the clone of the [Teacher's repository](https://github.com/CarmineD8/second_assignment). The repo contains the workspace for running the simulation node. The source includes the *CmakeList* for compiling the node and the *World* file for building the scene.
The package I developed contains:

* A *Controller node* which will command the movements and the acceleration of the robot.
* A *UI node* to let the user interact with the simulation with the KeyBoard Inputs. 
* A *Server node* that will process the KeyBoard Input and modify the acceleration factor accordingly to the pressed key. 
* ONE *custom service* needed by the server node to exchange data between the *request* and the *response*.
* ONE *custom message* needed to pass the acceleration factor from the service to the controller through the use of a topic.


## Running the simulation

* Please follow these steps to run the Circuit scene:

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

* You could also run the program with the launch command:
```bash

roslaunch second_assignment run.launch
```

the launch script lets the user start the whole project with just one command. The following code shows all the instructions sent by the launch program.
```xml

<launch>
  <node name="robot_controller_node" pkg="second_assignment" type="robot_controller_node" output="screen" required="true"/>
  <node name="input_node" pkg="second_assignment" type="input_node" output="screen" launch-prefix="xterm -fg white -bg black -e "  required="true"/>
  <node name="UI_node" pkg="second_assignment" type="UI_node" output="screen" launch-prefix="xterm -fg white -bg black -e "  required="true"/>
  <node name="stageros" pkg="stage_ros" type="stageros" required ="true" args = "$(find second_assignment)/world/my_world.world"/>
</launch>
```
This kind of execution needs the Xterm terminal to be installed. If the it's not already installed you can download it with the following shell command:

```bash

sudo apt-get install -y xterm
```

------
NODES Description
-------------------

## Simulation Node: StageRos

The node used for the simulation is the standard node `StageRos`. The stageros node wraps the Stage 2D multi-robot simulator, via libstage. Stage simulates a world as defined in a .world file. This file tells stage everything about the world, from obstacles (usually represented via a bitmap to be used as a kind of background), to robots and other objects.

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

This node controls the robot's driving capabilities inside the circuit. It will command the robot to drive at a certain velocity inside the track. Thanks to this node, the robot will also be able to scan the walls of the circuit and not hit them. 
The robot subscribes and publishes to the following topics:

* The node **publishes** to the `cmd_vel geometry_msgs/Twist` Topic to change the *linear* and *angular* velocity values related to the robot.

* The node **subscribes** to the following topics:

    * `base_scan (sensor_msgs/LaserScan)`:  which will provide the node with Data about the enviroment surrouding the robot.
    * `Acc (second_assignment/Acc)`:  Which will tell the node how much to increase the *linear* velocity of the robot on the `cmd_vel` Topic.
    
On the  `cmd_vel` Topic, the Simulation Node publishes all the information acquired by the laser scanner set. The robot is equipped with 721 lasers that retrieve information about the relative distance between the center of the robot and the walls of the circuit. The lasers reach a 30 units distance, and they cover a 180º degrees span in front of the robot. The scanning rate is 10 Hz.
The following picture gives a visual representation of the lasers inside the environment:

<p align="center">
 
<img width="456" alt="143497570-fc64e014-f41a-4a99-990c-1cc71a7cfe95-2" src="https://user-images.githubusercontent.com/91262561/145681009-eea49350-292f-4b75-9582-35a4ef295bad.png">


</p>

Thanks to the node's subscription to the `base_scan` topic, the *CallBack* function of the node will always know what the robot sees in front of it. 
To let the robot make its way around the circuit, I implemented a simple code that exploits the Data acquired by the laser scans.
The Data coming from the scanners is composed of two different arrays. One related to the **intensity** of the lasers the other one to the **scan ranges**. I used the second array to collect reliable information to manage the robot's navigation speed. 
I devided the area covered by the lasers in three main areas each one composed of 100 lasers. The first one covers form the index 0 to 100, the following one forms 310 to 410, and the last one from 620 to 720. Thanks to the function `mini(float array [720], int starting_index, int n_lasers)`, the detecting areas will return the minimum distance value registered by the lasers. 

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

As a "default state", the robot will move around the circuit at a constant velocity of 1.0 towards its relative X-Axis resulting in a motion. 
Once the middle detecting area scans a distance shorter than 1.5 units, the robot will stop driving forward and it'll check on the other detecting regions. These areas are on the left and right sides of the robot. Since the two regions retrieve the robot's distance to the wall, an if-statement can decide on what side the closest barrier is. Depending on this decision, the robot will either turn left or right to get away from the wall. As a "turning state", the robot will spin around the Z-Axis with an angular velocity that varies depending on the retrived accelleration and a linear velocity of 0.1. The accelleration factor for the angular velocity operates just like the accelleration factor of the linear velocity but scaled down of a `1/4 (=ang_multiplier)` factor. 
The following *gif* shows the robot approaching a tight turn: 

<p align="center">
 
![ezgif-6-ba821d419f16](https://user-images.githubusercontent.com/91262561/145681074-b2b2bc3e-fa93-412c-904d-4e3793999d0d.gif)
 
</p>

At the end of the cycle, the velocity will be published on the `cmd_vel` topic. The simulation node will read the new speed value and it'll make the robot move correctly.

`robotCallBack`:

```cpp

void robotCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
    // Shell print of the just one of the updated values retrived by the laserScan set.
    ROS_INFO("Central laser value [%f] ",msg->ranges[360]);
    
    
    
        // Assigning all the retrived values to a different array in order to exploit the collected data.
        float laser[721];
        for(int i=0;i<721;i++){
        laser[i]=msg->ranges[i];
        }
        
        // If the closest distance detected by the lasers'set in fornt of the robot is higher than 1.5 units the robot will
        // travel straight.
        if (mini(laser,310,100)>1.5){
        
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
            if (mini(laser,0,100)<mini(laser,620,100)){
    
                my_vel.linear.x = 0.1;
                my_vel.angular.z = 1+(acc*ang_multiplier); 
            }
            
        // If the closest wall detected is on the LEFT SIDE.
            else {
            
                my_vel.linear.x = 0.1;
                my_vel.angular.z = -1+(-acc*ang_multiplier); 
            }                
        }
         
        // Pubblishing the velocity values
        pub.publish(my_vel);


}
```

`accelerator` is a *CallBack* that subscribes to the `Acc` Topic to retrieve the acceleration factor set by the UI interface. The *CallBack* will associate the new coefficient to the global variable `acc` which will set the new speed at every cycle. 

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

<img height="800" src="https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/flow2.png">

</p>

------
## Server Node: input_node

This node manages the acceleration factor of the robot. The Server node exploits the *custom server* `second_assignment/Accelerate` to receive a **request** and produce a **response** in the UI node. 
The `input_node` implements the service that decides either to increase or decrease the robot's speed and to reset the robot's location to the starting position.
The service is advertised inside of the *bool function*  `char_input`. The **request** associated with a `char` value will either:

* Send a **response** to *increase* the acceleration factor by 0.5 units if the `char` value is equal to `a`,
* Send a **response** to *decrease* the acceleration factor by 0.5 units if the `char` value is equal to `s`,
* Call the service `/reset_positions (std_srvs/Empty)` to RESET the position of the robot  if the `char` value is equal to `r`,
* Give an ERROR message if any other  `char` value is entered.

Once either the  `a` or the  `s` key are pressed, the global variable  `acc` will either increase or decrease by half a unit. This variable will later be assigned to the **response** of the service. 

* The following code shows the *bool function* `char_input` of the node:

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

The UI_node is the interface through which the user can interact with the simulation.
The *Interaction* function will ask the user to enter a char variable to INCREMENT or DECREMENT the robot velocity or RESET its position. 
The `std::cin>>` will associate the `char` input to a `char` variable that will later be passed as a **request** of the *ros::ServiceClient client1*. The client will send the request to a service of type `/Accelerate`. The service will associate the read `char` to the increase or the decrease of the speed variable. The node will later associate the **response** of the service to the `float` variable of the `my_acc.a (second_assignment/Acc)` variable of the custom message `/acc`. At last, the `float` variable will be **published** to the Topic to pass the information about the acceleration factor inside the controller. 

* The node **publishes** to the `Acc (second_assignment/Acc)` topic on which the node will publish the increase of *linear* velocity of the robot.
* The node doesen't **subscribe**  to any topic but it just spins in a while loop inside the main function.

* The following code shows  the robotCallBack:

```cpp

void Interaction()
{
    
    // Custom message with which will send the updated data to the controller.
    second_assignment::Accelerate acc_srv;
        
    // Section due to retriving input from the user.
    char in;
    std::cout<<"input 'a' to accelerate, 's' to slow down or 'r' to RESET:\n";
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

* **Green** and **yallow** boxs that rappresent the *Ros nodes* required to run the simulation,
* **Red** ovals rappresenting the *topics* the nodes read to exchange data and
* **Light blue** ovals to show all the requests and responses of *services*. 

The whole system is controlled by the **Controller Node**. This program deals with the robot's navigation taking care of the following tasks:

* It receives a constant flow of information about the robot's surroundings thanks to the subscription to the */Base_scan* topic.
* This Data is managed by the *robotCallBack* to publish the proper robot's velocity parameters on the */Cmd_vel* topic. 
* Thanks to the */Acc* topic's subscription, the node will also take care of the change of the robot's speed chosen by the user in the *UI node*.

The **UI Node** has the role of taking KeyBoard inputs from the user and sending their responses to the system. The program communicates the other nodes as it follows: 

* It receives information from the user's input with a  `std::cin>>`.
* The Input will be sent as a request to the *Server Node* that will turn the given input into a response regarding the increase/decrease speed factor.
* This value will be sent to the controller node that will add it to the current linear velocity.

The **Input Node** works as a *service* for the requests sent by the *UI Node* for managing the robot's speed and the RESET condition of the system. Depending on what kind of input the node will receive, the service could either:

* INCREMENT the robot's speed if the given input is the `a` char or, 
* DECREMENT the robot's speed if the given input is the `s` char or, 
* RESET the robot's position through the use of the */reset_position* if the given input is the `r` char.

For any other input the node will just return an error message.

### Video of the robot's performance

The following video shows the performance of the robot during the first few laps around the circuit. The performance shows how the different input commands affect the robot's movements. 


https://user-images.githubusercontent.com/91262561/143498300-ba1dee74-d5ef-467e-820d-c8e629a0a385.mp4


Possible improvements
-----------------
During the development of the code, I came up with a few ideas that could lead to some improvement of the driving capabilities of the robot. I also implemented a simpler version of the package based on a smaller number of Ros nodes utilized. 

* With the implementation I came up with so far, once the robot gets close to a maze's border, it will stop the forward motion and will start checking the sides to see where the closest wall is. Thanks to the laser scans, the robot will know whether the closest wall to the robot is to the robot's right to or left. It will accordingly assign an angular rotation to turn away from the wall. Since the motor sensors return a highly updated data set about the robot's surroundings, the idea I had was to implement a code capable of making the robot always keep to its front the detection area with the longest distance detected. This different approach to the development of the robot's movement would theoretically keep the robot's orientation always towards the freest direction. However, the coding of this logic didn't lead to a feasible solution. The main issue related to this way of reasoning comes from the scanning capabilities of the laser scanners. During the approach of tight turns, the lasers will identify the longest distance towards a direction slightly covered by a wall. This detection will cause the robot to follow the laser scanner's direction and eventually hit the wall. Fixing this issue could lead to a nicer logic that could handle higher robots' velocities. 

* Another implementation I successfully built relies on a less modular structure of the nodes' communication scheme. Instead of executing a separate Ros node for implementing the *service*, I coded it inside the controller node. This project version simplifies the communication net between all the nodes (the user should only launch three nodes instead of four). Though, the modularity of this version of the project would be slightly penalized cause of the removal of one Ros Node. According to the objective of maintaining a more modular project, I decided to keep the 4-nodes version as the final one.


Conclusions
-----------------
To take care of all the project's requests, I choose to manage the code's structure with modular logic. Thanks to this approach, I was able to get to the end of the assignment with a schematic structure of the project concerning 4 Ros nodes overall.  Thanks to the well-chosen parameters given to the controlling node, the robot can autonomously drive around the circuit for many laps at different velocities. The whole implementation makes the robot capable of lapping flawlessly at the highest velocity pick of 11-12 units. 
This project was my first approach to the ROS1 workflow. Working on this assignment, I gained knowledge about the basic concepts of ROS such as managing *callBack* functions, implementing new *services* and *messages* and building a modular and organized *package* structure. 
