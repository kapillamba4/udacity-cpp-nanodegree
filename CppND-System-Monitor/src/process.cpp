#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return mPid; }

float Process::CpuUtilization() { return mCpuUtilization; }

string Process::Command() { return mCommand; }

string Process::Ram() { return mRam; }

string Process::User() { return mUser; }

long int Process::UpTime() { return mUpTime; }

bool Process::operator<(Process const& p) const {
  return mCpuUtilization < p.mCpuUtilization;
}

Process::Process(int pid, std::string user, std::string command,
                 float cpuUtilization, long int upTime, std::string ram) {
  mPid = pid;
  mUser = std::move(user);
  mCommand = std::move(command);
  mCpuUtilization = cpuUtilization;
  mUpTime = upTime;
  mRam = std::move(ram);
}
