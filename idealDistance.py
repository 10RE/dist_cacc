import matplotlib.pyplot as plt
import numpy as np

def computeIdealDistanceCACC(speed):
  d = 6
  d_p = 6
  vehicleLen = 15.5
  commuLatency = 0.5
  lower_safe_boundary_with_comm_latency = 3.65 * commuLatency * commuLatency + 3
  boundaryValue1 = 0.3
  boundaryValue2 = 1.3
  ret = 0

  if (r_safe < 10):
    ret = 1.2 * vehicleLen /(1 + np.exp(-(0.6 * (r_safe - 3.5))))
  else:
    ret = 1.2 * vehicleLen + 0.4 * vehicleLen /(1 + np.exp(-(0.6 * (r_safe - 3.5))))

  # r_safe = speed * speed / 2 * (1 / d - 1 / d_p) + 0.1 * speed
  # if (r_safe < boundaryValue1):
  #   ret = r_safe
  # elif (r_safe <= boundaryValue2):
  #   ret = 1.2 * vehicleLen
  # elif (r_safe <= 1.6 * vehicleLen):
  #   ret = 1.6 * vehicleLen
  # else:
  #   ret = r_safe
  
  # if (ret < lower_safe_boundary_with_comm_latency):
  #   ret = lower_safe_boundary_with_comm_latency
    
  return ret

def computeIdealDistanceACC(speed):
    return 3 * (speed / 6 + 1) ** 2 + 5

speed = np.arange(0, 30, 0.1)
idealDistanceCACC = np.array([computeIdealDistanceCACC(s) for s in speed])
idealDistanceACC = np.array([computeIdealDistanceACC(s) for s in speed])
plt.plot(speed, idealDistanceACC, label="ACC")
plt.plot(speed, idealDistanceCACC, label="CACC")
plt.legend()
plt.xlabel("Speed")
plt.ylabel("Ideal Distance")
plt.grid()
plt.savefig("idealDistance.png", dpi=300)