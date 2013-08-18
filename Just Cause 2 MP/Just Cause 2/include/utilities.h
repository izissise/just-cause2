#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include <string>
#include <sstream>

template<typename T>
std::string to_string(const T & Value)
{
  std::ostringstream oss;
  oss << Value;
  return oss.str();
}

#endif // UTILITIES_H_INCLUDED
