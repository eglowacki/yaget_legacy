//////////////////////////////////////////////////////////////////////
// YagetVersion.h
//
//  Copyright 9/1/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Version number for Yaget engine and all plugins used by it
//
//
//  #include "YagetVersion.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef YAGET_VERSION_H
#define YAGET_VERSION_H
#pragma once

#include "Base.h"
#include <ostream>
#include <sstream>

namespace yaget
{
    /*!
    Version number for current implementation of Yaget engine.
    */
    struct Version
    {
        Version(uint32_t major, uint32_t minor, uint32_t build)
        : Major(major)
        , Minor(minor)
        , Build(build)
        {}

        bool operator ==(const Version& source) const
        {
            return source.Major == Major && source.Minor == Minor && source.Build == Build;
        }

        //! Major version of engine. In most cases no other plugins will work anymore
        uint32_t Major;
        //! Small incremental change in Yaget engine.
        uint32_t Minor;
        //! Current public build of Yaget engine
        uint32_t Build;
    };

    const static Version YagetVersion(0, 1, 1);

    inline std::ostream& operator<<(std::ostream& out, const Version& version)
    {
        return out << version.Major << "." << version.Minor << "." << version.Build;
    }

    //! Utility functions for version comparisons
    namespace ver
    {
        inline bool is_major(const Version& currentVersion)
        {
            return currentVersion.Major == YagetVersion.Major;
        }

        inline bool is_minor(const Version& currentVersion)
        {
            return currentVersion.Minor == YagetVersion.Minor;
        }

        inline bool is_build(const Version& currentVersion)
        {
            return currentVersion.Build == YagetVersion.Build;
        }

        inline bool is_mm(const Version& currentVersion)
        {
            return is_major(currentVersion) && is_minor(currentVersion);
        }

        inline std::string to_string(const Version& currentVersion)
        {
            std::ostringstream strStream;
            strStream << currentVersion;

            return strStream.str();
        }

    } // namespace ver


} // namespace yaget

#endif // YAGET_VERSION_H

