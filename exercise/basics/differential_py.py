from math import cos, pi, sin


def f(x):
    return sin(x)


def df(x):
    return cos(x)


x = 0.3 * pi
h = 0.1
exact = df(x)
diff = (f(x + h) - f(x)) / h
print(h, diff, abs(diff - exact))
