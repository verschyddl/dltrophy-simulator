//
// Created by qm210 on 11.07.2025.
//

#ifndef DLTROPHY_SIMULATOR_OBSERVABLES_H
#define DLTROPHY_SIMULATOR_OBSERVABLES_H

#include <string>
#include <functional>
#include <vector>

using Observable = std::function<std::string()>;

class Observables {
public:

    Observables() = default;

    [[nodiscard]]
    std::vector<Observable> copy() const { return registry; }

    template<typename T>
    auto add(const T& obj) ->
        typename std::enable_if<
            std::is_convertible<decltype(obj.to_string()), std::string>::value
        >::type
    {
        registry.emplace_back([obj]() {
            return obj.to_string();
        });
    }

    template<typename T>
    auto add(const T& obj) ->
        typename std::enable_if<
            std::is_convertible<decltype(std::to_string(obj)), std::string>::value
        >::type
    {
        registry.emplace_back([obj]() {
            return std::to_string(obj);
        });
    }

    void clear()
    {
        registry.clear();
    }

private:
    std::vector<Observable> registry;
};

#endif //DLTROPHY_SIMULATOR_OBSERVABLES_H
