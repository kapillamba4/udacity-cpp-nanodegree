#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  vector<int> pids{LinuxParser::Pids()};
  processes_.clear();
  for (int pid : pids) {
    processes_.emplace_back(
        Process(pid, LinuxParser::User(pid), LinuxParser::Command(pid),
                LinuxParser::CpuUtilization(pid), LinuxParser::UpTime(pid),
                LinuxParser::Ram(pid)));
  }

  sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }