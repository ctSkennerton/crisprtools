// SanitiseTool.cpp
//
// Copyright (C) 2011 - Connor Skennerton
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "SanitiseTool.h"
#include "CrisprException.h"
#include "config.h"
#include "CrassXML.h"

#include <iostream>
int SanitiseTool::processOptions (int argc, char ** argv)
{
	int c;
	while((c = getopt(argc, argv, "ahscfdo:")) != -1)
	{
        switch(c)
		{
			case 'a':
            {
                ST_contigs = ST_Repeats = ST_Flank = ST_Spacers = true;
                break;
            }
            case 'h':
			{
				sanitiseUsage();
				exit(1);
				break;
			}
			case 's':
			{
				ST_Spacers = true;
                break;
			}
            case 'o':
            {
                ST_OutputFile = optarg;
                break;
            }
            case 'f':
            {
                ST_Flank = true;
                break;
            }
            case 'd':
            {
                ST_Repeats = true;
                break;
            }
            case 'c':
            {
                ST_contigs = true;
                break;
            }
            default:
            {
                sanitiseUsage();
                exit(1);
                break;
            }
		}
	}
	return optind;
}

int SanitiseTool::processInputFile(const char * inputFile)
{
    try {
        CrassXML xml_parser;
        xercesc::DOMDocument * input_doc_obj = xml_parser.setFileParser(inputFile);
        xercesc::DOMElement * root_elem = input_doc_obj->getDocumentElement();
        
        if (ST_OutputFile.empty()) {
            ST_OutputFile = inputFile;
        }
        
        
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
                    currentElement->setAttribute(xml_parser.STR_2_XMLCH("gid"), xml_parser.STR_2_XMLCH(getNextGroupS()));
                    incrementGroup();
                    
                    // the user wants to change any of these 
                    if (ST_Spacers || ST_Repeats || ST_Flank || ST_contigs) {
                        parseGroup(currentElement, xml_parser);
                    }
                }
            }
            setNextRepeat(1);
            setNextContig(1);
            setNextRepeat(1);
            setNextSpacer(1);
        }
        xml_parser.printDOMToFile(ST_OutputFile, input_doc_obj);
    } catch (crispr::xml_exception& e) {
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
    
    return 0;
}
void SanitiseTool::parseGroup(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getData())) {
                if (ST_Spacers || ST_Repeats || ST_Flank) {
                    parseData(currentElement, xmlParser);
                }
                
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getAssembly())) {
                if (ST_contigs || ST_Spacers || ST_Repeats || ST_Flank) {
                    parseAssembly(currentElement, xmlParser);
                }
            }


        }
    }
    
            

}

void SanitiseTool::parseData(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getDrs())) {
                if (ST_Repeats) {
                    // change the direct repeats
                    parseDrs(currentElement, xmlParser);
                }
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getSpacers())) {
                if (ST_Spacers) {
                    // change the spacers
                    parseSpacers(currentElement, xmlParser);
                }
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFlankers())) {
                if (ST_Flank) {
                    // change the flankers
                    parseFlankers(currentElement, xmlParser);
                }
            }
        }
    }
}
void SanitiseTool::parseDrs(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getDr())) {
                ST_RepeatMap[xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getDrid()))] = getNextRepeatS();
                currentElement->setAttribute(xmlParser.getDrid(), xmlParser.STR_2_XMLCH(getNextRepeatS()));
                incrementRepeat();
            }
        }
    }
}
void SanitiseTool::parseSpacers(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
                ST_SpacerMap[xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getSpid()))] = getNextSpacerS();
                currentElement->setAttribute(xmlParser.getSpid(), xmlParser.STR_2_XMLCH(getNextSpacerS()));
                incrementSpacer();
            }
        }
    }
}

void SanitiseTool::parseFlankers(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
                ST_FlankMap[xmlParser.XMLCH_2_STR(currentElement->getAttribute(xmlParser.getFlid()))] = getNextFlankerS();
                currentElement->setAttribute(xmlParser.getFlid(), xmlParser.STR_2_XMLCH(getNextFlankerS()));
                incrementFlanker();
            }
        }
    }
}

void SanitiseTool::parseAssembly(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
                currentElement->setAttribute(xmlParser.getCid(), xmlParser.STR_2_XMLCH(getNextContigS()));
                incrementContig();
                parseContig(currentElement, xmlParser);
            }
            
        }
    }

}

void SanitiseTool::parseContig(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getCspacer())) {
                if (ST_Spacers) {
                    std::string spid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getSpid()));
                    currentElement->setAttribute(xmlParser.getSpid(), xmlParser.STR_2_XMLCH(ST_SpacerMap[spid]));
                }
                if (ST_Spacers || ST_Repeats || ST_Flank) {
                    parseCSpacer(currentElement, xmlParser);
                }
            }
        }
    }

}

void SanitiseTool::parseCSpacer(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getBspacers())) {
                if (ST_Spacers || ST_Repeats) {
                    parseLinkSpacers(currentElement, xmlParser);
                }
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFspacers())) {
                if (ST_Spacers || ST_Repeats) {
                    parseLinkSpacers(currentElement, xmlParser);
                }
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getBflankers())) {
                if (ST_Flank || ST_Repeats) {
                    parseLinkFlankers(currentElement, xmlParser);
                }
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFflankers())) {
                if (ST_Flank || ST_Repeats) {
                    parseLinkFlankers(currentElement, xmlParser);
                }
            }
        }
    }
}

void SanitiseTool::parseLinkSpacers(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (ST_Spacers) {
                std::string spid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getSpid()));
                currentElement->setAttribute(xmlParser.getSpid(), xmlParser.STR_2_XMLCH( ST_SpacerMap[spid]));
            }
            if (ST_Repeats) {
                std::string drid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getDrid()));
                currentElement->setAttribute(xmlParser.getDrid(), xmlParser.STR_2_XMLCH( ST_RepeatMap[drid]));
            }
        }
    }
}

void SanitiseTool::parseLinkFlankers(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (ST_Flank) {
                std::string flid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getFlid()));
                currentElement->setAttribute(xmlParser.getFlid(), xmlParser.STR_2_XMLCH( ST_FlankMap[flid]));
            }

            if (ST_Repeats) {
                std::string drid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getDrid()));
                currentElement->setAttribute(xmlParser.getDrid(), xmlParser.STR_2_XMLCH( ST_RepeatMap[drid]));
            }
        }
    }
}

int sanitiseMain (int argc, char ** argv)
{
    try {
		SanitiseTool st;
		int opt_index = st.processOptions (argc, argv);
		if (opt_index >= argc) {
			throw crispr::input_exception("No input file provided" );
            
        } else {
			// get cracking and process that file
			return st.processInputFile(argv[opt_index]);
		}
	} catch(crispr::input_exception& re) {
        std::cerr<<re.what()<<std::endl;
        sanitiseUsage();
        return 1;
    } catch(crispr::exception& ce ) {
		std::cerr<<ce.what()<<std::endl;
		return 1;
	}
}

void sanitiseUsage(void)
{
    std::cout<<PACKAGE_NAME<<" sanitise [-ohcsdf] file.crispr"<<std::endl;
	std::cout<<"Options:"<<std::endl;
	std::cout<<"-h					Print this handy help message"<<std::endl;
    std::cout<<"-o FILE             Output file name, creates a sanitised copy of the input file  [default: sanitise input file inplace]" <<std::endl; 
	std::cout<<"-s					Sanitise the spacers "<<std::endl;
	std::cout<<"-d					Sanitise the direct repeats "<<std::endl;
	std::cout<<"-f					Sanitise the flanking sequences "<<std::endl;
	std::cout<<"-c					Sanitise the contigs "<<std::endl;

    

}