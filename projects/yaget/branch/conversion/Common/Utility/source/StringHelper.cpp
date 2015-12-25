#include "Precompiled.h"
#pragma warning(push)
#pragma warning (disable : 4244)  // '' : conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
#include "StringHelper.h"
#pragma warning(pop)
#include <boost/foreach.hpp>

namespace
{
    std::string makeSafePath(const std::string& name)
    {
        std::string pathName = name;
        if (!pathName.empty())
        {
            // strip all text from first occurrence of '*' or '?'
            size_t i = pathName.find_first_of("?*");
            if (i != std::string::npos)
            {
                pathName = pathName.erase(i);
            }
            if (*pathName.rbegin() == '/' || *pathName.rbegin() == '\\')
            {
                pathName.erase(pathName.size()-1);
            }

            //! strip all text from last occurrence of '/' or '\'
            i = pathName.find_last_of("/\\");
            if (i != std::string::npos)
            {
                pathName = pathName.erase(i);
            }

            boost::replace_all(pathName, "/", ".");
            boost::replace_all(pathName, "\\", ".");
        }

        return pathName;
    }

} // namespace

namespace eg {


std::string NormalizePath(const std::string& pathName, bool bAddSeperator, bool bCheckAsWild)
{
    std::string newPath = pathName;
    if (bCheckAsWild && IsWildString(pathName))
    {
        newPath = makeSafePath(pathName);
    }
    else
    {
        bool slash = false;
        newPath = "";// = pathName;
        BOOST_FOREACH(char letter, pathName)
        {
            if (letter == '\\' || letter == '/')
            {
                if (!slash)
                {
                    slash = true;
                    newPath += file::sep;
                }

                continue; 
            }

            slash = false;
            if (letter == ' ')
            {
                newPath += '_';
            }
            else
            {
                newPath += letter;
            }
        }

        //boost::replace_all(newPath, "\\", file::sep);
        //boost::replace_all(newPath, "//", file::sep);
        //boost::replace_all(newPath, " ", "_");
    }

    if (bAddSeperator && (newPath.empty() || *newPath.rbegin() != file::sep[0]))
    {
        newPath += file::sep;
    }

    return newPath;
}


bool IsWildString(const std::string& text)
{
    return text.find('*') != std::string::npos || text.find('?') != std::string::npos;
}


bool WildCompare(const char *wild, const char *string)
{
    // if anny strings are null, then it will never match anything
    if (IsNull(wild) || IsNull(string))
    {
        return false;
    }

    // Written by Jack Handy - jakkhandy@hotmail.com
    const char *cp = NULL, *mp = NULL;

    while ((*string) && (*wild != '*'))
    {
        if ((*wild != *string) && (*wild != '?'))
        {
            return 0;
        }
        wild++;
        string++;
    }

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
                return 1;
            }

            mp = wild;
            cp = string+1;
        }
        else if ((*wild == *string) || (*wild == '?'))
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            string = cp++;
        }
    }

    while (*wild == '*')
    {
        wild++;
    }

    return(!*wild) == true;
}


} // namespace eg