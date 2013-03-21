/*
 * DJArchiveTask.java
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

package org.csiro.darjeeling.djarchive.ant;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;

/**
 * 
 * Ant task that converts a binary file to a c-style constant array. It can add user-specified keywords such 
 * as 'PROGMEM' (required for AVR targets), or for putting the array in specific segments (i.e. FAR on msp430x). 
 * When the 'PROGMEM' keyword is used, the appropriate avr include file will be automatically included.
 * 
 * The resulting array will be declared constant using the 'const' keyword, which usually has an effect on where
 * the data will be placed by the linker. This can be omitted using 'omitconst=true' in the ant build file. 
 * 
 * @author Niels Brouwers
 *
 * NR 20130224: Replacing the size from a separate variable to the first 4 bytes of the C array.
 *              This will make wireless reprogramming easier since we can just upload a whole new
 *              array, and will free a little bit of memory as well.
 *              Also adding an extra parameter "arraysize" so we can easily reserve space for
 *              uploading larger archives (will only be useful for the app archive). When it's
 *              empty we'll create an array as large as the archive. When it's set, but smaller
 *              than the archive, throw an error.
 */
public class DJArchiveTask extends Task
{
	private final static byte FILETYPE_LIB_INFUSION = 0;
	private final static byte FILETYPE_APP_INFUSION = 1;
	private final static byte FILETYPE_WKPF_TABLE = 2;

	// Source, destination files and output array name.
	private String dest;
	private String libInfusions;
	private String appInfusions;
	private String wkpfTables;

	/**
	 * Ant execute entry point.
	 */
	public void execute()
	{
		// make sure properties are set
		if (dest==null) throw new BuildException("Destination file name not set");

		try {
			System.out.println("Creating archive '" + dest + "'");
			FileOutputStream fout = new FileOutputStream(dest);
			appendFiles(fout, libInfusions, FILETYPE_LIB_INFUSION);
			appendFiles(fout, appInfusions, FILETYPE_APP_INFUSION);
			appendFiles(fout, wkpfTables, FILETYPE_WKPF_TABLE);
			// Close the file with a 00 00
			fout.write((byte)0);
			fout.write((byte)0);
			fout.close();
		} catch (IOException ioex) {
			throw new org.apache.tools.ant.BuildException("IO error while writing: " + dest);
		}
		
	}

	public void appendFiles(FileOutputStream fout, String files, byte filetype) throws IOException {
		if (files == null)
			return;
		String[] filenames = files.split(" ");
		for (String filename: filenames) {
			System.out.println("Adding '" + filename + "'");
			byte[] bytes;
			// open input file
			try {
				FileInputStream fileInput = new FileInputStream(filename);
				bytes = new byte[fileInput.available()];
				fileInput.read(bytes);
				fileInput.close();
			} catch (FileNotFoundException fnfex)
			{
				throw new org.apache.tools.ant.BuildException("File not found: " + filename);
			} catch (IOException ioex) {
				throw new org.apache.tools.ant.BuildException("IO error while reading: " + filename);
			}
			// First write the length of each file
			fout.write((byte)((bytes.length>>0)%256));
			fout.write((byte)((bytes.length>>8)%256));
			// Then write the file type
			fout.write(filetype);
			// Then write the file
			fout.write(bytes);
		}
	}
	
	/**
	 * Sets the list of library infusions to include in the archive.
	 * @param libInfusions list of library infusions
	 */
	public void setLibInfusions(String libInfusions)
	{
		this.libInfusions = libInfusions;
	}

	/**
	 * Sets the list of application infusions to include in the archive.
	 * @param appInfusions list of application infusions
	 */
	public void setAppInfusions(String appInfusions)
	{
		this.appInfusions = appInfusions;
	}

	/**
	 * Sets the list of wkpf tables to include in the archive.
	 * @param wkpfTables list of wkpf tables
	 */
	public void setwkpfTables(String wkpfTables)
	{
		this.wkpfTables = wkpfTables;
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
