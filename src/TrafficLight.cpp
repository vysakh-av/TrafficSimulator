#include "TrafficLight.h"
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] { return !_queue.empty(); });

    T result = std::move(_queue.back());
    _queue.pop_back();
    return result;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
    std::lock_guard<std::mutex> guard(_mutex);
    _queue.emplace_back(msg);
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

TrafficLight::~TrafficLight() {}
void TrafficLight::waitForGreen() {
    while (true) {
        _currentPhase = _qPhase.receive();
        if (_currentPhase == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
    std::lock_guard<std::mutex> guard(_mutex);
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
    std::random_device device;
    std::mt19937 randdev(device());
    std::uniform_int_distribution<std::mt19937::result_type> duration(4, 6);

    auto startTime = std::chrono::system_clock::now();
    while (true) {
        auto endTime = std::chrono::system_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::seconds>(endTime -
                                                                     startTime)
                        .count();

        if (duration(randdev) <= diff) {
            _currentPhase = static_cast<TrafficLightPhase>(
                (static_cast<int>(_currentPhase) + 1) % 2);
            _qPhase.send(std::move(_currentPhase));
            startTime = endTime;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
}
