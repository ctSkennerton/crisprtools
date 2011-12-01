// ExtractTool.cpp
//
// Copyright (C) 2011 - Connor Skennerton
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
#include "ExtractTool.h"
#include "StlExt.h"
#include "config.h"
#include "CrisprException.h"
#include "CrassXML.h"
#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

ExtractTool::ExtractTool (void)
{
	ET_Spacer = false;
	ET_DirectRepeat = false;
	ET_Flanker = false;
	ET_SplitGroup = false;
	ET_SplitType = false;
	ET_Subset = false;
}

ExtractTool::~ExtractTool (void)
{}

void ExtractTool::generateGroupsFromString ( std::string str)
{
	std::set<std::string> vec;
	split ( str, vec, ",");
	ET_Group = vec;
	ET_Subset = true;
}

int ExtractTool::processOptions (int argc, char ** argv)
{
	int c;
    int index;
    static struct option long_options [] = {       
        {"help", required_argument, NULL, 'h'}
    };
	while((c = getopt_long(argc, argv, "hg:sdfxy", long_options, &index)) != -1)
	{
        switch(c)
		{
			case 'h':
			{
				extractUsage ();
				exit(1);
				break;
			}
			case 'g':
			{
				generateGroupsFromString (optarg);
				break;
			}
			case 's':
			{
				ET_Spacer = true;
                break;
			}
			case 'd':
			{
				ET_DirectRepeat = true;
                break;
			}
			case 'f':
			{
				ET_Flanker = true;
                break;
			}
			case 'x':
			{
				ET_SplitGroup = true;
                break;
			}
			case 'y':
			{
				ET_SplitType = true;
                break;
			}
            default:
            {
                extractUsage();
                exit(1);
                break;
            }
		}
	}
	return optind;
}

int ExtractTool::processInputFile(const char * inputFile)
{
    // open the file
    try {
        CrassXML xml_obj;
        xml_obj.setFileParser(inputFile);
        
        // go through the children of the root element
        
    } catch (crispr::xml_exception& xe) {
        std::cerr<<xe.what()<<std::endl;
        return 1;
    }
    // go through the groups
    
    // go to the child nodes based on the options
    
    // print to output files
    return 0;
}

void ExtractTool::parseWantedGroups(void)
{
    if (ET_Subset) {
        // we only want some of the groups look at ET_Groups
    } else {
        // parse all of the groups
    }
}

int extractMain (int argc, char ** argv)
{
    try {
		ExtractTool et;
		int opt_index = et.processOptions (argc, argv);
		if (opt_index >= argc) {
			throw crispr::input_exception("No input file provided" );
		
        } else {
			// get cracking and process that file
			return et.processInputFile(argv[opt_index]);
		}
	} catch(crispr::input_exception& re) {
        std::cerr<<re.what()<<std::endl;
        extractUsage();
        return 1;
    } catch(crispr::exception& ce ) {
		std::cerr<<ce.what()<<std::endl;
		return 1;
	}

}


void extractUsage (void)
{
	std::cout<<PACKAGE_NAME<<" extract [-ghyxsdf] file.crispr"<<std::endl;
	std::cout<<"Options:"<<std::endl;
	std::cout<<"-h					print this handy help message"<<std::endl;
	std::cout<<"-g INT[,n]          a comma separated list of group IDs that you would like to extract data from."<<std::endl;
	std::cout<<"					Note that only the group number is needed, do not use prefixes like 'Group' or 'G', which"<<std::endl;
	std::cout<<"					are sometimes used in file names or in a .crispr file"<<std::endl;
	std::cout<<"-s					Extract the spacers of the listed group"<<std::endl;
	std::cout<<"-d					Extract the direct repeats of the listed group"<<std::endl;
	std::cout<<"-f					Extract the flanking sequences of the listed group"<<std::endl;
	std::cout<<"-x					Split the results into different files for each group.  If multiple types are set i.e. -sd"<<std::endl;
	std::cout<<"					then both the spacers and direct repeats from each group will be in the one file"<<std::endl;
	std::cout<<"-y					Split the results into different files for each type of sequence from all selected groups."<<std::endl;
	std::cout<<"					Only has an effect if multiple types are set."<<std::endl;

}
				
				