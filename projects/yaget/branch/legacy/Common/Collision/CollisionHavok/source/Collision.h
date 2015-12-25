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
#include <boost/signal.hpp>


// forward declarations
class hkpWorld;
class hkThreadMemory;
class hkpPhysicsContext;
class hkVisualDebugger;
class hkpPhysicsSystem;

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

        //! Called by Collision components on creation and destructions
        //! oId is associated with each hkpPhysicsSystem
        void AddToSimulation(const hkpPhysicsSystem *sys, const component::ObjectId& oId);
        void RemoveFromSimulation(const hkpPhysicsSystem *sys, const component::ObjectId& oId);

        // from ICollision
        virtual collision::Response HitObjects(const collision::Request& request) const;

        // from IPluginObject
        virtual const std::string& GetName() const;
        virtual const Version GetVersion() const;

    private:
        // from IPluginObject
        virtual eInitReturnCode OnInit(int32_t initPass);

        void onTick(Message& msg);
        void onShutdown(Message& msg);
        boost::signals::connection mTickSlot;
        boost::signals::connection mShutdowSlot;

        hkpWorld* mpPhysicsWorld;
        hkThreadMemory *mpThreadMemory;
        char *mpStackBuffer;
        float mDeltaTimeAccumulator;
        hkpPhysicsContext *mpDebugContext;
        hkVisualDebugger *mpvdb;
        //! Map from rigid body (and all it's pieces) to Object id, which can be turn
        //! into proper IComponent
        typedef std::map<size_t, component::ObjectId> BodyMap_t;
        BodyMap_t mBodyMap;
    };

} // namespace eg

#endif // COLLISION_H


