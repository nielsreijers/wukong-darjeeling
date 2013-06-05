#!/usr/local/bin/python
import reconfig_demo as demo
from wkpf.wuapplication import ChangeSets
from wkpf.wkpfcomm import getComm
from wkpf.wkpfcomm import WKPF_PROPERTY_TYPE_SHORT, WKPF_PROPERTY_TYPE_BOOLEAN, WKPF_PROPERTY_TYPE_REFRESH_RATE
from wkpf.transport import ZwaveAgent
from wkpf.mapper import first_of
from wkpf.models import WuNode, WuClassDef, WuClass, WuObject, WuComponent

## Testing Scenario:
# Testing where a node 32 in region 1 will be turned off

# Initially there is only one node 32 in region 1
# This generates the new configuration which will turn off every node
def dummy_initial_configuration():
  data = {}
  for ind in range(demo.NUMBER_OF_REGIONS):
    region = ind+1
    data[str(region)] = 0
  return data

# This generates the old configuration which has one node in region 1
def dummy_configuration():
  data = {}
  for ind in range(demo.NUMBER_OF_REGIONS):
    region = ind+1
    data[str(region)] = 0
  data['1'] = 1
  return data

# Generates a fake deployed application with WuComponent instances
def dummy_last_changesets(dummy_configuration, fake_network_info):
  wuclassdef = WuClassDef.find(id=demo.wuclass_id)

  dummy = ChangeSets([], [], [])
  for region, requirements in dummy_configuration.items():
    dummy.components.append(WuComponent(0, 
      str(region), int(requirements), 1, wuclassdef.name, 'demo_GH'))

  for region, nodes in fake_network_info.items():
    for component in dummy.components:
      if component.location == region:
        for node in nodes:
          component.instances += node.wuobjects()
        break
  return dummy

# test env setup
def fake_node():
  id = 32
  location = '1'
  energy = 3451.4

  node = WuNode.find(id=id)

  if not node:
    node = WuNode.create(id, location, energy)
  else:
    node.location = location
    node.energy = energy
    node.save()

  print 'Going into node', node.id

  # For this demo, we need to creat an wuobject for every node
  # And fake the property ourselves (defaul: on)
  wuclassdef = WuClassDef.find(id=demo.wuclass_id)
  if not wuclassdef:
    raise Exception("can't find wuclass with id %d" % (demo.wuclass_id))

  wuclass = WuClass.find(wuclassdef_identity=wuclassdef.identity,
      node_identity=node.identity)

  if not wuclass:
    print 'Creating wuclass', wuclassdef.id
    wuclass = WuClass.create(wuclassdef, node, False)

  wuobject = WuObject.find(wuclass_identity=wuclass.identity)

  if not wuobject:
    print 'Creating wuobject', wuclassdef.id
    # TODO: need to be certain about port number or reconfig will fail
    port_number = 1
    wuobject = WuObject.create(port_number, wuclass)

  # Right now creating a new WuObject will also creates WuProperties
  # automatically

  # Should create wuproperty here but need to discuss later if we should have
  # a procedure to retrieve property

  return node

def retrieve_fake_network_info():
  print 'Faking network info'
  network_info_data = {}
  node = fake_node()
  network_info_data.setdefault(node.location, []).append(node)
  return network_info_data

def fakeDeploy(commands):
  for wuproperty in commands:
    print 'setting property of node', wuproperty.wuobject().wunode().id
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

'''
ZwaveAgent.__init__ = dummy
demo.deploy = fakeDeploy
'''




# New configuration (All nodes off)
configuration = dummy_initial_configuration()
#print "config", configuration

# New application (All nodes off)
new_changesets = demo.generate_demo_application(configuration)
#print "new application", new_changesets

network_info = retrieve_fake_network_info()
#print "network info", network_info

# Old dummy configuration (One node in region 1)
configuration = dummy_configuration()
#print "old config", configuration

# Previous application (node 32 in region 1 is on)
last_changesets = dummy_last_changesets(configuration, network_info)
#print "last deployed application", last_changesets

commands, last_changesets = first_of(new_changesets, network_info, last_changesets)

print "commands", commands

demo.deploy(commands)
