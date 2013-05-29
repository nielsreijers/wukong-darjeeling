import reconfig_demo as re
from wkpf.wuapplication import ChangeSets
from wkpf.wkpfcomm import getComm
from wkpf.wkpfcomm import WKPF_PROPERTY_TYPE_SHORT, WKPF_PROPERTY_TYPE_BOOLEAN, WKPF_PROPERTY_TYPE_REFRESH_RATE
from wkpf.transport import ZwaveAgent
from wkpf.mapper import first_of

configuration = re.dummy_configuration()

print "dummy config", configuration

configuration = re.retrieve_configuration()

print "configuration", configuration

network_info = re.retrieve_network_info()

print "network_info", network_info

changesets = re.generate_demo_application(network_info, configuration)

#print "demo application", changesets

def fakeDeploy(commands):
  for wuproperty in commands:
    wunode = wuproperty.wuobject().wunode()
    setFakeProperty(wuproperty)

def setFakeProperty(wuproperty):
  wuobject = wuproperty.wuobject()
  wuclassdef = wuobject.wuclass().wuclassdef()
  wunode = wuobject.wunode()
  value = wuproperty.value
  datatype = wuproperty.datatype
  number = wuproperty.wupropertydef().number

  if datatype == 'boolean':
    datatype = WKPF_PROPERTY_TYPE_BOOLEAN

  elif datatype == 'short':
    datatype = WKPF_PROPERTY_TYPE_SHORT

  elif datatype == 'refresh_rate':
    datatype = WKPF_PROPERTY_TYPE_REFRESH_RATE

  if datatype == WKPF_PROPERTY_TYPE_BOOLEAN:
    payload=[wuobject.port_number, wuclassdef.id/256,
    wuclassdef.id%256, number, datatype, 1 if value else 0]

  elif datatype == WKPF_PROPERTY_TYPE_SHORT or datatype == WKPF_PROPERTY_TYPE_REFRESH_RATE:
    payload=[wuobject.port_number, wuclassdef.id/256, wuclassdef.id%256, number, datatype, value/256, value%256]

  print "payload", payload

def dummy(cls):
  pass

ZwaveAgent.__init__ = dummy

re.deploy = fakeDeploy

last_changesets = re.dummy_changesets()

#print "dummy changesets", last_changesets

commands, last_changesets = first_of(changesets, network_info, last_changesets)

print "commands", commands

re.deploy(commands)
