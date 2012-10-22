/*!
* @file 	main.cpp
* @author 	Michael Blösch
* @date		10.10.2012
* @brief	Simple ros-node publishing the tabulator separated data from a file
 */

#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "sensor_msgs/JointState.h"
#include "geometry_msgs/PoseStamped.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main (int argc, char **argv)
{
	// Ros initialization
	ros::init(argc, argv, "LSE_test");
	ros::NodeHandle nh;
	ros::Rate loop_rate(1000);

	// Preparing stuff for file handling
	std::ifstream infile;
	infile.open ("data/measdata.dat", ifstream::in);
	std::string line;
	std::istringstream tokens;
	std::string token;
	double time = 0;
	double startTime = 0;
	bool gotLine = std::getline(infile,line);
	if(gotLine){
		tokens.clear();
		tokens.str(line);
		std::getline(tokens, token, '\t');
		startTime = std::atof(token.c_str());
	}

	// Preparing stuff for IMU measurements
	ros::Publisher imuPub = nh.advertise<sensor_msgs::Imu>("/MeasLoader/imuMeas", 1000);
	sensor_msgs::Imu imuMsg;
	imuMsg.orientation.x = 0;
	imuMsg.orientation.y = 0;
	imuMsg.orientation.z = 0;
	imuMsg.orientation.w = 1;
	for(int i=0;i<9;i++){
		imuMsg.orientation_covariance[i] = 0;
		imuMsg.angular_velocity_covariance[i] = 0;
		imuMsg.linear_acceleration_covariance[i] = 0;
	}

	// Preparing stuff for encoder measurements
	ros::Publisher encPub = nh.advertise<sensor_msgs::JointState>("/MeasLoader/encMeas", 1000);
	sensor_msgs::JointState encMsg;
	std::vector<std::string> encMsg_name(12,"");
	std::vector<double> encMsg_pos(12,0);
	std::vector<double> encMsg_cf(12,0);
	encMsg_name[0] = "LF_HAA";
	encMsg_name[1] = "LF_HFE";
	encMsg_name[2] = "LF_KFE";
	encMsg_name[3] = "RF_HAA";
	encMsg_name[4] = "RF_HFE";
	encMsg_name[5] = "RF_KFE";
	encMsg_name[6] = "LH_HAA";
	encMsg_name[7] = "LH_HFE";
	encMsg_name[8] = "LH_KFE";
	encMsg_name[9] = "RH_HAA";
	encMsg_name[10] = "RH_HFE";
	encMsg_name[11] = "RH_KFE";
	encMsg.name = encMsg_name;

	// Preparing stuff for pose sensor measurements
	ros::Publisher posPub = nh.advertise<geometry_msgs::PoseStamped>("/MeasLoader/posMeas", 1000);
	geometry_msgs::PoseStamped posMsg;

	// Start main loop, continue until end of file is reached
	ros::Time begin = ros::Time::now();
	while (ros::ok() && gotLine){
		// Rough synchronisation of publishing time and measurement timestamp (spins at 1000Hz)
		while((ros::Time::now() - begin).toSec() > time && gotLine){
			// Set headers
			imuMsg.header.seq = 0;
			imuMsg.header.stamp = begin+ros::Duration(time);
			imuMsg.header.frame_id = "/I";
			encMsg.header.seq = 0;
			encMsg.header.stamp = begin+ros::Duration(time);
			encMsg.header.frame_id = "/B";
			posMsg.header.seq = 0;
			posMsg.header.stamp = begin+ros::Duration(time);
			posMsg.header.frame_id = "/C";

			// Read Acceleration
			std::getline(tokens, token, '\t');
			imuMsg.linear_acceleration.x = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			imuMsg.linear_acceleration.y = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			imuMsg.linear_acceleration.z = std::atof(token.c_str());

			// Read Rotational Rate
			std::getline(tokens, token, '\t');
			imuMsg.angular_velocity.x = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			imuMsg.angular_velocity.y = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			imuMsg.angular_velocity.z = std::atof(token.c_str());

			// Read encoders
			for(int i=0;i<12;i++){
					std::getline(tokens, token, '\t');
					encMsg_pos[i] = std::atof(token.c_str());
			}
			encMsg.position = encMsg_pos;

			// Read contacts
			for(int i=0;i<4;i++){
				std::getline(tokens, token, '\t');
				encMsg_cf[3*i] = std::atof(token.c_str());
				encMsg_cf[3*i+1] = std::atof(token.c_str());
				encMsg_cf[3*i+2] = std::atof(token.c_str());
			}
			encMsg.effort = encMsg_cf;

			// Read pose sensor
			std::getline(tokens, token, '\t');
			posMsg.pose.position.x = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			posMsg.pose.position.y = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			posMsg.pose.position.z = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			posMsg.pose.orientation.x = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			posMsg.pose.orientation.y = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			posMsg.pose.orientation.z = std::atof(token.c_str());
			std::getline(tokens, token, '\t');
			posMsg.pose.orientation.w = std::atof(token.c_str());

			// Publish data
			imuPub.publish(imuMsg);
			encPub.publish(encMsg);
			posPub.publish(posMsg);

			// Get next line of data-file
			gotLine = std::getline(infile,line);
			if(gotLine){
				tokens.clear();
				tokens.str(line);
				std::getline(tokens, token, '\t');
				time = std::atof(token.c_str())-startTime;
			}
		}

		loop_rate.sleep();
	}

	infile.close();
	cout << "Finished" << endl;
	return 0;
}
