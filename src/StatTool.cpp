// StatTool.cpp
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

#include "StatTool.h"
#include "StlExt.h"
#include "config.h"
#include "CrisprException.h"
#include <iostream>

StatTool::~StatTool()
{
    std::vector<StatManager *>::iterator iter = begin();
    while (iter != end()) {
        delete *iter;
        iter++;
    }
}

void StatTool::generateGroupsFromString ( std::string str)
{
	std::set<std::string> vec;
	split ( str, vec, ",");
	ST_Groups = vec;
	ST_Subset = true;
}
int StatTool::processOptions (int argc, char ** argv)
{
	int c;
	while((c = getopt(argc, argv, "pahg:o:")) != -1)
	{
        switch(c)
		{
			case 'a':
            {
                ST_AssemblyStats = true;
                break;
            }
            case 'h':
			{
				statUsage();
				exit(1);
				break;
			}
            case 'o':
            {
                ST_OutputFileName = optarg;
                break;
            }
            case 'p':
            {
                ST_Pretty = true;
                break;
            }
            case 'g':
            {
                generateGroupsFromString(optarg);
                break;
            }
            default:
            {
                statUsage();
                exit(1);
                break;
            }
		}
	}
	return optind;
}

int StatTool::processInputFile(const char * inputFile)
{
    try {
        CrassXML xml_parser;
        xercesc::DOMDocument * input_doc_obj = xml_parser.setFileParser(inputFile);
        xercesc::DOMElement * root_elem = input_doc_obj->getDocumentElement();

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
                    if (ST_Subset) {
                        // we only want some of the groups look at DT_Groups
                        if (ST_Groups.find(group_id.substr(1)) != ST_Groups.end() ) {
                            parseGroup(currentElement, xml_parser);
                        }
                    } else {
                        parseGroup(currentElement, xml_parser);   
                    }
                }
            }
        }
        
        // go through each of the groups and print out a pretty picture
        std::vector<StatManager *>::iterator iter = this->begin();
        while (iter != this->end()) {
            prettyPrint(*iter);
            iter++;
        }
        
        
    } catch (crispr::xml_exception& e) {
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
    
    return 0;
}
void StatTool::parseGroup(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
{
    
    StatManager * sm = new StatManager();
    ST_StatsVec.push_back(sm);
    std::string concensusRepeat = TranscodeStr(parentNode->getAttribute(xmlParser.getDrseq())).cForm();
    sm->setConcensus(concensusRepeat);
    
    std::string gid = TranscodeStr(parentNode->getAttribute(xmlParser.getGid())).cForm();
    sm->setGid(gid);
    
    
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    
    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getData())) {
                parseData(currentElement, xmlParser, sm);
                
            } /*else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getAssembly())) {
                if (ST_AssemblyStats) {
                    parseAssembly(currentElement, xmlParser);
                }
            }*/
            
            
        }
    }
}

void StatTool::parseData(xercesc::DOMElement * parentNode, CrassXML& xmlParser, StatManager * statManager)
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
                // change the direct repeats
                parseDrs(currentElement, xmlParser, statManager);
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getSpacers())) {
                // change the spacers
                parseSpacers(currentElement, xmlParser, statManager);
            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFlankers())) {
                // change the flankers
                parseFlankers(currentElement, xmlParser, statManager);
            }
        }
    }
}
void StatTool::parseDrs(xercesc::DOMElement * parentNode, CrassXML& xmlParser, StatManager * statManager)
{
    xercesc::DOMNodeList * children = parentNode->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        xercesc::DOMNode * currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  currentNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            xercesc::DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getDr())) {
                std::string repeat = TranscodeStr(currentElement->getAttribute(xmlParser.getDrid())).cForm();
                statManager->addRepLenVec(static_cast<int>(repeat.length()));
                statManager->incrementRpeatCount();
            }
        }
    }
    
}
void StatTool::parseSpacers(xercesc::DOMElement * parentNode, CrassXML& xmlParser, StatManager * statManager)
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
                std::string spacer = TranscodeStr(currentElement->getAttribute(xmlParser.getSpid())).cForm();
                std::string cov = TranscodeStr(currentElement->getAttribute(xmlParser.getCov())).cForm();
                statManager->addSpLenVec(static_cast<int>(spacer.length()));
                int cov_int;
                from_string(cov_int, cov, std::dec);
                statManager->addSpCovVec(cov_int);
                statManager->incrementSpacerCount();
            }
        }
    }
}

void StatTool::parseFlankers(xercesc::DOMElement * parentNode, CrassXML& xmlParser, StatManager * statManager)
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
                std::string flanker = TranscodeStr(currentElement->getAttribute(xmlParser.getFlid())).cForm();
                statManager->addFlLenVec(static_cast<int>(flanker.length()));
                statManager->incrementFlankerCount();
            }
        }
    }
}

//void StatTool::parseAssembly(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
//            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getContig())) {
//                currentElement->setAttribute(xmlParser.getCid(), xmlParser.STR_2_XMLCH(getNextContigS()));
//                incrementContig();
//                parseContig(currentElement, xmlParser);
//            }
//            
//        }
//    }
//    
//}
//
//void StatTool::parseContig(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
//            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getCspacer())) {
//                if (ST_Spacers) {
//                    std::string spid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getSpid()));
//                    currentElement->setAttribute(xmlParser.getSpid(), xmlParser.STR_2_XMLCH(ST_SpacerMap[spid]));
//                }
//                if (ST_Spacers || ST_Repeats || ST_Flank) {
//                    parseCSpacer(currentElement, xmlParser);
//                }
//            }
//        }
//    }
//    
//}
//
//void StatTool::parseCSpacer(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
//            if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getBspacers())) {
//                if (ST_Spacers || ST_Repeats) {
//                    parseLinkSpacers(currentElement, xmlParser);
//                }
//            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFspacers())) {
//                if (ST_Spacers || ST_Repeats) {
//                    parseLinkSpacers(currentElement, xmlParser);
//                }
//            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getBflankers())) {
//                if (ST_Flank || ST_Repeats) {
//                    parseLinkFlankers(currentElement, xmlParser);
//                }
//            } else if (xercesc::XMLString::equals(currentElement->getTagName(), xmlParser.getFflankers())) {
//                if (ST_Flank || ST_Repeats) {
//                    parseLinkFlankers(currentElement, xmlParser);
//                }
//            }
//        }
//    }
//}
//
//void StatTool::parseLinkSpacers(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
//            if (ST_Spacers) {
//                std::string spid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getSpid()));
//                currentElement->setAttribute(xmlParser.getSpid(), xmlParser.STR_2_XMLCH( ST_SpacerMap[spid]));
//            }
//            if (ST_Repeats) {
//                std::string drid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getDrid()));
//                currentElement->setAttribute(xmlParser.getDrid(), xmlParser.STR_2_XMLCH( ST_RepeatMap[drid]));
//            }
//        }
//    }
//}
//
//void StatTool::parseLinkFlankers(xercesc::DOMElement * parentNode, CrassXML& xmlParser)
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
//            if (ST_Flank) {
//                std::string flid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getFlid()));
//                currentElement->setAttribute(xmlParser.getFlid(), xmlParser.STR_2_XMLCH( ST_FlankMap[flid]));
//            }
//            
//            if (ST_Repeats) {
//                std::string drid = xmlParser.XMLCH_2_STR( currentElement->getAttribute(xmlParser.getDrid()));
//                currentElement->setAttribute(xmlParser.getDrid(), xmlParser.STR_2_XMLCH( ST_RepeatMap[drid]));
//            }
//        }
//    }
//}
void StatTool::prettyPrint(StatManager * sm)
{
    std::cout<<sm->getGid()<<" | "<<sm->getConcensus()<<" | ";
    int i = 0;
    for ( i = 0; i < sm->getRpeatCount(); ++i) {
        std::cout<<REPEAT_CHAR;
    }
    for ( i = 0; i < sm->getSpacerCount() ; ++i) {
        std::cout<<SPACER_CHAR;
    }
    for ( i = 0; i < sm->getFlankerCount(); ++i) {
        std::cout<<FLANKER_CHAR;
    }
    std::cout<<"{ "<<sm->getRpeatCount()<< " " <<sm->getSpacerCount()<<" "<<sm->getFlankerCount()<<" } "<<std::endl;
}


int statMain (int argc, char ** argv)
{
    try {
		StatTool st;
		int opt_index = st.processOptions (argc, argv);
		if (opt_index >= argc) {
			throw crispr::input_exception("No input file provided" );
            
        } else {
			// get cracking and process that file
			return st.processInputFile(argv[opt_index]);
		}
	} catch(crispr::input_exception& re) {
        std::cerr<<re.what()<<std::endl;
        statUsage();
        return 1;
    } catch(crispr::exception& ce ) {
		std::cerr<<ce.what()<<std::endl;
		return 1;
	}
    return 0;
}
void statUsage(void)
{
    std::cout<<PACKAGE_NAME<<" stat [-gh] file.crispr"<<std::endl;
	std::cout<<"Options:"<<std::endl;
    std::cout<<"-h					print this handy help message"<<std::endl;
	std::cout<<"-g INT[,n]          a comma separated list of group IDs that you would like to see stats for."<<std::endl;}
