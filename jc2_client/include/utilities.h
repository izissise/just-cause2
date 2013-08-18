#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#define DLLEXPORT __declspec (dllexport)

#define UNUSED __pragma(warning(suppress:4100))

#define NAKED void _declspec(naked) //For hooking functions !

#define ASM _asm

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
