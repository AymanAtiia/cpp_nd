#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
vector<float> LinuxParser::MemoryUtilization() 
{ 
	string line;
	string key, value;
	string name1 = "MemTotal:";
	string name2 = "MemFree:";
	string name3 = "Buffers:";
	
  vector <float> out;
	float totalMem = 0;
	float freeMem = 0;
	float buffers = 0;
	
	std::ifstream stream(kProcDirectory + kMeminfoFilename);
	
  	if (stream.is_open()) 
  	{	
  		for (int i=0; i<4; ++i){
	  		std::getline(stream, line);
	  		std::istringstream linestream(line);
	  		while (linestream >> key >> value) {
				if (key == name1) {
		  		totalMem = std::stof(value);
          out.push_back(totalMem);
		  	}
        else if (key == name2) {
          freeMem = std::stof(value);
          out.push_back(freeMem);
        }
        else if (key == name3) {
          buffers = std::stof(value);
          out.push_back(buffers);
        }
		  	}
		  }
         }

	return out;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
 
   long uptime;
   string line, uptimeStr;
   std::ifstream stream(kProcDirectory + kUptimeFilename);
   if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream uptimeStream(line);
    uptimeStream >> uptimeStr;
    }
    uptime = std::stol(uptimeStr);
  return uptime;}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 

  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {

  string line, value;
  vector<string> values;
  long totaltime = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }

  long utime = 0, stime = 0 , cutime = 0, cstime = 0;
  utime = stol(values[13]);
  stime = stol(values[14]);
  cutime = stol(values[15]);
  cstime = stol(values[16]);

  totaltime = utime + stime + cutime + cstime;
  return totaltime / sysconf(_SC_CLK_TCK);
  }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]); }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]); }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 

  string line, value;
  string key;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value) {
      values.push_back(value);
    };
  }
  return values; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 

  string line, key, value;
  int num_process = 0;
  string name = "processes";
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
      if (key == name) {
        num_process = std::stoi(value);
      }
    }
  }}
  return num_process; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key, value;
  int num_process = 0;
  string name = "procs_running";
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
      if (key == name) {
        num_process = std::stoi(value);
      }
    }
  }}
  return num_process; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {  

  string command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
  }
  return command;; }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {

  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "VmSize:") {
          linestream >> value;
          return to_string(stol(value) / 1024);
        }
      }
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 

  string line, key, value;
  string name = "Uid:";
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while(std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
      if (key == name) {
        return value;
      }
    }
  }}
  return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 

  string line, name;
  string result = LinuxParser::Uid(pid);

  std::ifstream stream(kPasswordPath);

  if(stream.is_open()){

    name = "x:" + result;
    while (std::getline(stream,line))
    {
      if (line.find(name) != string::npos)
      {
        result = line.substr(0, line.find(":"));
        return result;
      }
    } }

    return string();
  }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {

  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    };
  }
  return LinuxParser::UpTime() - (stol(values[21]) / 100);
}
