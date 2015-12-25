///////////////////////////////////////////////////////////////////////
// ObjectMessage.h
//
//  Copyright 10/16/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ObjectMessage.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef OBJECT_MESSAGE_H
#define OBJECT_MESSAGE_H

#include "MessageEnums.h"

class wxWindow;

namespace eg
{

    //! This is only used if message type is kGetEditorProperties which mpData will point to
    namespace editor
    {
        struct UIProperties
        {
            UIProperties() :
                mpControl(0)
            {
            }

            Hash mComponentID;
            wxWindow *mpControl;
        };
    } // namespace editor


} // namespace eg

#endif //OBJECT_MESSAGE_H