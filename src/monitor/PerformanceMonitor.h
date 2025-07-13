//
// Created by qm210 on 11.07.2025.
//

#ifndef DLTROPHY_SIMULATOR_PERFORMANCEMONITOR_H
#define DLTROPHY_SIMULATOR_PERFORMANCEMONITOR_H

#include <string>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>

#include "Observables.h"

// QM: YET_UNUSED
class PerformanceMonitor {
    /*
     *  To find out, why the program seems to get slow over time, and then,
     *  after resizing, fast again, I want to log some metrics async to a file
     */

    std::queue<std::string> queue;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> alive{true};

    std::thread writer;
    std::ofstream outfile;
    std::chrono::milliseconds logInterval;

    Observables observables;
    std::mutex obsMtx;

public:

    PerformanceMonitor(const std::string& filename, long logIntervalMs = 2100):
        outfile(filename),
        logInterval(std::chrono::milliseconds(logIntervalMs))
        {
            writer = std::thread([this] {
                std::vector<Observable> obsCopy;
                while(alive) {
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait_for(lock, logInterval, [this] {
                            return !queue.empty() || !alive;
                        });
                        while (!queue.empty()) {
                            outfile << queue.front() << std::endl;
                            queue.pop();
                        }
                    }
                    {
                        std::lock_guard<std::mutex> lock(obsMtx);
                        obsCopy = observables.copy();
                    }
                    for (const auto& observable : obsCopy) {
                        outfile << observable() << std::endl;
                    }
                    outfile.flush();
                }
            });
        }

    ~PerformanceMonitor() {
        alive = false;
        cv.notify_one();
        if (writer.joinable()) {
            writer.join();
        }
        while (!queue.empty()) {
            outfile << queue.front() << std::endl;
            queue.pop();
        }
        outfile.flush();
        outfile.close();
    }

    void log(const std::string& entry) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(entry);
        }
        cv.notify_one();
    }

    template<typename O>
    void observe(const O& obs) {
        std::lock_guard<std::mutex> lock(obsMtx);
        // this should handle it via SFINAE
        observables.add(std::move(obs));
    }

};

#endif //DLTROPHY_SIMULATOR_PERFORMANCEMONITOR_H
