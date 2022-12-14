#include <map>
#include <typeindex>
#include <vector>

// All specific events should inhert from this generic Event class
struct Event {};

// Source:
// https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f
// ^ this guy is a genius
class EventBus final
{
private:
    //// Class definitions for HandlerFunctionBase and MemberFunctionHandler 
    //// are put inside of the EventBus class and declared private members 
    //// so nowhere outside of EventBus can they be created or accessed.
 
    // This is the interface for MemberFunctionHandler that each specialization will use
    class HandlerFunctionBase
    {
    private:
        // Implemented by MemberFunctionHandler
        virtual void call(Event* e) = 0;

    public:
        // to prevent the following compiler warning:
        // "warning: delete called on 'EventBus::HandlerFunctionBase' that is 
        // abstract but has non-virtual destructor"
        virtual ~HandlerFunctionBase() = default;

        // Call the member function
        void exec(Event* e)
        {
            call(e);
        }

    };

    template<class T, class EventT>
    class MemberFunctionHandler final : public HandlerFunctionBase
    {
        // A function pointer to a method of type T that returns void and takes in an EventT type ptr
        typedef void (T::* MemberFunction)(EventT*);

    private:
        T* instance;        // Pointer to instance with method we want to call
        MemberFunction m;   // Pointer to member function of GameSystem that we want to call

    public:
        MemberFunctionHandler(T* instance, MemberFunction memberFunction)
            :
            instance{ instance },
            m{ memberFunction }
        {};

        void call(Event* e)
        {
            // Cast event to the correct type and call member function
            (instance->*m)(static_cast<EventT*>(e));
        }

    };

// EventBus body start:
private:
    std::map<std::type_index, std::vector<HandlerFunctionBase*>> subscribers;

public:
    ~EventBus()
    {
        for (auto& [id, handlers] : subscribers)
            for (auto& handler : handlers)
                delete handler;
    }
    
    template<typename EventT>
    void publish(EventT* e)
    {
        const std::type_index eventID{ typeid(EventT) };

        // if we have handlers for a type of event
        if (subscribers.find(eventID) != subscribers.end())
        {
            // get the vector containing all the handlers for that event
            auto& handlers{ subscribers.at(eventID) };

            // and execute them
            for (auto& funcHandler : handlers)
                funcHandler->exec(e);
        }
    }

    template<class T, class EventT>
    void subscribe(T* instance, void (T::* memberFunction)(EventT*))
    {
        const std::type_index eventID{ typeid(EventT) };

        // if no handlers exist for an event, create a new vector for the handlers of that event
        if (subscribers.find(eventID) == subscribers.end())
            subscribers.insert({ eventID, std::vector<HandlerFunctionBase*>() });

        auto funcHandler{ new MemberFunctionHandler<T, EventT>(instance, memberFunction) };

        subscribers.at(eventID).push_back(funcHandler);
    }

};