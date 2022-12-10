import matplotlib.pyplot as plt
import numpy as np

path_acc = [
    "acc\\v0.csv",
    "acc\\v1.csv",
    "acc\\v2.csv",
]

path_cacc = [
    "v2\\v0.csv",
    "v2\\v1.csv",
    "v2\\v2.csv",
]

def computeIdealDistanceCACC(speed):
  vehicleLen = 15.5

  if (speed < 10):
    ret = 0.6 * vehicleLen + 0.6 * vehicleLen /(1 + np.exp(-((speed - 3.5))))
  else:
    ret = 1.2 * vehicleLen + 0.4 * vehicleLen /(1 + np.exp(-(speed - 13.5)))
    
  return ret

def computeIdealDistanceACC(speed):
    return 3 * (speed / 6 + 1) ** 2 + 5

acc_range = [69, 134]
cacc_range = [23, 132]

acc_data = []
for path in path_acc:
    acc_data.append(np.loadtxt(path, delimiter=",", dtype=float))
cacc_data = []
for path in path_cacc:
    cacc_data.append(np.loadtxt(path, delimiter=",", dtype=float))

acc_diff_v1 =[acc_data[1][i, 1] for i in range(acc_range[0], acc_range[1])] - np.array([computeIdealDistanceACC(acc_data[1][i, 0]) for i in range(acc_range[0], acc_range[1])])
acc_diff_v2 = [acc_data[2][i, 1] for i in range(acc_range[0], acc_range[1])] - np.array([computeIdealDistanceACC(acc_data[2][i, 0]) for i in range(acc_range[0], acc_range[1])])
print("acc max diff v1: ", np.max(np.abs(acc_diff_v1)))
print("acc max diff v2: ", np.max(np.abs(acc_diff_v2)))

print("acc avg diff v1: ", np.mean(np.abs(acc_diff_v1)))
print("acc avg diff v2: ", np.mean(np.abs(acc_diff_v2)))

cacc_diff_v1 = [cacc_data[1][i, 1] for i in range(cacc_range[0], cacc_range[1])] - np.array([computeIdealDistanceCACC(cacc_data[0][i, 0]) for i in range(cacc_range[0], cacc_range[1])])
cacc_diff_v2 = [cacc_data[2][i, 1] for i in range(cacc_range[0], cacc_range[1])] - np.array([computeIdealDistanceCACC(cacc_data[0][i, 0]) for i in range(cacc_range[0], cacc_range[1])])
print("cacc max diff v1: ", np.max(np.abs(cacc_diff_v1)))
print("cacc max diff v2: ", np.max(np.abs(cacc_diff_v2)))

print("cacc avg diff v1: ", np.mean(np.abs(cacc_diff_v1)))
print("cacc avg diff v2: ", np.mean(np.abs(cacc_diff_v2)))



acc_x = np.arange(0, acc_range[1] - acc_range[0], 1)
cacc_x = np.arange(0, cacc_range[1] - cacc_range[0], 1)
plt.plot(acc_x, [acc_data[1][i, 1] for i in range(acc_range[0], acc_range[1])], label="v1 ACC")
plt.plot(acc_x, [computeIdealDistanceACC(acc_data[1][i, 0]) for i in range(acc_range[0], acc_range[1])], label="v1 ACC ideal")
plt.plot(cacc_x, [cacc_data[1][i, 1] for i in range(cacc_range[0], cacc_range[1])], label="v1 CACC")
plt.plot(cacc_x, [computeIdealDistanceCACC(cacc_data[0][i, 0]) for i in range(cacc_range[0], cacc_range[1])], label="v1 CACC ideal")
plt.plot(acc_x, [acc_data[2][i, 1] for i in range(acc_range[0], acc_range[1])], label="v2 ACC")
plt.plot(acc_x, [computeIdealDistanceACC(acc_data[2][i, 0]) for i in range(acc_range[0], acc_range[1])], label="v2 ACC ideal")
plt.plot(cacc_x, [cacc_data[2][i, 1] for i in range(cacc_range[0], cacc_range[1])], label="v2 CACC")
plt.plot(cacc_x, [computeIdealDistanceCACC(cacc_data[0][i, 0]) for i in range(cacc_range[0], cacc_range[1])], label="v2 CACC ideal")
plt.legend()
plt.title("Comparison of ACC and CACC: leading vehicle at 0.75 m/s^2")
plt.xlabel("Time")
plt.ylabel("Distance")
plt.grid()
plt.savefig("metrics_throttle.png", dpi=300)
plt.clf()

acc_x = np.arange(0, acc_range[1] - acc_range[0], 1)
cacc_x = np.arange(0, cacc_range[1] - cacc_range[0], 1)
plt.plot(acc_x, acc_diff_v1, label="v1 ACC diff")
plt.plot(acc_x, acc_diff_v2, label="v2 ACC diff")
plt.plot(cacc_x, cacc_diff_v1, label="v1 CACC diff")
plt.plot(cacc_x, cacc_diff_v2, label="v2 CACC diff")
plt.legend()
plt.title("Comparison of ACC and CACC: leading vehicle at 0.75 m/s^2")
plt.xlabel("Time")
plt.ylabel("Distance difference")
plt.grid()
plt.savefig("metrics_throttle_diff.png", dpi=300)
plt.clf()

acc_x = np.arange(0, acc_range[1] - acc_range[0], 1)
cacc_x = np.arange(0, cacc_range[1] - cacc_range[0], 1)
plt.plot(acc_x, [acc_data[0][i, 2] for i in range(acc_range[0], acc_range[1])], label="acc leading throttle")
plt.plot(cacc_x, [cacc_data[0][i, 2] for i in range(cacc_range[0], cacc_range[1])], label="cacc leading throttle")
plt.legend()
plt.title("Leading vehicle at 0.75 m/s^2")
plt.xlabel("Time")
plt.ylabel("Throttle")
plt.grid()
plt.savefig("metrics_throttle_leading_throttle.png", dpi=300)
plt.clf()

plt.plot(acc_x, [acc_data[0][i, 0] for i in range(acc_range[0], acc_range[1])], label="acc leading speed")
plt.plot(cacc_x, [cacc_data[0][i, 0] for i in range(cacc_range[0], cacc_range[1])], label="cacc leading speed")
plt.legend()
plt.title("Leading vehicle at 0.75 m/s^2")
plt.xlabel("Time")
plt.ylabel("Speed")
plt.grid()
plt.savefig("metrics_throttle_leading_speed.png", dpi=300)
plt.clf()