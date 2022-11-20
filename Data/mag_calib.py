import numpy as np
from scipy.optimize import leastsq
from scipy.linalg import sqrtm
import matplotlib.pyplot as plt

# Load data
data = np.loadtxt('mag_calib.txt', delimiter=',', skiprows=1)
mx = data[:, 0]
my = data[:, 1]
mz = data[:, 2]

# Ellipsoid fit
def func(param, mx, my, mz):
    Axx, Axy, Axz, Ayy, Ayz, Azz, Vx, Vy, Vz = param
    res = []

    for mxx, myy, mzz in zip(mx, my, mz):
        x = np.array([mxx - Vx, myy - Vy, mzz - Vz])
        A = np.array([[Axx, Axy, Axz], [Axy, Ayy, Ayz], [Axz, Ayz, Azz]])
        res.append(x.T @ A @ x - 1)

    return np.array(res)

plsq = leastsq(func, [1, 0, 0, 1, 0, 1, 0, 0, 0], args=(mx, my, mz))

# Calculate calibration
Axx, Axy, Axz, Ayy, Ayz, Azz, Vx, Vy, Vz = plsq[0]
A = np.array([[Axx, Axy, Axz], [Axy, Ayy, Ayz], [Axz, Ayz, Azz]])
V = np.array([Vx, Vy, Vz])

B = 1 / np.linalg.det(A) ** (1 / 6)
A *= B ** 2

W_inv = sqrtm(A)
if np.linalg.det(W_inv) < 0:
    W_inv = -W_inv

data_c = (W_inv @ (data.T - V[:, np.newaxis])).T
mxc = data_c[:, 0]
myc = data_c[:, 1]
mzc = data_c[:, 2]

# Print calibration values
print(W_inv)
print(V)
print(B)

# Plot data
fig = plt.figure()

ax1 = fig.add_subplot(121, projection='3d')
ax1.scatter(mx, my, mz, marker='o')
ax1.set_aspect('equal', adjustable='datalim')

ax2 = fig.add_subplot(122, projection='3d')
ax2.scatter(mxc, myc, mzc, color='r', marker='o')
ax2.set_aspect('equal', adjustable='datalim')

plt.show()
