# vim: ts=4 sw=4
#!/usr/bin/python

import os, sys
from xml.etree import ElementTree
sys.path.append(os.path.abspath(os.path.join(os.path.abspath(os.path.dirname(__file__)), "../..")))
from jinja2 import Template, Environment, FileSystemLoader
from struct import pack

from wkpf.models import *

from configuration import *
from wkpf.util import *


class Generator:
    @staticmethod
    def generate(name, changesets):
        print '[generator] generate Java App'
        Generator.generateJavaApplication(name, changesets)
        print '[generator] generate Table XML'
        Generator.generateTablesXML(name, changesets)

    @staticmethod
    def generateJavaApplication(name, changesets):
        # i is the number to be transform into byte array, n is the number of bytes to use (little endian)
        def bytestring(i, n): 
            return ['(byte)' + str(ord(b)) for b in pack("H", i)][:n]

        def nodeinjava(node):
            return str(node.id)

        def wuobjectinjava(wuobject):
            return ', '.join([str(wuobject.wunode().id),
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

        def wuclassid(wuclass):
            return str(wuclass.id)

        def wuclassgenclassname(wuclass):
            return "GENERATEDVirtual" + Convert.to_java(wuclass.name) + "WuObject"

        def propertyconstname(property):
            print 'propertyconstname'
            return "PROPERTY_" + Convert.to_constant(property.wuobject().wuclass().wuclassdef().name) + "_" + Convert.to_constant(property.wupropertydef().name)

        # doesn't really matter to check since basic types are being take care of in application.java
        def propertyconstantvalue(property):
            wutype = WuTypeDef.where(name=property.datatype)
            if wutype:
                return wutype[0].type.upper() + '_' + Convert.to_constant(property.datatype) + "_" + Convert.to_constant(property.value)
            else:
                return 'ENUM' + '_' + Convert.to_constant(property.datatype) + "_" + Convert.to_constant(property.value)

        def generateProperties(wuobject_properties, component_properties):
            properties = wuobject_properties

            for property in properties:
                if property.wupropertydef().name in component_properties:
                    property.value = component_properties[property.wupropertydef().name]
            return properties

        # Generate the Java code
        print '[generator] generating', os.path.join(JAVA_OUTPUT_DIR, "WKDeploy.java")
        jinja2_env = Environment(loader=FileSystemLoader([os.path.join(os.path.dirname(__file__), 'jinja_templates')]))
        jinja2_env.filters['nodeinjava'] = nodeinjava
        jinja2_env.filters['wuclassname'] = wuclassname
        jinja2_env.filters['wuclassvirtualclassname'] = wuclassvirtualclassname
        jinja2_env.filters['wuclassid'] = wuclassid
        jinja2_env.filters['wuclassgenclassname'] = wuclassgenclassname
        jinja2_env.filters['propertyconstname'] = propertyconstname
        jinja2_env.filters['propertyconstantvalue'] = propertyconstantvalue
        jinja2_env.filters['generateProperties'] = generateProperties
        jinja2_env.add_extension('jinja2.ext.do')
        output = open(os.path.join(JAVA_OUTPUT_DIR, "WKDeploy.java"), 'w')
        output.write(jinja2_env.get_template('application2.java').render(name=name, changesets=changesets))
        output.close()

    @staticmethod
    def generateTablesXML(name, changesets):
        def generateProperties(wuobject_properties, component_properties):
            properties = wuobject_properties

            for property in properties:
                if property.wupropertydef().name in component_properties:
                    property.value = component_properties[property.wupropertydef().name]
            return [p for p in properties if p.wupropertydef().access!='readonly']

        # TODO: this should be in a higher level place somewhere.
        # TODO: is 'int' really a datatype? It was used in application2.java so keeping it here for now. should check if it can go later.
        datatype_sizes = {'boolean': 1, 'int': 2, 'short': 2, 'refresh_rate': 2}

        # Generate the link table and component map xml
        root = ElementTree.Element('wkpftables')
        tree = ElementTree.ElementTree(root)
        links = ElementTree.SubElement(root, 'links')
        components = ElementTree.SubElement(root, 'components')
        initvalues = ElementTree.SubElement(root, 'initvalues')
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
            component_wuclass = WuClassDef.find(name=component.type)
            component_element.attrib['wuclassId'] = str(component_wuclass.id)
            component_index += 1
            for endpoint in component.instances:
                endpoint_element = ElementTree.SubElement(component_element, 'endpoint')
                endpoint_element.attrib['node'] = str(endpoint.wunode().id)
                endpoint_element.attrib['port'] = str(endpoint.port_number)
        component_index = 0
        for component in changesets.components:
            wuobject = component.instances[0]
            for property in generateProperties(wuobject.wuproperties(), component.properties_with_default_values):
                initvalue = ElementTree.SubElement(initvalues, 'initvalue')
                initvalue.attrib['componentId'] = str(component_index)
                initvalue.attrib['propertyNumber'] = str(property.wupropertydef().number)
                if property.datatype in datatype_sizes: # Basic type
                    initvalue.attrib['valueSize'] = str(datatype_sizes[property.datatype])
                else: # Enum
                    initvalue.attrib['valueSize'] = '2'
                if property.datatype == 'short' or property.datatype == 'int' or property.datatype == 'refresh_rate':
                    initvalue.attrib['value'] = str(property.value)
                elif property.datatype == 'boolean':
                    initvalue.attrib['value'] = '1' if property.value == 'true'else '0'
                else: # Enum
                    enumtype = WuTypeDef.find(name=property.datatype)
                    enumvalues = [wuvalue.value.upper() for wuvalue in enumtype.wuvalues()]
                    print enumtype
                    print enumvalues
                    initvalue.attrib['value'] = str(enumvalues.index(property.value.upper())) # Translate the string representation to an integer
            component_index += 1
        tree.write(os.path.join(JAVA_OUTPUT_DIR, "WKDeploy.xml"))
