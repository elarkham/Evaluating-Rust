#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt

def mklist(filename):
    acc = []
    with open(filename, "r") as f:
        for line in f:
            acc.append(float(line[:-3]))
    return acc

if (len(sys.argv) != 3):
    print("usage: mkgraph cpp rust")
    exit(-1)

cpp = mklist(sys.argv[1])
rust = mklist(sys.argv[2])
#x_range = range(1, min(len(soa), len(aos)), 24)
x_range = range(1,25)

plt.xticks(range(1,25))
plt.plot(x_range, cpp, 'g--', x_range, rust, 'r--')
plt.legend(['C++', 'Rust'])
plt.ylabel("duration in seconds")
plt.xlabel("number of threads")
plt.show()
