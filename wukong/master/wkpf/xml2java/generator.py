# vim: ts=4 sw=4
#!/usr/bin/python

import os, sys
from xml.etree import ElementTree
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from jinja2 import Template, Environment, FileSystemLoader
from struct import pack

from models import WuType

from configuration import *
from util import *


class Generator:
    @staticmethod
    def generate(name, changesets):
        Generator.generateJavaApplication(name, changesets)
        Generator.generateTablesXML(name, changesets)

    @staticmethod
    def generateJavaApplication(name, changesets):
        # i is the number to be transform into byte array, n is the number of bytes to use (little endian)
        def bytestring(i, n): 
            return ['(byte)' + str(ord(b)) for b in pack("H", i)][:n]

        def nodeinjava(node):
            return str(node.id)

        def wuobjectinjava(wuobject):
            return ', '.join([str(wuobject.node_id),
                            str(wuobject.port_number)])
            
        def linkinjava(link):
            return ', '.join(bytestring(link.from_component_index, 2)
                    + bytestring(link.from_property_id, 1)
                    + bytestring(link.to_component_index, 2)
                    + bytestring(link.to_property_id, 1)
                    + bytestring(link.to_wuclass_id, 2))

        def wuclassname(wuclass):
            return wuclass.name

        def wuclassvirtualclassname(wuclass):
            return "Virtual" + Convert.to_java(wuclass.name) + "WuObject"

        def wuclassconstname(wuclass):
            return "WUCLASS_" + Convert.to_constant(wuclass.name)

        def wuclassgenclassname(wuclass):
            return "GENERATEDVirtual" + Convert.to_java(wuclass.name) + "WuObject"

        def propertyconstname(property):
            return "PROPERTY_" + Convert.to_constant(property.wuclass.name) + "_" + Convert.to_constant(property.name)

        # doesn't really matter to check since basic types are being take care of in application.java
        def propertyconstantvalue(property):
            wutype = WuType.where(name=property.datatype)
            if wutype:
                return wutype[0].type.upper() + '_' + Convert.to_constant(property.datatype) + "_" + Convert.to_constant(property.value)
            else:
                return 'ENUM' + '_' + Convert.to_constant(property.datatype) + "_" + Convert.to_constant(property.value)

        # Generate the Java code
        print 'generating', os.path.join(JAVA_OUTPUT_DIR, "WKDeploy.java")
        jinja2_env = Environment(loader=FileSystemLoader([os.path.join(os.path.dirname(__file__), 'jinja_templates')]))
        jinja2_env.filters['nodeinjava'] = nodeinjava
        jinja2_env.filters['wuclassname'] = wuclassname
        jinja2_env.filters['wuclassvirtualclassname'] = wuclassvirtualclassname
        jinja2_env.filters['wuclassconstname'] = wuclassconstname
        jinja2_env.filters['wuclassgenclassname'] = wuclassgenclassname
        jinja2_env.filters['propertyconstname'] = propertyconstname
        jinja2_env.filters['propertyconstantvalue'] = propertyconstantvalue
        output = open(os.path.join(JAVA_OUTPUT_DIR, "WKDeploy.java"), 'w')
        output.write(jinja2_env.get_template('application2.java').render(name=name, changesets=changesets))
        output.close()

    @staticmethod
    def generateTablesXML(name, changesets):
        # Generate the link table and component map xml
        root = ElementTree.Element('wkpftables')
        tree = ElementTree.ElementTree(root)
        links = ElementTree.SubElement(root, 'links')
        components = ElementTree.SubElement(root, 'components')
        for link in changesets.links:
            link_element = ElementTree.SubElement(links, 'link')
            link_element.attrib['fromComponent'] = str(link.from_component_index)
            link_element.attrib['fromProperty'] = str(link.from_property_id)
            link_element.attrib['toComponent'] = str(link.to_component_index)
            link_element.attrib['toProperty'] = str(link.to_property_id)
        component_index = 0
        for component in changesets.components:
            component_element = ElementTree.SubElement(components, 'component')
            component_element.attrib['id'] = str(component_index)
            component_index += 1
            for endpoint in component.instances:
                endpoint_element = ElementTree.SubElement(component_element, 'endpoint')
                endpoint_element.attrib['node'] = str(endpoint.node_id)
                endpoint_element.attrib['port'] = str(endpoint.port_number)
        tree.write(os.path.join(JAVA_OUTPUT_DIR, "WKDeploy.xml"))
