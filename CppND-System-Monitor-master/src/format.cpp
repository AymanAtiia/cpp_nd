#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) 
{ 
	float hours = seconds/(60.0 *60.0);
	float minutes = (hours-(int)hours)*60.0;
	float rem_seconds = (minutes-(int)minutes)*60.0;
	string result = std::to_string((int)hours) + ":" + std::to_string((int)minutes) + ":" + std::to_string((int)rem_seconds);
	return result;
}
