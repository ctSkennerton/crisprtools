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
#include <graphviz/gvc.h>


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
	int c;

	while((c = getopt(argc, argv, "hg:sdfxyo:")) != -1)
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
    }
    
    return 0;
}
void DrawTool::parseGroup(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // create a new graph object
    crispr::graph * current_graph = new crispr::graph(xmlParser.XMLCH_2_STR(parentNode->getAttribute(xmlParser.getGid())));
    
    DT_Graphs.push_back(current_graph);
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            /*if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getData())) {
                    parseData(currentElement, xmlParser);
                
            } else */
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getAssembly())) {
                    parseAssembly(currentElement, xmlParser, current_graph);
            }
        }
    }
    layoutGraph(current_graph->getGraph(), "dot");
    renderGraphToFile(current_graph->getGraph(), "eps", xmlParser.XMLCH_2_STR(parentNode->getAttribute(xmlParser.getGid())));
    freeLayout(current_graph->getGraph());
}

//void DrawTool::parseData(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
//            
//            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getDrs())) {
//                    // change the direct repeats
//                    parseDrs(currentElement, xmlParser);
//            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getSpacers())) {
//                    // change the spacers
//                    parseSpacers(currentElement, xmlParser);
//            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFlankers())) {
//                    // change the flankers
//                    parseFlankers(currentElement, xmlParser);
//            }
//        }
//    }
//}
//
//
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
//void DrawTool::parseSpacers(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
//            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getSpacer())) {
//
//            }
//        }
//    }
//}
//
//void DrawTool::parseFlankers(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
//            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getSpacer())) {
//
//            }
//        }
//    }
//}

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
                // add in a new node to our current graph
                Agnode_t * current_graphviz_node = currentGraph->addNode(xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getSpid())));

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
//    GVC_t * gvc = gvContext();
//    
//    crispr::graph graph = crispr::graph("g");
//    
//    Agnode_t * n = graph.addNode("n");
//    
//    Agnode_t * m = graph.addNode("m");
//    
//    Agedge_t * e = graph.addEdge(n, m);
//    
//    graph.setNodeAttribute(n, "color", "red");
//    
//    Agraph_t * g = graph.getGraph();
//    gvLayout(gvc, g, "dot");
//    /* Write the graph according to -T and -o options */
//    gvRenderFilename(gvc, g, "eps", "test.eps");
//    /* Free layout data */
//    gvFreeLayout(gvc, g);
//    /* Free graph structures */
//    	agclose(g);
//    /* close output file, free context, and return number of errors */
//    return (gvFreeContext(gvc));
}

void drawUsage(void)
{
    std::cout<<""<<std::endl;
}