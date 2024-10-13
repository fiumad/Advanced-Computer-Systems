import matplotlib.pyplot as plt

def plot_rubric_1():
    x = [.1, .05, .01]
    y = [.04759, .04369, .04034]
    # Plot 1000x1000 data
    plt.subplot(1, 3, 1)
    plt.title('1000x1000 Matrix Multiplication Sparsity vs Execution Time')
    plt.plot(x, y, marker='o')

    # Plot 5000x5000 data
    plt.subplot(1, 3, 2)
    y = [2.2989, 2.2003, 2.0999]
    plt.title('5000x5000 Matrix Multiplication Sparsity vs Execution Time')
    plt.plot(x, y, marker='o')

    # Plot 10000x10000 data
    plt.subplot(1, 3, 3)
    y = [18.768, 18.8366, 19.144]
    plt.title('10000x10000 Matrix Multiplication Sparsity vs Execution Time')
    plt.plot(x, y, marker='o')

    # Save the plot
    plt.show()

def plot_rubric_2():
    categories = ['No Optimization', 'Cache Miss Optimization', 'SIMD', 'SIMD and Cache Miss Optimization', 'Multithreading', 'Multithreading and Cache Miss Optimization', 'Multithreading and SIMD', 'Multithreading, SIMD, and Cache Miss Optimization']
    y1 = [1.1474, 1.0303, .1437, 1.5513, .5794, .53103, .55707, .02133]
    y2 = [32.8, 17.187, 1.4459, .91088, 5.8983, 6.5477, 6.0754, .17309]
    plt.subplot(1, 2, 1)
    plt.title('S1N1 Matrix Multiplication - Optimization Methods vs Execution Time, Size = 1000x1000, Sparsity = .1')
    plt.bar(categories, y1)
    plt.xlabel('Optimization Methods')
    plt.ylabel('Execution Time (s)')

    plt.subplot(1, 2, 2)
    plt.title('S2N2 Matrix Multiplication - Optimization Methods vs Execution Time, Size = 2000x2000, Sparsity = .2')
    plt.bar(categories, y2)
    plt.xlabel('Optimization Methods')
    plt.ylabel('Execution Time (s)')
    plt.show()

def plot_rubric_3():
    x = [500, 1000, 1500, 2000, 2500, 3500]
    y = [.02784, .616722, 2.2857, 6.00966, 11.99237, 36.003]
    plt.title('Dense Matrix Multiplication - Size vs Execution Time, Sparsity = 0.9')
    plt.plot(x, y, marker='o')

    # Save the plot
    plt.show()

def plot_rubric_4():
    x = [0, .05, .1, .15, .2, .25, .3]
    y = [.11366, .1444, .1378, .14971, .15103, .16447, .17501]
    plt.subplot(1, 2, 1)
    plt.title('Sparse Matrix Multiplication - Sparsity vs Execution Time, Size = 2000x2000')
    plt.plot(x, y, marker='o')
    plt.subplot(1, 2, 2)
    plt.title('Sparse Matrix Multiplication - Size vs Execution Time, Sparsity = .1')
    x = [100, 200, 300, 400, 500, 600, 700, 800]
    y = [.00184, .002307, .00323, .00389, .00424, .00568, .00669, .0114]
    plt.plot(x, y, marker='o')


    # Save the plot
    plt.show()

def plot_rubric_5():
    x = [0, .05, .1, .15, .2, .25, .3, .35, .4, .45]
    y = [.1207, .1346, .1409, .14719, .15777, .16186, .17179, .1856, .2064, .2091]
    plt.subplot(1, 2, 1)
    plt.title('Dense-Sparse Matrix Multiplication - Sparsity vs Execution Time, Size = 2000x2000, Dense matrix sparsity = 0.9')
    plt.plot(x, y, marker='o')
    # Save the plot
    plt.subplot(1, 2, 2)
    x = [500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500]
    y = [.00359, 0.1854, .058519, .148724, .27106, .47355, .752989, 1.0962, 1.609]
    plt.title('Dense-Sparse Matrix Multiplication - Size vs Execution Time, Dense matrix sparsity = 0.9, Sparse matrix sparsity = .1')
    plt.plot(x, y, marker='o')

    plt.show()

if __name__ == '__main__':
    #plot_rubric_1()
    plot_rubric_2()
    #plot_rubric_3()
    #plot_rubric_4()
    #plot_rubric_5()
