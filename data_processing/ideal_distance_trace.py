import matplotlib.pyplot as plt
import numpy as np

paths = [
    "v0.csv",
    "v1.csv",
    #"v2.csv",
]

def computeIdealDistanceCACC(speed):
  d = 6
  d_p = 6
  vehicleLen = 15.5
  commuLatency = 0.5
  lower_safe_boundary_with_comm_latency = 3.65 * commuLatency * commuLatency + 3
  boundaryValue1 = 0.3
  boundaryValue2 = 1.3
  ret = 0

  r_safe = speed * speed / 2 * (1 / d - 1 / d_p) + 0.1 * speed
  if (r_safe < boundaryValue1):
    ret = r_safe
  elif (r_safe <= boundaryValue2):
    ret = 1.2 * vehicleLen
  elif (r_safe <= 1.6 * vehicleLen):
    ret = 1.6 * vehicleLen
  else:
    ret = r_safe
  
  if (ret < lower_safe_boundary_with_comm_latency):
    ret = lower_safe_boundary_with_comm_latency

  return ret

def computeIdealDistanceACC(speed):
    return 3 * (speed / 6 + 1) ** 2 + 5

data0 = np.loadtxt(paths[0], delimiter=",", dtype=float)
for path in paths:
    data = np.loadtxt(path, delimiter=",", dtype=float)
    time = np.arange(0, len(data), 1)
    speed = data[:, 0]

    actualDistance = data[:, 1]
    idealDistanceCACC = np.array([computeIdealDistanceCACC(s) for s in data0[:, 0]])
    idealDistanceACC = np.array([computeIdealDistanceACC(s) for s in speed])
    plt.plot(time, idealDistanceACC, label="ACC Ideal")
    plt.plot(time, idealDistanceCACC, label="CACC Ideal")
    plt.plot(time, actualDistance, label="Actual")
    plt.legend()
    plt.xlabel("Time")
    plt.ylabel("Distance")
    plt.grid()
    plt.savefig(path + ".png", dpi=300)
    plt.clf()

