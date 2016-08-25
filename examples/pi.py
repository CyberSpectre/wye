
import sys
import time

def pi(x):

    global total

    if not "total" in globals():
        total = 0.0

    frac = x[1] /  (16.0 ** x[0])
    total = total + (10 ** 100) * frac

#    total = total + (x[1] / (16.0 ** x[0]) * (10 ** 400))
    
#    sys.stderr.write("%d\n" % total)
    return None

def digit(k):

    global ovf

    if not "ovf" in globals():
        globals()["ovf"] = 0.0

    f1 = 4.0 / (8.0 * k + 1)
    f2 = - (2.0 / (8.0 * k + 4.0))
    f3 = - (1.0 / (8.0 * k + 5.0))
    f4 = - (1.0 / (8.0 * k + 6.0))
    
    f = (f1 + f2 + f3 + f4)

    ovf = ovf + f

    digit = (k, int(ovf))

    ovf = ovf - int(ovf)
    ovf = ovf * 16
    
    return digit

for k in range(0,100):
    d = digit(k)
    sys.stdout.write("%x" % d[1])
    if (d[0] % 8) == 0:
        sys.stdout.write(" ");
#    pi(d)
#    time.sleep(1)

print

