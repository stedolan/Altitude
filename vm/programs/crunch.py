ret = 0
for i in xrange(1000):
    for j in xrange(i):
        ret += 30
    for j in xrange(1,i):
        ret -= 30
print ret
