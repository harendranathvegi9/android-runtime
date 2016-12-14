#ifndef STACKITY_H
#define STACKITY_H

#include <vector>
#include <chrono>
#include <string>
#include <stack>
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
        };

        static v8::Isolate *s_isolate;
        static int s_frames;
        static int s_frame_id;
        static std::chrono::time_point<std::chrono::steady_clock> s_benchmarkStart;
    };
}

#endif //STACKITY_H
