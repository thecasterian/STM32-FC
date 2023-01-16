import numpy as np

data = np.loadtxt('stddev.txt', delimiter=',', skiprows=1)
m = np.mean(data, axis=0)
std = np.std(data, axis=0)

std_acc = (std[0] + std[1] + std[2]) / (3 * np.sqrt(m[0] ** 2 + m[1] ** 2 + m[2] ** 2))
std_gyro = (std[3] + std[4] + std[5]) / 3
std_mag = (std[6] + std[7] + std[8]) / (3 * np.sqrt(m[6] ** 2 + m[7] ** 2 + m[8] ** 2))

print(f"{std_acc:.10f}, {std_gyro:.10f}, {std_mag:.10f}")
