#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Codi {

struct ProfileResult {
    std::string name;
    long long start;
    long long end;
    uint32_t threadID;
};

struct InstrumentationSession {
    std::string name;
};

class Instrumentor {
private:
    InstrumentationSession* _currentSession;
    std::ofstream _outputStream;
    int _profileCount;

public:
    Instrumentor() : _currentSession(nullptr), _profileCount(0) {}

    void beginSession(const std::string& name, const std::string& filepath = "results.json") {
        _outputStream.open(filepath);
        writeHeader();
        _currentSession = new InstrumentationSession{ name };
    }

    void endSession() {
        writeFooter();
        _outputStream.close();
        delete _currentSession;
        _currentSession = nullptr;
        _profileCount = 0;
    }

    void writeProfile(const ProfileResult& result) {
        if (_profileCount++ > 0)
            _outputStream << ",";

        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');

        _outputStream << "{";
        _outputStream << "\"cat\":\"function\",";
        _outputStream << "\"dur\":" << (result.end - result.start) << ',';
        _outputStream << "\"name\":\"" << name << "\",";
        _outputStream << "\"ph\":\"X\",";
        _outputStream << "\"pid\":0,";
        _outputStream << "\"tid\":" << result.threadID << ",";
        _outputStream << "\"ts\":" << result.start;
        _outputStream << "}";

        _outputStream.flush();
    }

    void writeHeader() {
        _outputStream << "{\"otherData\": {},\"traceEvents\":[";
        _outputStream.flush();
    }

    void writeFooter() {
        _outputStream << "]}";
        _outputStream.flush();
    }

    static Instrumentor& Get() {
        static Instrumentor instance;
        return instance;
    }
};

class IntrumentorTimer {
public:
    IntrumentorTimer(const char* name)
        : _name(name), _stopped(false) {
        _startTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~IntrumentorTimer() {
        if (!_stopped)
            Stop();
    }

    void Stop() {
        std::chrono::time_point<std::chrono::high_resolution_clock> endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(_startTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::Get().writeProfile({ _name, start, end, threadID });
        
        _stopped = true;
    }

private:
    const char* _name;
    std::chrono::time_point<std::chrono::high_resolution_clock> _startTimepoint;
    bool _stopped;
};

} // namespace Codi

#define CODI_PROFILE 1
#if CODI_PROFILE
    #define CODI_PROFILE_BEGIN_SESSION(name, filepath) ::Codi::Instrumentor::Get().beginSession(name, filepath)
    #define CODI_PROFILE_END_SESSION() ::Codi::Instrumentor::Get().endSession()
    #define CODI_PROFILE_SCOPE(name) ::Codi::IntrumentorTimer timer##__LINE__(name)
    #define CODI_PROFILE_FUNCTION() CODI_PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
    #define CODI_PROFILE_BEGIN_SESSION(name, filepath)
    #define CODI_PROFILE_END_SESSION()
    #define CODI_PROFILE_SCOPE(name)
    #define CODI_PROFILE_FUNCTION()
#endif
