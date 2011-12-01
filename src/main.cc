/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cc
 * Copyright (C) Connor Skennerton 2011 <c.skennerton@gmail.com>
 * 
crisprtools is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * crisprtools is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <getopt.h>
#include <cstring>


#include "MergeTool.h"
#include "SplitTool.h"
#include "ExtractTool.h"
#include "FilterTool.h"
#include "SanitiseTool.h"
#include "DrawTool.h"
#include "StatTool.h"
#include "config.h"
void usage (void)
{
	std::cout<<PACKAGE_NAME<<" ("<<PACKAGE_VERSION<<")"<<std::endl;
	std::cout<<PACKAGE_NAME<<" is a set of smal utilities for manipulating .crispr files"<<std::endl;
	std::cout<<"The .crispr file specification is a standard xml based format for describing CRISPRs"<<std::endl;
	std::cout<<"Type "<<PACKAGE_NAME<<" <subcommand> -h for help on each utility";
	std::cout<<"Usage:"<<std::endl;
	std::cout<<PACKAGE_NAME<<" merge [options] file.crispr[1,n]"<<std::endl;
	std::cout<<PACKAGE_NAME<<" split [options] file.crispr[1,n]"<<std::endl;
	std::cout<<PACKAGE_NAME<<" extract [options] file.crispr[1,n]"<<std::endl;
	std::cout<<PACKAGE_NAME<<" filter [options] file.crispr[1,n]"<<std::endl;
	std::cout<<PACKAGE_NAME<<" sanitise [options] file.crispr[1,n]"<<std::endl;
	std::cout<<PACKAGE_NAME<<" draw [options] file.crispr[1,n]"<<std::endl;
	std::cout<<PACKAGE_NAME<<" stat [options] file.crispr[1,n]"<<std::endl;
}

int main(int argc, char ** argv)
{
	if(argc == 1)
	{
		usage();
		return 1;
	}
	else if(!strcmp(argv[1], "merge"))
	{
		return mergeMain(argc -1 , argv + 1);
	}
	else if(!strcmp(argv[1], "split"))
	{
		return splitMain(argc - 1, argv + 1);
	}
	else if(!strcmp(argv[1], "extract"))
	{
		return extractMain(argc - 1 , argv + 1);
	}
	else if(!strcmp(argv[1],"filter"))
	{
		return filterMain(argc - 1, argv + 1);
	}
	else if(!strcmp(argv[1], "sanitise"))
	{
		return sanitiseMain(argc - 1 , argv + 1);
	}
	else if(!strcmp(argv[1], "draw"))
	{
		return drawMain(argc - 1, argv + 1);
	}
	else if(!strcmp(argv[1], "stat"))
	{
		return statMain(argc - 1, argv + 1);
	}
	else
	{
		std::cerr<<"Unknown option: "<<argv[1]<<std::endl;
		usage();
	}
	return 0;
}
