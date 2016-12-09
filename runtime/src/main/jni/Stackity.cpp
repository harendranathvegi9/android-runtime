#include "Stackity.h"

using namespace tns;

int Stackity::s_frames;
std::chrono::time_point<std::chrono::steady_clock> Stackity::s_benchmarkStart = std::chrono::steady_clock::now();
