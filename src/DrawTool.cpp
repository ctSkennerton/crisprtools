// DrawTool.cpp
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

#include "DrawTool.h"
#include "CrisprException.h"
#include "CrisprGraph.h"
#include "Utils.h"
#include "config.h"
#include "DrawMain.h"
#include <string.h>
#include <graphviz/gvc.h>
#include <sys/stat.h>


DrawTool::~DrawTool()
{
    graphVector::iterator iter = DT_Graphs.begin();
    while (iter != DT_Graphs.end()) {
        if ((*iter) != NULL) {
            delete *iter;
            *iter = NULL;
        }
        iter++;
    }
    gvFreeContext(DT_Gvc);
}

int DrawTool::processOptions (int argc, char ** argv)
{
	try {
        int c;
        
        while((c = getopt(argc, argv, "hg:c:a:f:o:b:")) != -1)
        {
            switch(c)
            {
                case 'h':
                {
                    drawUsage ();
                    exit(1);
                    break;
                }
                case 'g':
                {
                    generateGroupsFromString (optarg);
                    break;
                }
                    
                case 'o':
                {
                    DT_OutputFile = optarg;
                    if (DT_OutputFile[DT_OutputFile.length() - 1] != '/')
                    {
                        DT_OutputFile += '/';
                    }
                    
                    // check if our output folder exists
                    struct stat file_stats;
                    if (0 != stat(DT_OutputFile.c_str(),&file_stats)) 
                    {
                        recursiveMkdir(DT_OutputFile);
                    }
                    break;
                }
                case 'c':
                {
                    if (!strcmp(optarg, "red-blue") ) {
                        DT_ColourType = RED_BLUE;
                    } else if (!strcmp(optarg, "red-blue-green")) {
                        DT_ColourType = RED_BLUE_GREEN;
                    } else if (!strcmp(optarg, "blue-red")) {
                        DT_ColourType = BLUE_RED;
                    } else if (!strcmp(optarg, "green-blue-red")) {
                        DT_ColourType = GREEN_BLUE_RED;
                    } else {
                        throw crispr::input_exception("Not a known color type");
                    }
                    break;
                } 
                case 'f':
                {
                    DT_OutputFormat = optarg;
                    break;
                }
                case 'a':
                {
                    if (!strcmp(optarg, "dot") || 
                        !strcmp(optarg, "neato") || 
                        !strcmp(optarg, "fdp") || 
                        !strcmp(optarg, "sfdp") ||
                        !strcmp(optarg, "twopi") || 
                        !strcmp(optarg, "circo")) {
                        DT_RenderingAlgorithm = optarg;       
                    } else {
                        throw crispr::input_exception("Not a known Graphviz rendering algorithm");
                    }
                    break;
                }
                case 'b':
                {
                    int i;
                    if (from_string<int>(i, optarg, std::dec)) {
                        if (i > 0) {
                            DT_Bins = i;
                        } else {
                            throw crispr::input_exception("The number of bins of colour must be greater than 0");
                        }
                    } else {
                        throw crispr::runtime_exception(__FILE__, __LINE__, __PRETTY_FUNCTION__,"could not convert string to int");
                    }
                    break;
                }
                default:
                {
                    drawUsage();
                    exit(1);
                    break;
                }
            }
        }

    } catch (crispr::input_exception& e) {
        std::cerr<<e.what()<<std::endl;
        drawUsage();
        exit(1);
    } catch (crispr::runtime_exception& e) {
        std::cerr<<e.what()<<std::endl;
        exit(1);
    }
    return optind;
}

void DrawTool::generateGroupsFromString ( std::string str)
{
	std::set<std::string> vec;
	split ( str, vec, ",");
	DT_Groups = vec;
	DT_Subset = true;
}

int DrawTool::processInputFile(const char * inputFile)
{
    try {
        CrassXML xml_parser;
        xercesc::DOMDocument * input_doc_obj = xml_parser.setFileParser(inputFile);
        xercesc::DOMElement * root_elem = input_doc_obj->getDocumentElement();
        
        if( !root_elem ) throw(crispr::xml_exception(__FILE__, __LINE__, __PRETTY_FUNCTION__, "empty XML document" ));

        // get the children
        xercesc::DOMNodeList * children = root_elem->getChildNodes();
        const  XMLSize_t nodeCount = children->getLength();
        
        // For all nodes, children of "root" in the XML tree.
        for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
            xercesc::DOMNode * currentNode = children->item(xx);
            if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
                // Found node which is an Element. Re-cast node as element
                xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
                
                // is this a group element
                if (xercesc::XMLString::equals(currentElement->getTagName(), xml_parser.getGroup())) {
                    std::string group_id = xml_parser.XMLCH_2_STR(currentElement->getAttribute(xml_parser.getGid()));
                    if (DT_Subset) {
                        
                        // we only want some of the groups look at DT_Groups
                        if (DT_Groups.find(group_id.substr(1)) != DT_Groups.end() ) {
                            parseGroup(currentElement, xml_parser);
                            
                        }
                    } else {
                        parseGroup(currentElement, xml_parser);   
                    }
                }
            }
        }
    } catch (crispr::xml_exception& e) {
        std::cerr<<e.what()<<std::endl;
        return 1;
    } catch (crispr::runtime_exception& e) {
        std::cerr<<e.what()<<std::endl;
    }
    
    return 0;
}
void DrawTool::parseGroup(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // create a new graph object
    crispr::graph * current_graph = new crispr::graph(xmlParser.XMLCH_2_STR(parentNode->getAttribute(xmlParser.getGid())));
    
    // change the max and min coverages back to their original values
    resetInitialLimits();
    
    DT_Graphs.push_back(current_graph);
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getData())) {
                parseData(currentElement, xmlParser, current_graph);
                setColours();
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getAssembly())) {
                parseAssembly(currentElement, xmlParser, current_graph);
            }
        }
    }
    layoutGraph(current_graph->getGraph(), DT_RenderingAlgorithm);
    
    char * file_prefix = xmlParser.XMLCH_2_STR(parentNode->getAttribute(xmlParser.getGid()));
    char * file_name = strcat(strcat(file_prefix, "."), DT_OutputFormat);
    renderGraphToFile(current_graph->getGraph(), DT_OutputFormat, file_name);
    freeLayout(current_graph->getGraph());
}

void DrawTool::parseData(xercesc::DOMElement * parentNode, CrassXML& xmlParser, crispr::graph * currentGraph)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            
            /*if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getDrs())) {
                    // change the direct repeats
                    parseDrs(currentElement, xmlParser);
            } else*/
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getSpacers())) {
                    // change the spacers
                    parseSpacers(currentElement, xmlParser, currentGraph);
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFlankers())) {
                    // change the flankers
                    parseFlankers(currentElement, xmlParser, currentGraph);
            }
        }
    }
}


//void DrawTool::parseDrs(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
//{
//    xercesc::DOMNodeList * children = parentNode->getChildNodes();
//    const  XMLSize_t nodeCount = children->getLength();
//    
//    // For all nodes, children of "root" in the XML tree.
//    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
//        xercesc::DOMNode * currentNode = children->item(xx);
//        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
//            // Found node which is an Element. Re-cast node as element
//            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
//            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getDr())) {
//
//            }
//        }
//    }
//}
void DrawTool::parseSpacers(xercesc::DOMElement * parentNode, CrassXML& xmlParser, crispr::graph * currentGraph)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getSpacer())) {
                Agnode_t * current_graphviz_node = currentGraph->addNode(xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getSpid())));
                currentGraph->setNodeAttribute(current_graphviz_node, "shape", "circle");
                
                if (currentElement->hasAttribute(xmlParser.getCov())) {
                    std::string cov = xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getCov()));
                    double current_cov;
                    if (from_string<double>(current_cov, cov, std::dec)) {
                        recalculateLimits(current_cov);
                        DT_SpacerCoverage[xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getSpid()))] = std::pair<bool, double>(true,current_cov);
                    } else {
                        throw crispr::runtime_exception(__FILE__, __LINE__, __PRETTY_FUNCTION__,"Unable to convert serialized coverage");
                    }
                } else {
                    DT_SpacerCoverage[xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getSpid()))] = std::pair<bool, double>(false,0);
                }
            }
        }
    }
}

void DrawTool::parseFlankers(xercesc::DOMElement * parentNode, CrassXML& xmlParser, crispr::graph * currentGraph)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getSpacer())) {
                Agnode_t * current_graphviz_node = currentGraph->addNode(xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getFlid())));
                currentGraph->setNodeAttribute(current_graphviz_node, "shape", "diamond");
            }
        }
    }
}

void DrawTool::parseAssembly(xercesc::DOMElement * parentNode, CrassXML& xmlParser, crispr::graph * currentGraph)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getContig())) {
                std::string contig_id = xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getCid()));
                parseContig(currentElement, xmlParser,currentGraph, contig_id);
            }
            
        }
    }
    
}

void DrawTool::parseContig(xercesc::DOMElement * parentNode, CrassXML& xmlParser, crispr::graph * currentGraph, std::string& contigId)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // go through all the <cspacer> tags for the contig
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getCspacer())) {
                // get the node
                Agnode_t * current_graphviz_node = currentGraph->addNode(xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getSpid())));
                
                // find the coverage for this guy if it was set
                if (DT_SpacerCoverage[xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getSpid()))].first) {
                    std::string color = DT_Rainbow.getColour(DT_SpacerCoverage[xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getSpid()))].second);
                    
                    // fix things up for Graphviz
                    char * color_for_graphviz = strdup(('#' + color).c_str());

                    // add in the colour attributes
                    currentGraph->setNodeAttribute(current_graphviz_node, "style", "filled");
                    currentGraph->setNodeAttribute(current_graphviz_node, "fillcolor", color_for_graphviz );
                    
                    free(color_for_graphviz);
                }

                parseCSpacer(currentElement, xmlParser,currentGraph,current_graphviz_node,contigId);
            }
        }
    }
}

void DrawTool::parseCSpacer(xercesc::DOMElement * parentNode, CrassXML& xmlParser, crispr::graph * currentGraph, Agnode_t * currentGraphvizNode, std::string& contigId)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
                        
           /* if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getBspacers())) {
                    parseLinkSpacers(currentElement, xmlParser,currentGraph,currentGraphvizNode, REVERSE,contigId);
            } else*/ if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFspacers())) {
                    parseLinkSpacers(currentElement, xmlParser,currentGraph,currentGraphvizNode, FORWARD,contigId);
            /*} else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getBflankers())) {
                    parseLinkFlankers(currentElement, xmlParser,currentGraph,currentGraphvizNode, REVERSE,contigId);
            */} else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFflankers())) {
                    parseLinkFlankers(currentElement, xmlParser,currentGraph,currentGraphvizNode, FORWARD,contigId);
            }
        }
    }
}

void DrawTool::parseLinkSpacers(xercesc::DOMElement * parentNode, CrassXML& xmlParser, crispr::graph * currentGraph, Agnode_t * currentGraphvizNode, EDGE_DIRECTION edgeDirection, std::string& contigId)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            
            Agnode_t * edge_node = currentGraph->addNode(xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getSpid())));
            
            if (edgeDirection == FORWARD) {
                currentGraph->addEdge(currentGraphvizNode, edge_node);
            } else {
                currentGraph->addEdge(edge_node, currentGraphvizNode);
            }
        }
    }
}

void DrawTool::parseLinkFlankers(xercesc::DOMElement * parentNode, CrassXML& xmlParser, crispr::graph * currentGraph, Agnode_t * currentGraphvizNode, EDGE_DIRECTION edgeDirection, std::string& contigId)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            std::string flid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getFlid()));

            Agnode_t * edge_node = currentGraph->addNode(xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getFlid())));
            
            if (edgeDirection == FORWARD) {
                currentGraph->addEdge(currentGraphvizNode, edge_node);
            } else {
                currentGraph->addEdge(edge_node, currentGraphvizNode);
            }

        }
    }
}


int drawMain (int argc, char ** argv)
{
    try {
		DrawTool dt;
		int opt_index = dt.processOptions (argc, argv);
		if (opt_index >= argc) {
			throw crispr::input_exception("No input file provided" );
            
        } else {
			// get cracking and process that file
			return dt.processInputFile(argv[opt_index]);
		}
	} catch(crispr::input_exception& re) {
        std::cerr<<re.what()<<std::endl;
        drawUsage();
        return 1;
    } catch(crispr::exception& ce ) {
		std::cerr<<ce.what()<<std::endl;
		return 1;
	}
}

void drawUsage(void)
{
    std::cout<<PACKAGE_NAME<<" draw [-ghyoaf] file.crispr"<<std::endl;
	std::cout<<"Options:"<<std::endl;
	std::cout<<"-h					print this handy help message"<<std::endl;
    std::cout<<"-o DIR              output file directory  [default: .]" <<std::endl; 
	std::cout<<"-g INT[,n]          a comma separated list of group IDs that you would like to extract data from."<<std::endl;
	std::cout<<"					Note that only the group number is needed, do not use prefixes like 'Group' or 'G', which"<<std::endl;
	std::cout<<"					are sometimes used in file names or in a .crispr file"<<std::endl;
	std::cout<<"-a STRING           The Graphviz layout algorithm to use [default: dot ]"<<std::endl;
    std::cout<<"-f STRING           The output format for the image, equivelent to the -T parameter of Graphviz executables [default: eps]"<<std::endl;
    std::cout<<"-c COLOUR           The colour scale to use for coverage information.  The available choices are:"<<std::endl;
    std::cout<<"                        red-blue"<<std::endl;
    std::cout<<"                        blue-red"<<std::endl;
    std::cout<<"                        red-blue-green"<<std::endl;
    std::cout<<"                        green-blue-red"<<std::endl;
}