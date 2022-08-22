n_max = 60
a = [0, 1]  # a[0] =0 and a[1] = 1

for i in range(2, n_max + 1):
    a.append(a[-1] + a[-2])

for i in range(10, n_max + 1, 10):
    print(i, a[i])
