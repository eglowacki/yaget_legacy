//////////////////////////////////////////////////////////////////////
// Network.h
//
//  Copyright 4/10/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "Managed/Network.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_NETWORK_H
#define MANAGED_NETWORK_H
#pragma once

namespace eg
{
    namespace managed
    {
        /*!
        One to One mapping for WorldInfo in C++
        */
        public ref class WorldInfo
        {
        public:
            String^ Name;
        };

    } // namespace managed

} // namespace eg

#endif // MANAGED_NETWORK_H

