import sys
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = '1_000.csv'

def plot_acc(acc):
    x = np.linspace(0,len(acc[:,0]),len(acc[:,0]))
    plt.plot(x, acc[:,0])
    plt.plot(x, acc[:,1])
    plt.plot(x, acc[:,2])
    plt.legend(['acc.x', 'acc.y', 'acc.z'], loc='upper right')
    plt.show()
	
acc = np.genfromtxt(filename, delimiter=',')
print(acc)

plot_acc(acc)
