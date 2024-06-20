#!/usr/bin/env python3

import subprocess
import numpy as np
import matplotlib.pyplot as plt

runs = 10

def outlier_filter(datas, threshold = 2):
    datas = np.array(datas)
    z = np.abs((datas - datas.mean()) / datas.std())
    return datas[z < threshold]

def data_processing(data_set, n):
    catgories = data_set[0].shape[0]
    samples = data_set[0].shape[1]
    final = np.zeros((catgories, samples))

    for c in range(catgories):        
        for s in range(samples):
            final[c][s] =                                                    \
                outlier_filter([data_set[i][c][s] for i in range(n)]).mean()
    return final


if __name__ == "__main__":
    comp_proc = subprocess.run('make', shell = True)
    comp_proc = subprocess.run('make insmod', shell = True)
    comp_proc = subprocess.run('sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"', shell = True)
    comp_proc = subprocess.run('sudo sh performance.sh', shell = True)
    with open('runtime_mean.txt', 'w') as file:
        with open('cmp_cnt_mean.txt', 'w') as file_cmp:
            Ys = []
            cmp_cnt = []
            for i in range(runs):
                comp_proc = subprocess.run('sudo  taskset 0x1 ./user qsort', shell = True)
                output = np.loadtxt('./time.txt', dtype = 'float').T
                output_cnt = np.loadtxt('./cmp_cnt.txt', dtype = 'float').T
                Ys.append(output)
                cmp_cnt.append(output_cnt)
            
            X_Q = output[0]
            Y_Q = data_processing(Ys, runs)
            Y_cnt = data_processing(cmp_cnt, runs)
            Ys = []
            for i in range(runs):
                comp_proc = subprocess.run('sudo taskset 0x1 ./user libsort', shell = True)
                output = np.loadtxt('./time.txt', dtype = 'float').T
                Ys.append(output)
            
            X_LIB = output[0]
            Y_LIB = data_processing(Ys, runs)
            for j in range(len(X_Q)):
                file.write(str(int(X_Q[j])) + " " + str(int(Y_Q[1][j])) + " " + str(int(Y_Q[2][j])) + " " + 
                        str(int(X_LIB[j])) + " " + str(int(Y_LIB[1][j])) + " " + str(int(Y_LIB[2][j])) + "\n")
                file_cmp.write(str(int(X_Q[j])) + " " + str(int(Y_cnt[1][j])) + "\n")

    
    

    fig, ax = plt.subplots(1, 1, sharey = True)
    ax.set_title('perf', fontsize = 16)
    ax.set_xlabel(r'$elements', fontsize = 16)
    ax.set_ylabel('time (ns)', fontsize = 16)

    #ax.scatter(X_Q, Y_cnt[1],   label = 'Q_kernel')
    #ax.scatter(X_Q, Y_Q[1],   label = 'Q_user')
    #ax.scatter(X_LIB, Y_LIB[0],   label = 'lib/sort_kernel')
    #ax.scatter(X_LIB, Y_LIB[1],   label = 'lib/sort_user')
    #ax.plot(X, Y[2], marker = '^', markersize = 3, label = 'kernel to user')
    ax.legend(loc = 'upper left')

    #plt.show()
