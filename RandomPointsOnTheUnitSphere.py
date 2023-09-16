import random
import math
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# print('Hello world.')

x_points = []
y_points = []
z_points = []
for i in range(500):
    r1 = random.random()
    r2 = random.random()
    x = math.cos(2*math.pi*r1)*2*math.sqrt(r2*(1-r2))
    y = math.sin(2*math.pi*r1)*2*math.sqrt(r2*(1-r2))
    z = 1 - 2*r2
    x_points.append(x)
    y_points.append(y)
    z_points.append(z)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(x_points, y_points, z_points)
plt.show()
