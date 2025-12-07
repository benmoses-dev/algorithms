#pragma once

class FenwickTree {
  private:
    int biTree[1000001];   // Index from 1
    int memArray[1000000]; // Index from 0
    int n;

  public:
    FenwickTree(int size) : n(size) {
        for (int i = 0; i <= n; i++) {
            biTree[i] = 0;
        }
        for (int j = 0; j < n; j++) {
            memArray[j] = 0;
        }
    }

    void update(int index) {
        int delta;
        // Update array of bits
        if (memArray[index] == 0) {
            memArray[index] = 1;
            delta = 1;
        } else {
            memArray[index] = 0;
            delta = -1;
        }
        // Update binary index tree
        index++;
        while (index <= n) {
            // Propogate up the array by flipping the next unset bit after the last
            biTree[index] += delta;
            // bitwise AND using two's compliment (this gets just the lsb)
            int lastSigBit = index & -index;
            index = index + lastSigBit; // Add the lsb to the index
        }
    }

    int sum(int index) {
        index++;
        int total = 0;
        while (index > 0) {
            total += biTree[index];
            int lastSigBit = index & -index;
            // Propogate back down, ignoring the lsb (essentially doubling the jump every
            // time)
            index = index - lastSigBit;
        }
        return total;
    }

    int rangeSum(int l, int r) { return sum(r) - sum(l - 1); }
};
