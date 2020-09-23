from __future__ import print_function, division
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import geom
import random

prob = 0.5
N = 24
P =[]
K =[]
for k in range(1,N+1):
    p = prob*(1-prob)**(k-1)
    K.append(k)
    P.append(p)
print('values of P(X)')
print(P)
print('values of x')
print(K)
plt.bar(range(1,25),P, color = 'blue')
plt.xlim(1,25)
plt.xlabel('x')
plt.ylabel('p(x)')
plt.title('p(x) plot ')
plt.show()
