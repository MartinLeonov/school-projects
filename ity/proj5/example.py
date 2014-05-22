import math

def distance(a, b):
    x = math.pow((b[0] - a[0]), 2)
    y = math.pow((b[1] - a[1]), 2)
    z = math.pow((b[2] - a[2]), 2)
    distance = math.sqrt(x + y + z)
    print(distance)
