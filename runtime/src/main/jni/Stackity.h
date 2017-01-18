#ifndef STACKITY_H
#define STACKITY_H

#include <vector>
#include <chrono>
#include <string>
#include <stack>
#include <queue>
#include "Tracer.h"
#include "v8.h"
#include "ArgConverter.h"
#include "CallbackHandlers.h"

namespace tns {
    /**
     * The class sends events
     */
    class Stackity {
    public:
        static void setIsolate(v8::Isolate *s_isolate);

        struct FrameEntry {
            FrameEntry(std::string description, std::string message = std::string("")) :
                    m_start(std::chrono::steady_clock::now()),
                    m_entries(s_frames),
                    m_frame_id(++s_frame_id),
                    m_description(description),
                    m_message(message) {

                if (Tracer::isEnabled()) { // if benchmarking is enabled
                    timeval curTime;
                    auto timeFromStart = std::chrono::duration_cast<std::chrono::microseconds>(
                            m_start - s_benchmarkStart).count();

                    // Call to a JavaScript callback with { id: 1, start: "ms since start", depth: 0, description: "require", message: "more elaborate description" }
                    CallbackHandlers::FrameEntryStartCallback(s_isolate, m_frame_id, m_entries, timeFromStart, m_description, m_message);

                    ++s_frames;
                }
            }

            ~FrameEntry() {
                if (Tracer::isEnabled()) { // if benchmarking is enabled
                    --s_frames;
                    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();

                    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
                            end - m_start).count();

                    auto difference = (float) microseconds / 1000;

                    CallbackHandlers::FrameEntryEndCallback(s_isolate, m_frame_id, m_entries, difference);
                }
            }

            int m_entries;
            int m_frame_id;
            std::string m_description;
            std::string m_message;
            std::chrono::time_point<std::chrono::steady_clock> m_start;

            struct FrameEntryStartObject {
                FrameEntryStartObject(int id, int depth, long timeFromStart, std::string descriptor, std::string description) :
                        m_id(id),
                        m_depth(depth),
                        m_time(timeFromStart),
                        m_descriptor(descriptor),
                        m_description(description) {
                }

            public:
                int m_id;
                int m_depth;
                long m_time;
                std::string m_descriptor;
                std::string m_description;
            };

            struct FrameEntryEndObject {
                FrameEntryEndObject(int id, int depth, float duration) :
                        m_id(id),
                        m_depth(depth),
                        m_duration(duration) {
                }

            public:
                int m_id;
                int m_depth;
                float m_duration;;
            };
        };

        static v8::Isolate *s_isolate;
        static int s_frames;
        static int s_frame_id;
        static std::chrono::time_point<std::chrono::steady_clock> s_benchmarkStart;
        static std::queue<FrameEntry::FrameEntryStartObject> s_pendingFrameStartEntries;
        static std::queue<FrameEntry::FrameEntryEndObject> s_pendingFrameEndEntries;
    };
}

#endif //STACKITY_H
