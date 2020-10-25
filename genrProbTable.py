from scipy.integrate import quad
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from tabulate import tabulate

def normalProbabilityDensity(x):
    constant = 1.0 / np.sqrt(2*np.pi)
    miu = 0
    sigma = 1
    return(constant * np.exp((-(x-miu)**2) / (2.0*(sigma)**2)))

normal_table = pd.DataFrame(data=[], index = np.round(np.arange(0,3.5,.1),2), columns = np.round(np.arange(0.00,.1,.01),2))
for index in normal_table.index:
    for column in normal_table.columns:
        a = np.round(index+column,2)
        value,_ = quad(normalProbabilityDensity, np.NINF,a)
        normal_table.loc[index] = value
normal_table.index=normal_table.index.astype(str)
normal_table.columns= [str(column).ljust(4,'0') for column in normal_table.columns]
print(tabulate(normal_table, headers = 'keys', tablefmt = 'psql')) 

def plotnormal():
    x = np.linspace(-4, 4, num = 100)
    constant = 1.0 / np.sqrt(2*np.pi)
    miu = 0
    sigma = 1
    pdf_normal_distribution = constant * np.exp((-(x-miu)**2) / (2.0*(sigma)**2))
    fig, ax = plt.subplots(figsize=(10, 5))
    ax.plot(x, pdf_normal_distribution)
    ax.set_ylim(0)
    ax.set_title('Normal Distribution', size = 20)
    ax.set_ylabel('Probability Density', size = 20)
    plt.show()

plotnormal()