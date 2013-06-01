import math

ber=0.00002
mpl=0.2
up=100

for num in range(1,up):  #to iterate between 10 to 20
   if ber*(math.pow(1+mpl, num)) > 1: break
   print 'app -d 10 img_test_01.gif -b %f | grep \'RBER\'' %(ber*(math.pow(1+mpl, num)))
