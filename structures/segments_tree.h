#pragma once

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
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

template<bool Push>
struct Pusher {
template <typename Node>
static void update(
        Node* parrent,
        const Node& leftSon, const Node& rightSon,
        int left, int right) {
    parrent->update(leftSon, rightSon, left, right);
}

template <typename Node>
static void push(
        Node* parrent,
        Node* leftSon, Node* rightSon,
        int left, int right) {
    parrent->push(leftSon, rightSon, left, right);
}
};

template<>
struct Pusher<false> {
template <typename Node>
static void update(
        Node* /*parrent*/,
        const Node& /*leftSon*/, const Node& /*rightSon*/,
        int /*left*/, int /*right*/) {}

template<typename Node>
static void push(
        Node* /*parrent*/,
        Node* /*leftSon*/, Node* /*rightSon*/,
        int /*left*/, int /*right*/) {}
};

template<typename Node>
void combineOrCreate(
        const Node& node, int left, int right, std::unique_ptr<Node>* result) {
    if (!*result) {
        result = new Node(node);
    } else {
        (*result)->combine(node, left, right);
    }
}

template <typename Node, bool Push = false>
class SmartSegmentTree {
    typedef decltype(Node::value()) T;
    static const int ROOT = 0;
public:
    SmartSegmentTree(const std::vector<T>& values) :
        SmartSegmentTree(values.begin(), values.end()) {}

    template <class Iterator>
    SmartSegmentTree(Iterator begin, Iterator end) : size_(end - begin) {
        uint32_t levels = 0;
        while ((1 << levels) < size_) {
            ++levels;
        }
        tree_.resize((1 << (levels + 1)) - 2);

        if (size_) {
            build(ROOT, 0, size_, begin);
        }
    }

    template<class Combiner>
    std::unique_ptr<Node> combine(int askLeft, int askRight) {
        std::unique_ptr<Node> result;

        ask(ROOT, 0, size_, askLeft, askRight, &result);
        return result;
    }

    void update(int index, T newValue) {

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

        tree_[v].update(tree_[leftSon], tree_[rightSon], left, right);
    }

    template<class Callback>
    void forAllSegmentInDecomposition(
            int v,
            int left, int right,
            int askLeft, int askRight,
            Callback callback) {
        if (askLeft <= left && right <= askRight) {
            return callback(tree_[v], left, right);
        }

        Pusher<Push>::push(
                &tree_[v],
                tree_[getLeftSon(v)], tree_[getRightSon(v)],
                left, right);

        int middle = (left + right) / 2;
        if (middle > askLeft) {
            forAllSegmentInDecomposition(
                getLeftSon(v), left, middle, askLeft, askRight, callback);
        }
        if (middle < askRight) {
            forAllSegmentInDecomposition(
                getRightSon(v), middle, right, askLeft, askRight, callback);
        }

        Pusher<Push>::update(
                &tree_[v],
                &tree_[getLeftSon(v)], &tree_[getRightSon(v)],
                left, right);

    }

    std::vector<Node> tree_;
    uint32_t size_;
};

template <
    typename T = int32_t,
    std::function<T(const T&, const T&)> Func = std::plus<T>()
>
class SegmentTree: public SmartSegmentTree<DefaultNode<T, Func>> { // : private ?
    SegmentTree(const std::vector<T>& values) :
        SmartSegmentTree(values.begin(), values.end()) {}

    template <class Iterator>
    SegmentTree(Iterator begin, Iterator end) : SmartSegmentTree(begin, end) {}

    T ask(int askLeft, int askRight) {
        assert(L <= R);
        auto resultNode = combine(askLeft, askRight);
        return resultNode->value();
    }
};

}
