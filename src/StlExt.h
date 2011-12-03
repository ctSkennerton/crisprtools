/*
 * StlExt.h
 *
 * Copyright (C) 2011 - Connor Skennerton
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef STLEXT_H
#define STLEXT_H

#include <sstream>
#include <map>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <vector>
#include <iterator>
#include <numeric>



template <class T1, class T2>
void addOrIncrement(std::map<T1, T2> &inMap, T1 &searchThing)
{
    
    if (inMap.find(searchThing) != inMap.end())
    {
        inMap[searchThing] += 1;
    }
    else
    {
        inMap[searchThing] = 1;
    }
}

template <class T>
inline std::string to_string (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template <class T>
bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
    std::istringstream iss(s);
    return !(iss >> f >> t).fail();
}

template <class T1, class T2, class T3>
void mapToVector(std::map<T1, T2>& map, std::vector<T3>& vector) 
{
    
    typename std::map<T1, T2>::iterator iter = map.begin();
    while (iter != map.end()) 
    {
        vector.push_back(iter->first);
        iter++;
    }
}

// templated function to split a string on delimeters and return it in a container of class T
template < class ContainerT >
void tokenize(const std::string& str, ContainerT& tokens, const std::string& delimiters = " ", const bool trimEmpty = false)
{
    std::string::size_type pos, lastPos = 0;
    while(true)
    {
        pos = str.find_first_of(delimiters, lastPos);
        if(pos == std::string::npos)
        {
            pos = str.length();
            
            if(pos != lastPos || !trimEmpty)
                tokens.push_back( typename ContainerT::value_type(str.data()+lastPos, (typename ContainerT::value_type::size_type)pos - lastPos ));
            
            break;
        }
        else
        {
            if(pos != lastPos || !trimEmpty)
                tokens.push_back(typename ContainerT::value_type(str.data()+lastPos, (typename ContainerT::value_type::size_type)pos-lastPos ));
        }
        
        lastPos = pos + 1;
    }
}

template < class ContainerT >
void split(const std::string& str, ContainerT& tokens, const std::string& delimiters = " ", const bool trimEmpty = false)
{
    std::string::size_type pos, lastPos = 0;
    while(true)
    {
        pos = str.find_first_of(delimiters, lastPos);
        if(pos == std::string::npos)
        {
            pos = str.length();
            
            if(pos != lastPos || !trimEmpty)
                tokens.insert( typename ContainerT::value_type(str.data()+lastPos, (typename ContainerT::value_type::size_type)pos - lastPos ));
            
            break;
        }
        else
        {
            if(pos != lastPos || !trimEmpty)
                tokens.insert(typename ContainerT::value_type(str.data()+lastPos, (typename ContainerT::value_type::size_type)pos-lastPos ));
        }
        
        lastPos = pos + 1;
    }
}


#endif