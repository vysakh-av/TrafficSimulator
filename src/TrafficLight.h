#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

template <class T> class MessageQueue {
  public:
    T receive();
    void send(T &&msg);

  private:
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mutex;
};

enum TrafficLightPhase { red = 0x00, green };

class TrafficLight : public TrafficObject {
  public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    TrafficLight(const TrafficLight &src) = delete;           // Copy construcor
    TrafficLight operator=(const TrafficLight &src) = delete; // Assignement
    TrafficLight(TrafficLight &&src) = delete;                // Move ctor
    TrafficLight
    operator=(const TrafficLight &&src) = delete; // Move assignement

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

  private:
    // typical behaviour methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _qPhase;

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif
