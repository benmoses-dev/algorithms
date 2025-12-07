#pragma once

#include <deque>

namespace algo::ds {

using ll = long long;

class MyTeque {
  private:
    std::deque<ll> left;
    std::deque<ll> right;

  public:
    void push_back(const ll x) {
        right.push_back(x);
        if (right.size() > left.size()) {
            const ll y = right.front();
            right.pop_front();
            left.push_back(y);
        }
    }
    void push_front(const ll x) {
        left.push_front(x);
        if (left.size() > right.size() + 1) {
            const ll y = left.back();
            left.pop_back();
            right.push_front(y);
        }
    }
    void push_middle(const ll x) {
        if (left.size() > right.size()) {
            right.push_front(x);
        } else {
            left.push_back(x);
        }
    }
    ll get(ll index) const {
        if (index >= left.size()) {
            index = index - left.size();
            return right.at(index);
        } else {
            return left.at(index);
        }
    }
};

} // namespace algo::ds
