import numpy as np
import matplotlib . pyplot as plt
import math as ma

n =24
p =0.15
 # number of N times
N =1000
number =[]
freq =[]
K=[]
 # freq array initializing
for k in range(1,n+1):
    K.append(k)
for i in range (n+1) :
    freq.append (0)
for i in range (N):
    geom =0
    for j in range (1,n+1):# calculating geom variables n times
        r = np.random.uniform(0,1,size=1)
        prob = p*(1-p)**(j-1)
        if r < prob:
            geom = geom + 1
    number.append (geom)# storing them in number array with length N
# calculating the how many times repeated in number array
for i in number :
    freq[i] = freq[i]+1
 # calculating observed frequency in fraction
for i in range (n+1) :
    freq[i]= freq[i]/ N
 # plotting in histogram
print('values of x')
print(K)
print('frequency values')
print(freq)
plt.bar(np.arange(0,n+1,1),freq, color = 'red')
plt.xlim(1,25)
plt.xlabel('x')
plt.ylabel('frequency/N')
plt.title('frequency plot ')
plt.show()

