/*
    @name: StopWatch
    @brief: to set a stopwatch
    @author: ZCC
    @date: 2022/9/26
*/
#pragma once
#include <chrono>
#include <iostream>
#include <string>
using namespace std::chrono;

namespace ZCCTools {

class StopWatch {
   private:
    system_clock::time_point m_start;
    system_clock::time_point m_end;
    std::string Event;
    bool StartFlag = false;
    bool EndFlag = false;

   public:
    /*create the stopwatch*/

    StopWatch(std::string event_name) : Event(event_name) { ; }

    /*destroy the stopwatch*/
    ~StopWatch() = default;

    /*judge if the stopwatch is running*/
    bool IsStarted() const { return StartFlag; };

    /*Show the wated time, while keep running*/
    void Show() {
        if (StartFlag || !EndFlag) {
            std::cerr
                << "The stopwatch havn't been started or havn't been stoped"
                << std::endl;
        }
        system_clock::duration diff;
        diff = m_end - m_start;
        std::cout << "Event: " << Event << " ("
                  << (unsigned)(duration_cast<milliseconds>(diff).count())
                  << "ms)" << std::endl;
    }

    /*start the stopwatch*/
    void Start() {
        StartFlag = true;
        m_start = system_clock::now();
    }

    /*Stop the watch*/
    void Stop() {
        if (!StartFlag)
            std::cerr << "the stopwatch havn't been started" << std::endl;
        StartFlag = false;
        EndFlag = true;
        m_end = system_clock::now();
    }
};

}  // namespace ZCCTools
