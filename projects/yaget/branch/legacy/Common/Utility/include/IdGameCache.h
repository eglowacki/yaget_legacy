/////////////////////////////////////////////////////////////////////
// IdGameCache.h
//
//  Copyright 1/14/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides id's for persistent and burnable id's
//      Persistent id's are extracted from DB, and burnable
//      are created locally which fall within some specified range which will
//      not overlap with persistent ones.
//
//
//  #include "IdGameCache.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef ID_GAME_CACHE_H
#define ID_GAME_CACHE_H
#pragma once

#include "Base.h"


namespace eg
{
    // forward declarations
    namespace config { class Konsole; }

    class IdGameCache
    {
    public:
        enum IdType {itBurnable, itPersistent};

        IdGameCache();
        IdGameCache(config::Konsole& konsole);
        ~IdGameCache();

        //! Return next available id which will be burnable or persistent based on IdType parameter
        uint64_t GetId(IdType idType);

    private:
        //! Burnable id's will only fall within this range
        //! first <= currentId < second
        std::pair<uint64_t, uint64_t> mBurnableRange;
        //! next valid burnable id
        uint64_t mNextBurnableId;
        //! Next persistent id
        //! \note we need to get batch of id's from DB
        uint64_t mNextPersistentId;
    };

    namespace idspace
    {
        //! Convenient functions to get id's
        uint64_t get_burnable(IdGameCache& idCache);
        uint64_t get_persistent(IdGameCache& idCache);

    } // namespace idspace


} // namespace eg

#endif // ID_GAME_CACHE_H

