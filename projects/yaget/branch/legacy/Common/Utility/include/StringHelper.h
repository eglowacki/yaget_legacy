///////////////////////////////////////////////////////////////////////
// StringHelper.h
//
//  Copyright 11/28/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      String utilities and helper functions.
//
//
//  #include "StringHelper.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef STRING_HELPER_H
#define STRING_HELPER_H
#pragma once


#include "Base.h"
#include <string>
#pragma warning(push)
#pragma warning (disable : 4512)  // '' assignment operator could not be generated
#include <boost/algorithm/string.hpp>
#pragma warning(pop)


namespace eg
{
    //! Return string which will have ending path seperator added if one does not exist
    std::string NormalizePath(const std::string& pathName, bool bAddSeperator = true, bool bCheckAsWild = true);

    //! Checks if pString null or pString[0] is null
    bool IsNull(const char *pString);

    //! Retunr true if text has any of DOS wild chars' ('*', '?') only so far
    bool IsWildString(const std::string& text);

    //! Return TRUE if string matches wild pattern which follows DOS style wild card file format
    bool WildCompare(const char *wild, const char *string);

    inline bool WildCompare(const std::string& wild, const std::string& string)
    {
        return WildCompare(wild.c_str(), string.c_str());
    }

    inline bool WildCompareI(const std::string& wild, const std::string& string)
    {
        std::string pattern = boost::to_lower_copy(wild);
        std::string fullString = boost::to_lower_copy(string);
        return WildCompare(pattern.c_str(), fullString.c_str());
    }

    inline bool WildCompareISafe(const std::string& wild, const std::string& string)
    {
        std::string pattern = NormalizePath(wild, false,  false);
        std::string fullString = NormalizePath(string, false,  false);
        return WildCompareI(pattern, fullString);
    }

    //! Helper function to convert string into number
    //! string can be in hex form (0x...) or decimal notation
    template <class T>
    T from_string(const std::string& s)
    {
        std::istringstream iss(s);
        T t(0);
        if (boost::istarts_with(s, std::string("0x")))
        {
            (iss >> std::hex >> t);
        }
        else
        {
            (iss >> std::dec >> t);
        }
        return t;
    }


    // ------------------------------------------------------------
    // inline implementation
    inline bool IsNull(const char *pString)
    {
        // return true if pString is NULL or pString points to NULL string
        return (pString && *pString != 0) == false;
    }


} // namespace eg


#endif // STRING_HELPER_H

