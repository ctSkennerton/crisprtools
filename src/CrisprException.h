/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * crisprtools
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

#ifndef _CRISPREXCEPTION_H_
#define _CRISPREXCEPTION_H_

#include <exception>
#include <string>
namespace crispr {
class CrisprException 
{
public:
	// constructor
	CrisprException(const char * file, int line, const char * function ,const char * message);

	// destructor
	~CrisprException(){}

	virtual std::string what(void)
	{
		return errorMsg;
	}
protected:
	std::string errorMsg;

};

}
#endif // _CRISPREXCEPTION_H_
