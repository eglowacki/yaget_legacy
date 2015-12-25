///////////////////////////////////////////////////////////////////////
// ComponentEnumsRender.h
//
//  Copyright 10/29/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      All the components types, interface provided byt his render module
//
//
//  #include "ComponentEnumsRender.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_ENUMS_RENDER_H
#define COMPONENT_ENUMS_RENDER_H

#include "Hash/Hash.h"

namespace eg
{

    /*!
    Components provided by this module and available to the game.
    */
    namespace ComponentType
    {
        static const Hash kView("View");
        static const Hash kModel("Model");

    } // namespace ComponentType


    /*!
    Message types handles by Model component
    */
    namespace mtype
    {
        //! This is send from renderer after it's intialize and ready to go
        static const guid_t kRenderValid = 0x90711353;

        /*!
        This allow us to change shader at any time by sending this message
        Message::mpData points to std::string * which is name of the new shader to use
        Component will put it's old shader name after setting to new one
        */
        static const guid_t kSetShader = 0xf14fefb9;

        /*!
        This allow us to render model or not based on Message::mpData value
        Message::mpData is:
            0 - Hide model
            1 - Render model
        */
        static const guid_t kRenderModel = 0xd0c464f4;

        /*!
        This is send to Material Factory to get all names.
        Message::mpData is:
            pointer to std::vector<std::string> which is provided by the caller

        \note this is not safe across DLL's
        */
        static const guid_t kGetMaterialNames = 0x10e7d4fb;

        /*!
        This is send to Material Factory to get all render target names.
        Message::mpData is:
            pointer to std::vector<std::string> which is provided by the caller

        \note this is not safe across DLL's
        */
        static const guid_t kGetRenderTargetNames = 0xf1076d78;

    } // namespace MType

} // namespace eg

#endif // COMPONENT_ENUMS_RENDER_H

