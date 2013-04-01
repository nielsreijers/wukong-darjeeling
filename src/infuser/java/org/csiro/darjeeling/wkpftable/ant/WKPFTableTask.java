/*
 * WKPFTableTask.java
 * 
 * Copyright (c) 2008-2010 CSIRO, Delft University of Technology.
 * 
 * This file is part of Darjeeling.
 * 
 * Darjeeling is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Darjeeling is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with Darjeeling.  If not, see <http://www.gnu.org/licenses/>.
 */ 

package org.csiro.darjeeling.wkpftable.ant;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;


import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;

/**
 * 
 * Ant task that converts a text file containing the WuKong link table and component map to a
 * binary file in the format wkpf_links.c expects.
 */
public class WKPFTableTask extends Task
{
	// Number of output values per line
	private final static int LINESIZE = 16;
	
	// Source, destination files and output array name.
	private String src, dest, arrayName;
	// Desired size of the array. Defaults to the size of the archive if 0. Error if !=0 but < archive size.
	private int arraysize;

	// Keyword list. 
	private ArrayList<String> keywords = new ArrayList<String>();
	
	// If true, the array will be made constant using the 'const' keyword. 
	private boolean constKeyword = true;
	
	/**
	 * Ant execute entry point.
	 */
	public void execute()
	{
		int number_of_links;
		int number_of_components;

		ArrayList<Byte> links_bytes = new ArrayList<Byte>();
		ArrayList<Integer> components_offsets = new ArrayList<Integer>();
		ArrayList<Byte> components_bytes = new ArrayList<Byte>();
		ArrayList<Byte> total_bytes = new ArrayList<Byte>();
		
		// make sure properties are set
		if (src==null) throw new BuildException("Source file name not set");
		if (dest==null) throw new BuildException("Destination file name not set");

        // Check if the file needs to be regenerated
		long srcDate =new File(src).lastModified();
        long destDate=new File(dest).lastModified();
        if(destDate > srcDate) {
            log("This file is up to date",Project.MSG_VERBOSE);
            return ;
        }

        try {
			File fXmlFile = new File(src);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(fXmlFile);
			doc.getDocumentElement().normalize();

			NodeList links = ((Element)doc.getElementsByTagName("links").item(0)).getElementsByTagName("link");
			NodeList components = ((Element)doc.getElementsByTagName("components").item(0)).getElementsByTagName("component");

			for (int i=0; i<links.getLength(); i++) {
				Node node = links.item(i);
				if (node.getNodeType() == Node.ELEMENT_NODE) {
					Element link = (Element)node;
					// six bytes per link
					links_bytes.add((byte)(Short.parseShort(link.getAttribute("fromComponent")) % 256));
					links_bytes.add((byte)(Short.parseShort(link.getAttribute("fromComponent")) / 256));
					links_bytes.add(Byte.parseByte(link.getAttribute("fromProperty")));
					links_bytes.add((byte)(Short.parseShort(link.getAttribute("toComponent")) % 256));
					links_bytes.add((byte)(Short.parseShort(link.getAttribute("toComponent")) / 256));
					links_bytes.add(Byte.parseByte(link.getAttribute("toProperty")));
				}
			}

			int expected_component_id = 0;
			for (int i=0; i<components.getLength(); i++) {
				Node node = components.item(i);
				if (node.getNodeType() == Node.ELEMENT_NODE) {
					Element component = (Element)node;
					// Check if this is the component we're expecting.
					// The XML should contain a continuous range starting at 0.
					int component_id = Integer.parseInt(component.getAttribute("id"));
					if (component_id != expected_component_id)
						throw new org.apache.tools.ant.BuildException("Unexpected component is: " + component_id + " expected: " + expected_component_id + ". Component ids should be a continuous range starting at 0.");
					components_offsets.add(components_bytes.size()); // Offset of this component

					NodeList endpoints = component.getElementsByTagName("endpoint");
					components_bytes.add((byte)endpoints.getLength()); // Number of endpoints
					components_bytes.add((byte)(Short.parseShort(component.getAttribute("wuclassId")) % 256));
					components_bytes.add((byte)(Short.parseShort(component.getAttribute("wuclassId")) / 256));
					for (int j=0; j<endpoints.getLength(); j++) {
						Node node2 = endpoints.item(j);
						if (node2.getNodeType() == Node.ELEMENT_NODE) {
							Element endpoint = (Element)node2;
							components_bytes.add(Byte.parseByte(endpoint.getAttribute("node")));
							components_bytes.add(Byte.parseByte(endpoint.getAttribute("port")));
						}
					}
					expected_component_id++;
				}
			}


			//// Link table
			// two bytes: number of links, little endian
			total_bytes.add((byte)(links.getLength() % 256));
			total_bytes.add((byte)(links.getLength() / 256));
			// the link table
			total_bytes.addAll(links_bytes);
			//// Component map
			// two bytes: number of components
			total_bytes.add((byte)(components.getLength() % 256));
			total_bytes.add((byte)(components.getLength() / 256));
			// offset table containing an offset from the beginning of the component map
			for(int i=0; i<components_offsets.size(); i++) {
				int offset = components_offsets.get(i);
				offset += 2; // for the number of components
				offset += components_offsets.size()*2; // for the offset table
				// offset is now a true offset from the beginning of the components table
				total_bytes.add((byte)(offset % 256));
				total_bytes.add((byte)(offset / 256));
			}
			// The actual component map
			total_bytes.addAll(components_bytes);
		} catch (FileNotFoundException fnfex) {
			throw new org.apache.tools.ant.BuildException("File not found: " + src);
		} catch (IOException ioex) {
			throw new org.apache.tools.ant.BuildException("IO error while reading: " + src);
		} catch (Exception e) {
			e.printStackTrace();
		}

		// write C-style array definition
		try {
			FileOutputStream fout = new FileOutputStream(dest);
			for (int i=0; i<total_bytes.size(); i++)
				fout.write((byte)total_bytes.get(i));
			fout.close();
		} catch (IOException ioex) {
			throw new org.apache.tools.ant.BuildException("IO error while writing: " + src);
		}
	}
	
	/**
	 * Sets the source file name.
	 * @param src source file name
	 */
	public void setSrc(String src)
	{
		this.src = src;
	}

	/**
	 * Sets the destination file name.
	 * @param dest destination file name
	 */
	public void setDest(String dest)
	{
		this.dest = dest;
	}
}
