#include <cassert>
#include <iostream>
#include <string_view>

#include "EventBus.h"

template<typename T>
constexpr std::string_view typeName(const T& t)
{
    return typeid(t).name();
}

// All events must inherit from class called "Event"
struct Event{};

struct EventA : public Event
{
    int foo = 111;
    int bar = 22;
};

struct EventB : public Event
{
    int foo = 3;
};

struct SystemA
{
    int foobar = 0;

    void handleEventA(EventA* e)
    {
        std::cout << typeName(*this) << " Handling: \"" << typeName(*e) << "\"\n";

        foobar += e->foo;
        foobar += e->bar;
    }
    
    void handleEventB(EventB* e)
    {
        std::cout << typeName(*this) << " Handling: \"" << typeName(*e) << "\"\n";

        foobar -= e->foo;
    }
    
};

struct SystemB
{
    int foobar = 0;

    void handleEventA(EventA* e)
    {
        std::cout << typeName(*this) << " Handling: \"" << typeName(*e) << "\"\n";

        foobar -= e->foo;
        foobar -= e->bar;
    }
    
    void handleEventB(EventB* e)
    {
        std::cout << typeName(*this) << " Handling: \"" << typeName(*e) << "\"\n";

        foobar += e->foo;
    }
    
};

int main()
{
    // create an event bus
    EventBus bus;

    // create systems which listens for events
    SystemA sA;
    SystemB sB;
    
    // set up which events  systems listens for
    bus.subscribe(&sA, &SystemA::handleEventA);
    bus.subscribe(&sA, &SystemA::handleEventB);
    bus.subscribe(&sB, &SystemB::handleEventA);
    bus.subscribe(&sB, &SystemB::handleEventB);
    
    // demo of  events taking place and  system handling them
    EventA eventA;
    bus.publish(&eventA);

    EventB eventB;
    bus.publish(&eventB);

    // assert system is handling events correctly
    const bool expectedOutputA{ sA.foobar == eventA.foo + eventA.bar - eventB.foo };
    const bool expectedOutputB{ sB.foobar == eventB.foo - eventA.foo - eventA.bar };
    assert(expectedOutputA && expectedOutputB);
}