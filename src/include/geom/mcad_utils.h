/*
 * file: mcad_utils.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: miscellaneous routines required by the IGES
 * preprocessor
 *
 * This file is part of libIGES.
 *
 * libIGES is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, If not, see
 * <http://www.gnu.org/licenses/> or write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef MCAD_UTILS_H
#define MCAD_UTILS_H

#include <string>
#include <libigesconf.h>

class MCAD_API MCAD_FILEPATH
{
private:
    std::string* m_fullpath;
    std::string* m_filename;
    std::string* m_basename;
    std::string* m_ext;
    char pathsep;           // directory separator
    char altsep;            // dir separator on complementary system

public:
    MCAD_FILEPATH();
    MCAD_FILEPATH( const char* aPath );
    ~MCAD_FILEPATH();

    /**
     * Function SetPath
     * takes a path and automatically converts the pathname separator;
     * naturally this operation will produce bad results if the actual path
     * contains characters which represent a path separator on another system.
     *
     * @param aPath [in] is the path name to use
     * @return true if the path was successfully converted or is empty
     */
    bool SetPath( const char* aPath );

    /**
     * Function Exists
     * returns true if the full path (directory or file) exists
     */
    bool Exists( void );

    /**
     * Function DirExists
     * returns true if the full path exists and is a directory
     */
    bool DirExists( void );

    /**
     * Function DirExists
     * returns true if the full path exists and is a file
     */
    bool FileExists( void );

    /**
     * Function GetFullPath
     * returns the current full path; this may be a path to a file or a directory
     */
    const char* GetFullPath( void );

    /**
     * Function GetFileName
     * takes the last part of the path spec as a filename and returns it
     */
    const char* GetFileName( void );

    /**
     * Function GetBaseName
     * takes the last part of the path spec as a filename, strips a single
     * extension if there is one, and returns the base name
     */
    const char* GetBaseName( void );

    /**
     * Function GetExtension
     * takes the last part of the path spec as a filename, checks for an
     * extension and returns it
     */
    const char* GetExtension( void );

    bool SetExtension( const char* anExt );

    /**
     * Function GetPathSep
     * returns the path separator native to the operating system
     */
    char GetPathSep( void );

    /**
     * Function Append
     * appends a path separator if the current path is not empty and
     * appends the given string
     */
    bool Append( const char* aSubPath );
};

#endif  // MCAD_UTILS_H
