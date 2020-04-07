#include <dirent.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> kernel;
  }
  return kernel;
}

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

float LinuxParser::MemoryUtilization() {
  int totalMemory = 0, freeMemory = 0;
  string line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    string key, value, unit;
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal") {
          totalMemory = stoi(value);
        } else if (key == "MemFree") {
          freeMemory = stoi(value);
        }
      }
    }
  }

  return float(totalMemory - freeMemory) / totalMemory;
}

long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string line, uptime;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  return stol(uptime.substr(0, uptime.size() - 3));
}

long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpuUtilization = CpuUtilization();
  return (std::stol(cpuUtilization[CPUStates::kIdle_]) +
          std::stol(cpuUtilization[CPUStates::kIOwait_]));
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuUtilization;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  string line, cpu, cpu_time;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;

    while (linestream >> cpu_time) {
      cpuUtilization.push_back(cpu_time);
    }
  }

  return cpuUtilization;
}

// Ref:
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float LinuxParser::CpuUtilization(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    vector<string> fields;
    string s;
    while (linestream >> s) {
      fields.push_back(s);
    }
    long utime = std::stol(fields[13]);
    long stime = std::stol(fields[14]);
    long ctime = std::stol(fields[15]);
    long cstime = std::stol(fields[16]);
    long startTime = std::stol(fields[21]);
    long sys_uptime = UpTime();
    long total_time = utime + stime + ctime + cstime;
    long seconds = sys_uptime - (startTime / sysconf(_SC_CLK_TCK));
    if (seconds == 0) return 0;
    return (float(total_time) / sysconf(_SC_CLK_TCK)) / float(seconds);
  }
  return 0.0;
}

int LinuxParser::TotalProcesses() {
  string key, value;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") return stoi(value);
      }
    }
  }

  return 0;
}

int LinuxParser::RunningProcesses() {
  string key, value;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") return stoi(value);
      }
    }
  }

  return 0;
}

string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }

  return line;
}

string LinuxParser::Ram(int pid) {
  string line, key;
  long value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return std::to_string(value / 1000);
        }
      }
    }
  }

  return string("0");
}

string LinuxParser::Uid(int pid) {
  string line, key;
  long value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return std::to_string(value);
        }
      }
    }
  }

  return string();
}

string LinuxParser::User(int pid) {
  string line;
  string user, uid_lhs;
  string uid_rhs{LinuxParser::Uid(pid)};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> uid_lhs >> uid_lhs) {
        if (uid_lhs == uid_rhs) {
          return user;
        }
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  string line, val;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> val;
    }

    // uptime in seconds
    return stol(val) / sysconf(_SC_CLK_TCK);
  }

  return 0;
}