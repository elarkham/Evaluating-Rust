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
    print("usage: mkgraph soa aos")
    exit(-1)

soa = mklist(sys.argv[1])
aos = mklist(sys.argv[2])
x_range = range(1, min(len(soa), len(aos)) * 1000, 1000)

plt.plot(x_range, soa, 'g--', x_range, aos, 'r--')
plt.ylabel("duration in seconds")
plt.xlabel("size of dataset")
plt.show()
