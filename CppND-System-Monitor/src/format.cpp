#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds [[maybe_unused]]) {
  string HH, MM, SS;
  long hour, minutes, secs;
  secs = seconds % 60;
  minutes = (seconds / 60) % 60;
  hour = (seconds / 3600);

  HH = std::to_string(hour);
  MM = std::to_string(minutes);
  if (MM.size() == 1) {
    MM = "0" + MM;
  }
  SS = std::to_string(secs);
  if (SS.size() == 1) {
    SS = "0" + SS;
  }
  return HH + ':' + MM + ':' + SS;
}