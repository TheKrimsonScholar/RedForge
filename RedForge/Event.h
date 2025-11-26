#pragma once

#include <functional>
#include <unordered_set>

template<typename... Args>
class Event;
template<typename... Args>
struct EventCallback
{
private:
    void* obj;
    void* memberFunction;
    std::function<void(Args...)> lambda;

public:
    // Constructor for free functions
    EventCallback(void(*function)(Args...))
    {
        this->obj = nullptr;
        this->memberFunction = *reinterpret_cast<void**>(&function);
        this->lambda = [function](Args... args) { (*function)(args...); };
    };
    // Constructor for non-const member functions
    template<typename ClassType>
    EventCallback(ClassType* obj, void(ClassType::* memberFunction)(Args...))
    {
        this->obj = static_cast<void*>(obj);
        this->memberFunction = *reinterpret_cast<void**>(&memberFunction);
        this->lambda = [obj, memberFunction](Args... args) { (obj->*memberFunction)(args...); };
    };
    // Constructor for const member functions
    template<typename ClassType>
    EventCallback(const ClassType* obj, void(ClassType::* memberFunction)(Args...) const)
    {
        this->obj = const_cast<void*>(static_cast<const void*>(obj));
        this->memberFunction = *reinterpret_cast<void**>(&memberFunction);
        this->lambda = [obj, memberFunction](Args... args) { (obj->*memberFunction)(args...); };
    };

    bool operator==(const EventCallback& other) const
    {
        return this->obj == other.obj && this->memberFunction == other.memberFunction;
    }

    friend struct std::hash<EventCallback<Args...>>;

    friend class Event<Args...>;
};
namespace std
{
    template<typename... Args>
    struct hash<EventCallback<Args...>>
    {
        size_t operator()(const EventCallback<Args...>& callback) const
        {
            size_t obj_hash = hash<void*>()(callback.obj);
            size_t func_hash = hash<void*>()(callback.memberFunction);

            // Combine the hashes
            return obj_hash ^ (func_hash + 0x9e3779b9 + (obj_hash << 6) + (obj_hash >> 2));
        }
    };
}
template<typename... Args>
class Event
{
private:
    std::unordered_set<EventCallback<Args...>> callbacks;

public:
    void AddUnique(const EventCallback<Args...>& callback) { callbacks.insert(callback); }
    void RemoveUnique(const EventCallback<Args...>& callback) { callbacks.erase(callback); }

    void Broadcast(Args... args) const
    {
        for(const EventCallback<Args...>& callback : callbacks)
            callback.lambda(args...);
    }
};