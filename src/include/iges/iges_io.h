/*
 * file: iges_io.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: internal structures to aid in IGES file I/O.
 *
 * This file is part of libIGES.
 *
 * libIGES is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libIGES.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef IGES_IO_H
#define IGES_IO_H

#include <string>
#include <libigesconf.h>
#include <iges_base.h>

/** Single-line data record as per IGES specification */
struct IGES_RECORD
{
    std::string data;           //< data section (columns 1..72)
    char        section_type;   //< column  73
    int         index;          //< columns 74..80
};


/**
 * Function DEItemToInt
 *  extract an item from a Directory Entry record and convert to an integer;
 *  returns true if an integer was converted or an assignment was made
 *  Note: these functions assume an input string which is a multiple of 8 chars
 *  and fields must be right-aligned.
 *
 * @param input = the Directory Entry Record (or any generic string)
 * @param field = the Field Number within the record (0 .. 9)
 * @param var = the variable to store the result
 * @param defaulted = pointer to a variable with a default value if the variable may be defaulted
 */
bool DEItemToInt( const std::string& input, int field, int& var, int* defaulted = NULL );


/**
 * Function DEItemToStr
 * extract an item from a Directory Entry record and convert to a normal string;
 * return true if a string was extracted. Note: the IGES specification does not
 * preclude trailing spaces within strings in the DE.
 *
 * @param input = the Directory Entry Record
 * @param field = the Field Number within the record (0 .. 9)
 * @param var = the variable to store the result
 */
bool DEItemToStr( const std::string& input, int field, std::string& var );


struct IGES_RECORD;


/**
 * Function ReadIGESRecord
 * read a single line of the IGES file and parse into the record fields; returns
 * true if an input line is successfully read.
 *
 * @param aRecord = pointer to sturcture to store record
 * @param aFile = stream to read input file
 * @param aRefPos = stream position on invocation (useful for error recovery and other things)
 */
bool ReadIGESRecord( IGES_RECORD* aRecord, std::ifstream& aFile, std::streampos* aRefPos = NULL );


/**
 * Function ParseHString
 * parse a free-form Hollerith string and return true on success. The @param idx parameter
 * is updated to point to the start of the next data item.
 *
 * @param data = IGES record
 * @param idx = index to current position within record
 * @param param = variable to store the Hollerith string
 * @param eor = set to true if the record delimeter has been encountered
 * @param pd = IGES Parameter Delimeter
 * @param rd = IGES Record Delimeter
 */
bool ParseHString( const std::string& data, int& idx, std::string& param, bool& eor, char pd, char rd );


/**
 * Function ParseLString
 * parse a free-form LanguageString (a generic string up to the first encountered delimeter )
 * and return true if a string was found. The @param idx parameter is updated to point to the
 * start of the next data item.
 *
 * @param data = IGES record
 * @param idx = index to current position within the record
 * @param param = variable to store the string
 * @param eor = set to true if the record delimeter has been encountered
 * @param pd = IGES Parameter Delimeter
 * @param rd = IGES Record Delimeter
 */
bool ParseLString( const std::string& data, int& idx, std::string& param, bool& eor, char pd, char rd );


/**
 * Function ParseInt
 * parse a free-form string to retrieve an integer; return true if an integer was read
 * or assigned from the default parameter. The @param idx parameter is updated to point
 * to the start of the next data item.
 *
 * @param data = IGES record
 * @param idx = index to current position within the record
 * @param param = variable to store the string
 * @param eor = set to true if the record delimeter has been encountered
 * @param pd = IGES Parameter Delimeter
 * @param rd = IGES Record Delimeter
 * @param idefault = pointer to a variable with a default value if the variable may be defaulted
 */
bool ParseInt(const std::string& data, int& idx, int& param, bool& eor,
	char pd, char rd, int* idefault = NULL);


/**
 * Function ParseReal
 * parse a free-form string to retrieve a floating point number and return true if a float was
 * converted or assigned from the default parameter. The @param idx parameter is updated to point
 * to the start of the next data item.
 *
 * @param data = IGES record
 * @param idx = index to current position within the record
 * @param param = variable to store the string
 * @param eor = set to true if the record delimeter has been encountered
 * @param pd = IGES Parameter Delimeter
 * @param rd = IGES Record Delimeter
 * @param ddefault = pointer to a variable with a default value if the variable may be defaulted
 */
bool ParseReal( const std::string& data, int& idx, double& param, bool& eor,
	char pd, char rd, double* ddefault = NULL );


/**
 * Function FormatDEInt
 * format and right-justify an integer; pad to 8 characters using spaces and return
 * true if successful.
 *
 * @param out = variable to store result
 * @param num = integer to format
 */
bool FormatDEInt( std::string& out, const int num );


/**
 * Function FormatPDREal
 * format a real number as a float or double and tack on a delimeter (may be PD or RD);
 * return true on success.
 *
 * @param tStr = variable to store the formatted string
 * @param var = double to be formatted
 * @param delim = item delimeter (must be current Parameter or Record delimeter)
 * @param minRes = minimum desired numeric resolution
 */
bool FormatPDREal( std::string& tStr, double var, char delim, double minRes );


/**
 * Function GetHConst
 * store the Hollerith constant of a string and return true on success.
 *
 * @param tStr = string whose Hollerith constant is to be determined
 * @param hConst = variable to store the Hollerith constant
 */
bool GetHConst( const std::string& tStr, std::string& hConst );


/**
 * Function AddPDItem
 * add a delimited item to the current record and push complete records into
 * the output string; return true for success. If the last character of
 * @param tStr equals the character @param rd then the PD record is
 * finalized.
 *
 * @param tStr = formatted and delimited data item
 * @param fStr = current record being assembled for output
 * @param fOut = string storing the entire PD section of an entity
 * @param pdIndex = (I/O) current Parameter Data sequence number
 * @param deIndex = Directory Entry sequence number for the entity
 * @param pd = IGES Parameter Delimeter
 * @param rd = IGES Record Delimeter
 */
bool AddPDItem( std::string& tStr, std::string& fStr, std::string& fOut,
                int& pdIndex, int deIndex, char pd, char rd );


/**
 * Function AddSecItem
 * add an item to the Global Section entry; return true on success.
 *
 * @param tStr = formatted and delimited item to be added
 * @param fStr = current record being assembled for output
 * @param fOut = string storing the entire Global Section data
 * @param index = (I/O) sequence index for the Global Section
 * @param pd = IGES Parameter Delimeter
 * @param rd = IGES Record Delimeter
 */
bool AddSecItem( std::string& tStr, std::string& fStr, std::string& fOut,
                 int& index, char pd, char rd );


/**
 * Function AddSecHStr
 * convert a string into a Hollerith string and append to the Global Section data;
 * return true on success.
 *
 * @param tStr = string to be output
 * @param fStr = current record being assembled for output
 * @param fOut = string storing the entire Global Section data
 * @param index = (I/O) sequence index for the Global Section
 * @param pd = IGES Parameter Delimeter
 * @param rd = IGES Record Delimeter
 * @param delim = delimeter to use for the string being output
 */
bool AddSecHStr( const std::string& tStr, std::string& fStr, std::string& fOut,
                 int& index, char pd, char rd, char delim );

#endif  // IGES_IO_H
