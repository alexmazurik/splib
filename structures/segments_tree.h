#pragma once

#include <cstdint>
#include <functional>
#include <vector>


namespace segmentTree {

template <typename T, typename Node>
T getValue(const Node& node) { return node.value; }

template <typename T>
T getValue(const T& value) { return value; }

template <
    typename T = int32_t,
    typename Node = T,
    std::function<T(const T&, const T&)> Func = std::plus<T>(),
    bool Push = false,
    bool Update = false
>
class SegmentTree {
public:
private:
    std::vector<Node> tree_;

};
}
