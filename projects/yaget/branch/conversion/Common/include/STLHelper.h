///////////////////////////////////////////////////////////////////////
// STLHelper.h
//
//  Copyright 11/6/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "STLHelper.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef STL_HELPER_H
#define STL_HELPER_H
#pragma once

#include <Base.h>

namespace eg
{
    /*!
    This is predicate for the for_each algorithm to delete objects.
    Usage:
        std::vector<ClassFoo *> container;
        std::for_each(container.begin(), container.end(), DeleteElem<ClassFoo *>);
        container.clear();
    */
    template <typename T> void DeleteElem(T elem)
    {
        delete elem;
    }

    /*!
    This is predicate for the for_each algorithm to delete objects, but assumes
    that it's the map
    Usage:
        std::map<int, ClassFoo *> container;
        std::for_each(container.begin(), container.end(), DeleteMap<std::map<int, ClassFoo *>::value_type>);
        container.clear();
    */
    template <typename T> void DeleteMap(T elem)
    {
        delete elem.second;
    }

    /*!
    This templetized method will delete all the objects and will clear the container
    Usage:
        std::vector<ClassFoo *> container;
        DeleteClear(container);
    */
    template <typename T> void DeleteClear(T& containerList)
    {
        std::for_each(containerList.begin(),
                      containerList.end(),
                      DeleteElem<T::value_type>);

        containerList.clear();
    }

    /*!
    This templetized method will delete all the objects in the map and will clear the container
    Usage:
        std::map<int, ClassFoo *> container;
        DeleteMapClear(container);
    */
    template <typename T> void DeleteMapClear(T& containerList)
    {
        std::for_each(containerList.begin(),
                      containerList.end(),
                      DeleteMap<T::value_type>);

        containerList.clear();
    }

} // namespace eg

#endif // STL_HELPER_H


