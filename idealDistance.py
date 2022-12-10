import matplotlib.pyplot as plt
import numpy as np

def prevComputeIdealDistanceCACC(speed):
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

def computeIdealDistanceCACC(speed):
  vehicleLen = 15.5

  if (speed < 10):
    ret = 0.6 * vehicleLen + 0.6 * vehicleLen /(1 + np.exp(-((speed - 3.5))))
  else:
    ret = 1.2 * vehicleLen + 0.4 * vehicleLen /(1 + np.exp(-(speed - 13.5)))
    
  return ret

def computeIdealDistanceACC(speed):
    return 3 * (speed / 6 + 1) ** 2 + 5

speed = np.arange(0, 30, 0.1)
idealDistanceCACC = np.array([computeIdealDistanceCACC(s) for s in speed])
prevIdealDistanceACC = np.array([prevComputeIdealDistanceCACC(s) for s in speed])
idealDistanceACC = np.array([computeIdealDistanceACC(s) for s in speed])
plt.plot(speed, idealDistanceACC, label="Ideal ACC")
plt.plot(speed, prevIdealDistanceACC, label="Ideal CACC")
plt.plot(speed, idealDistanceCACC, label="Smoothed CACC")

plt.title("Ideal Distances for ACC and CACC")
plt.legend()
plt.xlabel("Speed")
plt.ylabel("Ideal Distance")
plt.grid()
plt.savefig("idealDistance.png", dpi=300)

def computeSafeDistanceACC(speed):
    return 3 * (speed / 6 + 1) ** 2
safeDistanceACC = np.array([computeSafeDistanceACC(s) for s in speed])
plt.clf()
plt.plot(speed, safeDistanceACC, label="ACC Safe Boundary")
plt.plot(speed, [3.65 * 0.5 * 0.5 for i in range(len(speed))], label="CACC Safe Boundary")

plt.title("Safe Boundary for ACC and CACC")
plt.legend()
plt.xlabel("Speed")
plt.ylabel("Safe Boundary")
plt.grid()
plt.savefig("safeBound.png", dpi=300)
