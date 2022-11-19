import numpy as np

# Load data
data = np.loadtxt('gyro_calib.txt', delimiter=',', skiprows=1)

# Compute mean
mean = np.mean(data[:, 3:], axis=0)

# Print
print(f'{mean[0]:.8e}, {mean[1]:.8e}, {mean[2]:.8e}')
