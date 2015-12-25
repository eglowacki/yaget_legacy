///////////////////////////////////////////////////////////////////////
// Collision.h
//
//  Copyright 09/13/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Implementation of a Collision class which handles
//      front ednf ro any collison/physics api
//
//
//  #include "Collision.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COLLISION_H
#define COLLISION_H

#include "Interface/ICollision.h"
#include "CollisionModule.h"
#include <vector>
#include <string>


namespace eg
{
    /*!
    Implementation of our Collision object
    */
    class Collision : public ICollision
    {
        DECLARE_USER_EXPORTED_PLUGGABLE_CLASS(Collision, DLLIMPEXP_COLLISION)

    public:
        Collision();
        virtual ~Collision();

        // from ICollision
        virtual collision::Response HitObjects(const collision::Request& request) const;

        // from IPluginObject
        virtual const std::string& GetName() const;
        virtual const Version GetVersion() const;

    private:
        // from IPluginObject
        virtual eInitReturnCode OnInit(int32_t initPass);

        void Shutdown();
    };

} // namespace eg

#endif // COLLISION_H


