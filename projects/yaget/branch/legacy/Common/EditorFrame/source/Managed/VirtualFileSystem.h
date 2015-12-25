//////////////////////////////////////////////////////////////////////
// VirtualFileSystem.h
//
//  Copyright 1/27/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Glue between C++ VirtualFileSystem and (MC++) .NET
//
//  #include "Managed/VirtualFileSystem.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_VIRTUAL_FILE_SYSTEM_H
#define MANAGED_VIRTUAL_FILE_SYSTEM_H
#pragma once

namespace eg
{
    namespace managed
    {
        public ref class VirtualFileSystem
        {
        public:
            //! Return read only stream.
            Stream^ GetFileStream(String^ name);

            //! Return writiable stream. When object is disposed, it will write out to a storage
            //! What storage depeneds on which FileFactory provided this stream
            Stream^ AttachFileStream(String^ name);

            //! Factory pattern
            static property VirtualFileSystem^ Get
            {
                VirtualFileSystem^ get()
                {
                    if (sInstance == nullptr)
                    {
                        sInstance = gcnew VirtualFileSystem();
                    }
                    return sInstance;
                }
            }

        private:
            //! Our one and only instance (Factory pattern)
            static VirtualFileSystem^ sInstance = nullptr;
        };

    } // namespace managed
} // namespace eg

#endif // MANAGED_VIRTUAL_FILE_SYSTEM_H

