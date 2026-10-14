#ifndef REGISTRY_H
#define REGISTRY_H

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

template <typename T>
class Registry {
public:
    template <typename U>
    size_t store(U&& value) {
        size_t id;
        if (!freeIds.empty()) {
            // There are free ids to use before using new ones
            id = freeIds.back();
            freeIds.pop_back();
            data[id] = std::forward<U>(value);  // std::forward preserves the original rvalue or lvalue
            isAlive[id] = true;
        } 
        else {
            id = data.size();
            data.push_back(std::forward<U>(value));
            isAlive.push_back(true);
        }
        ++count;

        return id;
    }

    void remove(size_t id) {
        assert(valid(id));
        data[id] = T{};  // assigning releases the old resources without losing the position - default initialized value
        isAlive[id] = false;
        freeIds.push_back(id);
        --count;
    }

    T& operator[](size_t id) {
        assert(valid(id));
        return data[id];
    }

    const T& operator[](size_t id) const {
        assert(valid(id));
        return data[id];
    }

    bool valid(size_t id) const 
    { 
        return (id < data.size()) && isAlive[id]; 
    }

    size_t size() const { return count; }

private:
    std::vector<T> data;
    std::vector<bool> isAlive;
    std::vector<size_t> freeIds;
    size_t count = 0;  // number of alive objects

};

#endif  // REGISTRY_H