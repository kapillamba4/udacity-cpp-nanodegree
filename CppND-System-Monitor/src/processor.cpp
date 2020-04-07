#include "processor.h"
#include <string>
#include <vector>
#include "linux_parser.h"

// https://rosettacode.org/wiki/Linux_CPU_utilization
float Processor::Utilization() {
  std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();
  long totalTime = 0;
  for (const std::string &s : cpuUtilization) {
    totalTime += stol(s);
  }

  long idleTime = LinuxParser::IdleJiffies();
  return 1.0f - idleTime / float(totalTime);
}