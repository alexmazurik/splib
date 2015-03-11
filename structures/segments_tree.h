#pragma once

#include <cstdint>
#include <functional>
#include <vector>


namespace segmentTree {

template <typename T, typename Node>
T getValue(const Node& node) { return node.value; }

template <typename T>
T getValue(const T& value) { return value; }

template <typename T, std::function<T(const T&, const T&)> Func>
struct DefaultNode {
    DefaultNode(uint32_t /*index*/, T value) : value(value) {}
    DefaultNode(
            DefaultNode* left,
            DefaultNode* right,
            uint32_t /*leftSon*/,
            uint32_t /*rightSon*/) :
        value(Func(left->value, right->value))
    {}

    T value;
};

template <
    typename T = int32_t,
    std::function<T(const T&, const T&)> Func = std::plus<T>(),
    typename Node = DefaultNode<T, Func>,
    bool Push = false,
    bool Update = false
>
class SegmentTree {
public:
    SegmentTree(const std::vector<T>& values) :
        SegmentTree(values.begin(), values.end()) {}

    template <class Iterator>
    SegmentTree(Iterator begin, Iterator end) : size_(end - begin) {
        uint32_t levels = 0;
        while ((1 << levels) < size_) {
            ++levels;
        }
        tree_.resize((1 << (levels + 1)) - 2);

        if (size_) {
            build(0, 0, size_, begin);
        }
    }

private:
    static uint32_t getLeftSon(uint32_t v) { return 2 * v + 1; }
    static uint32_t getRightSon(uint32_t v) { return 2 * v + 2; }

    template<class Iterator>
    void build(uint32_t v, uint32_t left, uint32_t right, Iterator begin) {
        if (left + 1 == right) {
            tree_[v] = Node(left, *(begin + left));
            return;
        }
        uint32_t leftSon = getLeftSon(v);
        uint32_t rightSon = getRightSon(v);
        uint32_t median = (left + right) / 2;

        build(leftSon, left, median, begin);
        build(rightSon, median, right, begin);

        tree_[v] = Node(&tree_[leftSon], &tree_[rightSon], left, right);
    }
    std::vector<Node> tree_;
    uint32_t size_;

};
}
