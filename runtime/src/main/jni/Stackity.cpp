#include "Stackity.h"

using namespace tns;

void Stackity::setIsolate(v8::Isolate *isolate) {
    Stackity::s_isolate = isolate;
}

v8::Isolate *Stackity::s_isolate;
int Stackity::s_frame_id;
int Stackity::s_frames;

std::queue<Stackity::FrameEntry::FrameEntryStartObject> Stackity::s_pendingFrameStartEntries;
std::queue<Stackity::FrameEntry::FrameEntryEndObject> Stackity::s_pendingFrameEndEntries;
std::chrono::time_point<std::chrono::steady_clock> Stackity::s_benchmarkStart = std::chrono::steady_clock::now();
