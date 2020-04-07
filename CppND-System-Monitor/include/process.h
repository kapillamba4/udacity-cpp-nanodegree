#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid, std::string user, std::string command, float cpuUtilization,
          long int upTime, std::string ram);
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

 private:
  int mPid;
  std::string mUser;
  std::string mCommand;
  float mCpuUtilization;
  long int mUpTime;
  std::string mRam;
};

#endif