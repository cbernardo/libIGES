/*
 * file: mcad_utils.cpp
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

// Note: Characters restricted in MSWin filenames are:
// < > : " / \ | ? *

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _MSC_VER
    #include <unistd.h>
#endif

#include <geom/mcad_utils.h>


static bool checkWinPath( std::string& aPath, char sep, char alt  )
{
    if( aPath.empty() )
        return true;

    static char badchars[] = "<>:\"/|?*";

    size_t sp = aPath.find_first_of( badchars );

    if( sp != std::string::npos && !( ':' == aPath[sp] && 1 == sp ) )
    {
        std::cerr << __FILE__ << ":" << __LINE__ << ":";
        std::cerr << __FUNCTION__ << ": illegal characters in path\n";
        return false;
    }


    while( sp != std::string::npos )
    {
        aPath.replace( sp, 1, 1, sep );
        sp = aPath.find_first_of( alt );
    }

    // strip a trailing '\' unless it is the only character
    size_t sl = aPath.size() - 1;

    if( '\\' == aPath[sl] && 0 != sl )
        aPath.erase( aPath.size() - 1 );

    return true;
}


// Convert the path to native format and perform checks
static bool makePathNative( std::string& aPath, char sep, char alt )
{
    if( aPath.empty() )
        return true;

    // convert all occurrences of the path separator
    size_t sp = aPath.find_first_of( alt );

    while( sp != std::string::npos )
    {
        aPath.replace( sp, 1, 1, sep );
        sp = aPath.find_first_of( alt );
    }

    bool result = false;

    if( sep == '\\' )
        result = checkWinPath( aPath, sep, alt );
    else
        result = true;

    if( false == result )
        aPath.clear();

    return result;
}


MCAD_FILEPATH::MCAD_FILEPATH()
{
#ifdef __WIN32
    pathsep = '\\';
    altsep = '/';
#else
    pathsep = '/';
    altsep = '\\';
#endif

    m_fullpath = new std::string;
    m_filename = new std::string;
    m_basename = new std::string;
    m_ext = new std::string;

    return;
}


MCAD_FILEPATH::MCAD_FILEPATH( const char* aPath )
{
#ifdef __WIN32
    pathsep = '\\';
    altsep = '/';
#else
    pathsep = '/';
    altsep = '\\';
#endif

    m_fullpath = new std::string;
    m_filename = new std::string;
    m_basename = new std::string;
    m_ext = new std::string;

    SetPath( aPath );

    return;
}


MCAD_FILEPATH::~MCAD_FILEPATH()
{
    delete m_fullpath;
    delete m_filename;
    delete m_basename;
    delete m_ext;
}


bool MCAD_FILEPATH::SetPath( const char* aPath )
{
    m_fullpath->clear();
    m_filename->clear();
    m_basename->clear();
    m_ext->clear();

    if( NULL == aPath || 0 == aPath[0] )
        return true;

    *m_fullpath = aPath;

    return makePathNative( *m_fullpath, pathsep, altsep );
}


bool MCAD_FILEPATH::Exists( void )
{
    if( m_fullpath->empty() )
        return false;

#if defined( _MSC_VER )
    struct _stat fs;

    if( 0 != _stat( m_fullpath->c_str(), &fs ) )
        return false;

    if( fs.st_mode & ( _S_IFDIR | _S_IFREG ) )
        return true;
#else
    struct stat fs;

    if( 0 != stat( m_fullpath->c_str(), &fs ) )
        return false;

    if( fs.st_mode & ( S_IFDIR | S_IFREG ) )
        return true;
#endif

    return false;
}


bool MCAD_FILEPATH::DirExists( void )
{
    if( m_fullpath->empty() )
        return false;

#if defined( _MSC_VER )
    struct _stat fs;

    if( 0 != _stat( m_fullpath->c_str(), &fs ) )
        return false;

    if( fs.st_mode & _S_IFDIR )
        return true;
#else
    struct stat fs;

    if( 0 != stat( m_fullpath->c_str(), &fs ) )
        return false;

    if( fs.st_mode & S_IFDIR )
        return true;
#endif

    return false;
}


bool MCAD_FILEPATH::FileExists( void )
{
    if( m_fullpath->empty() )
        return false;

#if defined( _MSC_VER )
    struct _stat fs;

    if( 0 != _stat( m_fullpath->c_str(), &fs ) )
        return false;

    if( fs.st_mode & _S_IFREG )
        return true;
#else
    struct stat fs;

    if( 0 != stat( m_fullpath->c_str(), &fs ) )
        return false;

    if( fs.st_mode & S_IFREG )
        return true;
#endif

    return false;
}


const char* MCAD_FILEPATH::GetFullPath( void )
{
    if( m_fullpath->empty() )
        return NULL;

    return m_fullpath->c_str();
}


const char* MCAD_FILEPATH::GetFileName( void )
{
    if( m_fullpath->empty() )
        return NULL;

    if( m_filename->empty() )
    {
        const char* cp = GetBaseName();

        if( NULL == cp )
            return NULL;

        *m_filename = cp;

        cp = GetExtension();

        if( NULL != cp )
        {
            m_filename->append( 1, '.' );
            m_filename->append( cp );
        }
    }

    return m_filename->c_str();
}


const char* MCAD_FILEPATH::GetBaseName( void )
{
    if( m_fullpath->empty() )
        return NULL;

    if( !m_basename->empty() )
        return m_basename->c_str();

    size_t lastSep = m_fullpath->find_last_of( pathsep );

    if( std::string::npos == lastSep )
        lastSep = 0;
    else
        ++lastSep;

    size_t lastDot = m_fullpath->find_last_of( '.' );

    if( std::string::npos == lastDot )
        lastDot = m_fullpath->size();

    if( lastDot > lastSep )
    {
        *m_basename = m_fullpath->substr( lastSep, lastDot - lastSep );

        if( m_ext->empty() )
            *m_ext = m_fullpath->substr( lastDot + 1 );
    }
    else
    {
        *m_basename = m_fullpath->substr( lastSep );
    }

    return m_basename->c_str();
}


const char* MCAD_FILEPATH::GetExtension( void )
{
    if( m_fullpath->empty() )
        return NULL;

    if( !m_ext->empty() )
        return m_ext->c_str();

    size_t lastDot = m_fullpath->find_last_of( '.' );

    if( std::string::npos == lastDot )
        return NULL;

    size_t lastSep = m_fullpath->find_last_of( pathsep );

    if( std::string::npos == lastSep )
        lastSep = 0;
    else
        ++lastSep;

    if( lastDot <= lastSep )
        return NULL;

    *m_ext = m_fullpath->substr( lastDot + 1 );

    return m_ext->c_str();
}


bool MCAD_FILEPATH::SetExtension( const char* anExt )
{
    char ps[] = ":";

    if( m_fullpath->empty()
        || ( '\\' == pathsep && m_fullpath->size() > 2 &&  !m_fullpath->compare( ps ) ) )
    {
        return false;
    }

    if( m_ext->empty() && NULL != GetExtension() )
        m_fullpath->erase( m_fullpath->size() - m_ext->size() -1 );

    m_filename->clear();
    m_ext->clear();

    if( NULL == anExt || 0 == anExt[0] )
        return true;

    if( '.' != anExt[0] )
    {
        m_fullpath->append( 1, '.' );
        *m_ext = anExt;
    }
    else
    {
        if( 0 == anExt[1] )
            return false;

        *m_ext = &anExt[1];
    }

    m_fullpath->append( *m_ext );
    return true;
}


char MCAD_FILEPATH::GetPathSep( void )
{
    return pathsep;
}


bool MCAD_FILEPATH::Append( const char* aSubPath )
{
    if( NULL == aSubPath || 0 == aSubPath[0] )
        return true;

    std::string subp = aSubPath;

    if( makePathNative( subp, pathsep, altsep ) )
    {
        if( subp.size() == 1 && subp[0] == pathsep )
            return true;

        m_fullpath->append( 1, pathsep );
        m_fullpath->append( subp );
        m_filename->clear();
        m_basename->clear();
        m_ext->clear();
    }

    return false;
}
