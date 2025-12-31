#ifndef COUNTER_TOP_HPP
#define COUNTER_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"
#include "Counter.hpp"

using namespace cadmium;

class Counter_Top : public Coupled {
    public:
    /**
     * Constructor function for the Counter_Top model.
     * @param id ID of the Counter_Top model.
     */
    Counter_Top(const std::string& id) : Coupled(id) {
        auto counter = addComponent<Counter>("counter");

        auto direction_input = addComponent<lib::IEStream<bool>>("direction_input","../main/input_data/direction_in.txt");
        auto increment_input = addComponent<lib::IEStream<int>>("increment_input","../main/input_data/increment_in.txt");
    
        addCoupling(direction_input->out, counter->direction_in);
        addCoupling(increment_input->out, counter->increment_in);
    }
};

#endif //COUNTER_TOP_HPP