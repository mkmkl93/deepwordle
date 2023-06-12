import re
import matplotlib.pyplot as plt
import math

times = [0]

threads = []
for i in range(10):
    threads.append([0])

with open('../output2') as f:
    for line in f.readlines():
        result = re.search(r'(\d*?):?(\d*?):?(\d*) Job .* has ended by thread (\d*)', line)
        if result:
            hours = int(result[1]) if result[1] != '' else 0
            minutes = int(result[2]) if result[2] != '' else 0
            seconds = int(result[3]) if result[3] != '' else 0
            time = hours * 60 + minutes
            thread = int(result[4])

            if time < times[-1]:
                for i in range(10):
                    threads[i].append(0)

            times.append(time)
            threads[thread].append(time)


last_checked = 0
times2 = [0]
for i in range(1, len(times)):
    if times[i] < times[i - 1]:
        last_checked += times[i - 1]
    times2.append(times[i] + last_checked)
times = times2

asymptotic = [0]
for i in range(1, len(times)):
    asymptotic.append(0.1 * (times[i] - times[i - 1]) + 0.9 * asymptotic[-1])
    # asymptotic.append(0.1 * (times[i]/i) + 0.9 * asymptotic[-1])

plt.plot(asymptotic[30:])
plt.show()
print("Asymptotic: ", asymptotic[-1])

max_time = 0
for thread in threads:
    for i in range(1, len(thread)):
        max_time = max(max_time, thread[i] - thread[i - 1])


print("Max time: ", max_time)

print(times[-10:])
total = 2315
left = total - len(times) + 1
prc = (left/total)**6
print("Covered: ", len(times) - 1)
print("Percentage: ", prc * 100)
minutes_left = math.floor(asymptotic[-1] * left)
print("Estimated time: {}:{}:{}:00".format(minutes_left // 1440, (minutes_left % 1440) // 60, minutes_left % 60))

