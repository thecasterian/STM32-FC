import numpy as np
from scipy.optimize import leastsq
from scipy.linalg import sqrtm
import matplotlib.pyplot as plt

G = 9.80665

# Load data
ax = []
ay = []
az = []
for i in range(1, 33):
    data = np.loadtxt('acc_calib_' + str(i) + '.txt', delimiter=',', skiprows=1)
    ax.append(np.mean(data[:, 0]))
    ay.append(np.mean(data[:, 1]))
    az.append(np.mean(data[:, 2]))
ax = np.array(ax)
ay = np.array(ay)
az = np.array(az)

# Spherical fit
def func(param, ax, ay, az):
    R, Vx, Vy, Vz = param

    return (ax - Vx) ** 2 + (ay - Vy) ** 2 + (az - Vz) ** 2 - R ** 2

f = np.vectorize(func, excluded=[0])
plsq = leastsq(f, [G, 0, 0, 0], args=(ax, ay, az))

# Calculate calibration
R, Vx, Vy, Vz = plsq[0]

axc = (ax - Vx) / R * G
ayc = (ay - Vy) / R * G
azc = (az - Vz) / R * G

# Print calibration values
print(f"{G / R:.10f}")
print(f"{Vx:.10f}, {Vy:.10f}, {Vz:.10f}")

# Plot data
fig = plt.figure()

ax1 = fig.add_subplot(121, projection='3d')
ax1.scatter(ax, ay, az, marker='o')
ax1.set_aspect('equal', adjustable='datalim')

ax2 = fig.add_subplot(122, projection='3d')
ax2.scatter(axc, ayc, azc, color='r', marker='o')
ax2.set_aspect('equal', adjustable='datalim')

plt.show()
