//////////////////////////////////////////////////////////////////////
// YagetVersion.h
//
//  Copyright 9/1/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Version number for Yaget negine and all plugins used by it
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

namespace eg
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

        //! Major version of engine. In most cases no other plugins will work anymore
        uint32_t Major;
        //! Small incrimental change in Yaget engine.
        uint32_t Minor;
        //! Current public build of Yaget engine
        uint32_t Build;
    };

    const static Version YagetVersion(0, 1, 1);

    inline std::ostream& operator<<(std::ostream& out, const Version& version)
    {
        return out << version.Major << "." << version.Minor << "." << version.Build;
    }


} // namespace eg

#endif // YAGET_VERSION_H

