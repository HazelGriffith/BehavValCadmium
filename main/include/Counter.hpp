#ifndef COUNTER_HPP
#define COUNTER_HPP

#include <random>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct counter_modelState {
    double sigma;
    int count;
    int increment;
    bool countUp;

    explicit counter_modelState(): sigma(1), count(0), increment(1), countUp(true){
    }
};

std::ostream& operator<<(std::ostream &out, const counter_modelState& state) {
    out  << "sigma:" << state.sigma << ";count:" << state.count << ";increment:" << state.increment << ";countUp:" << state.countUp;
    return out;
}

class Counter : public Atomic<counter_modelState> {
    public:

    Port<bool> direction_in;
    Port<int> increment_in;
    Port<int> count_out;

    Counter(const std::string id) : Atomic<counter_modelState>(id, counter_modelState()) {
        direction_in = addInPort<bool>("direction_in");
        increment_in = addInPort<int>("increment_in");
        count_out = addOutPort<int>("count_out");
    }

    // inernal transition
    void internalTransition(counter_modelState& state) const override {
        if (state.countUp){
            state.count += state.increment;
        } else {
            state.count -= state.increment;
        }
    }

    // external transition
    void externalTransition(counter_modelState& state, double e) const override {
        if (!direction_in->empty()){
            for (const auto x: direction_in->getBag()){
                state.countUp = x;
            }
        }
        if (!increment_in->empty()){
            for (const auto x: increment_in->getBag()){
                state.increment = x;
            }
        }
    }

    void confluentTransition(counter_modelState& state, double e) const override {
        externalTransition(state, e);
        internalTransition(state);
    }
    
    
    // output function
    void output(const counter_modelState& state) const override {
        count_out->addMessage(state.count);
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const counter_modelState& state) const override {     
            return state.sigma;
    }
};

#endif