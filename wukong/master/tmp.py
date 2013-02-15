from wkpf.wkpfcomm import *
c=Communication()
c.setLocation(2, 'rotterdam')
x=c.getLocation(2)
print x
print len(x)
