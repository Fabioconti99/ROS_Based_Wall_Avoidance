 #  [Research_Track_1](https://unige.it/en/off.f/2021/ins/51201.html?codcla=10635) , [Robotics Engineering](https://courses.unige.it/10635) ([UNIGE](https://unige.it/it/)) : Second assignement
## Robot Operating System <img height="30" src="https://github.com/Fabioconti99/RT1_Assignment_2/blob/main/images/python.png">
### Professor. [Carmine Recchiuto](https://github.com/CarmineD8)


--------------------
Project objectives
--------------------
The aim of this project was to create a ROS Pakage capable of make a cubical robot equiped with laser scanners, lap around a given circuit. The package contains the C++ surces needed for the interaction with the robot. The given enviroment is a simple 3D model of the actual Monza's F1 circuit.

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

![alt text](https://github.com/Fabioconti99/RT1_Assignment_1/blob/main/images/arena.png) 

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
Simulation Node: StageRos
-------------------
The node used for the simulation is the standard node `StageRos`. The stageros node wraps the Stage 2-D multi-robot simulator, via libstage. Stage simulates a world as defined in a .world file. This file tells stage everything about the world, from obstacles (usually represented via a bitmap to be used as a kind of background), to robots and other objects.

This node only exposes a subset of Stage's functionality via ROS. Specifically, it finds the Stage models of type laser, camera and position, and maps these models to the ROS topics given below. If at least one laser/camera and position model are not found, stageros exits.

* Usage:
```
rosrun stage_ros stageros [-g runs headless] <world> [standard ROS args]
```
* Parameters:
    * world : The Stage .world file to load.
    * -g : If set, this option will run stage as "headless," no GUI will be displayed.
    
* Stageros only exposes models created by a subset of the `.world` file syntax, specifically `laser`, `position` and `camera` models.

* The node **subscribes** to the `cmd_vel geometry_msgs/Twist` Topic to manage the velocity commands to drive the position model of the robot.

* The node **publishes** to the following topics:

    * `odom (nav_msgs/Odometry)`: it pubs odometry data from the position model.
    * `base_scan (sensor_msgs/LaserScan)`:  it pubs scans from the laser model
    * `base_pose_ground_truth (nav_msgs/Odometry)`:  it pubs the ground truth pose.
    * `image (sensor_msgs/Image)`: it pubs the visual camera image.
    * `depth (sensor_msgs/Image)`: it pubs depth camera image.
    * `camera_info (sensor_msgs/CameraInfo)`:  it pubs camera calibration info.

------
Controller Node: robot_controller_node
------------------
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



* Usage:
```
rosrun second_assignment robot_controller_node
```


