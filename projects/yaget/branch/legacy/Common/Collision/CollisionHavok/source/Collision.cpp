#include "Precompiled.h"
#include "Collision.h"
#include "ComponentEnumsCollision.h"
#include "Config/ConfigHelper.h"
#include "Registrate.h"
#include "MessageInterface.h"
#include "Message/Dispatcher.h"
#include "YagetVersion.h"
#include <wx/confbase.h>
#include <wx/dynload.h>
#include <boost/bind.hpp>

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/Memory/Pool/hkPoolMemory.h>
#include <Common/Visualize/hkVisualDebugger.h>
#include <Common/Serialize/Resource/hkResource.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>
#include <Physics/Collide/Query/Collector/RayCollector/hkpClosestRayHitCollector.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>
#include <Physics/Utilities/Serialize/hkpPhysicsData.h>
// Classlists
#define INCLUDE_HAVOK_PHYSICS_CLASSES
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

// Generate a custom list to trim memory requirements
#define HK_COMPAT_FILE <Common/Compat/hkCompatVersions.h>
#include <Common/Compat/hkCompat_None.cxx>

namespace 
{
    static void HK_CALL errorReport(const char *msg, void *)
    {
        wxLogError("%s", msg);
        //printf("%s", msg);
    }

    const float kStepFrame = 1.0f/60.0f;
} // namespace 


namespace eg {


IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS(Collision, IPluginObject)


Collision::Collision()
: mpPhysicsWorld(0)
, mpThreadMemory(0)
, mpStackBuffer(0)
, mDeltaTimeAccumulator(0)
, mpDebugContext(0)
, mpvdb(0)
{
    wxLogTrace(TR_COLLISION, "CollisionObject Havok Created.");
}


Collision::~Collision()
{
    registrate::RegisterObject(boost::any(), "Collision");
    wxLogTrace(TR_COLLISION, "CollisionObject Havok Deleted.");
}


const std::string& Collision::GetName() const
{
    static std::string staticName("Collision");
    return staticName;
}


IPluginObject::eInitReturnCode Collision::OnInit(int32_t initPass)
{
    if (initPass == 0)
    {
        hkPoolMemory *memoryManager = new hkPoolMemory();
        mpThreadMemory = new hkThreadMemory(memoryManager);
        hkBaseSystem::init(memoryManager, mpThreadMemory, errorReport);
        memoryManager->removeReference();

        int stackSize = 0x100000;
        mpStackBuffer = hkAllocate<char>(stackSize, HK_MEMORY_CLASS_BASE);
        hkThreadMemory::getInstance().setStackArea(mpStackBuffer, stackSize);

        hkpWorldCinfo worldInfo;

        // Set the simulation type of the world to multi-threaded.
        worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_CONTINUOUS ;

        // Flag objects that fall "out of the world" to be automatically removed - just necessary for this physics scene
        worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_FIX_ENTITY;

        mpPhysicsWorld = new hkpWorld(worldInfo);
        //int refCount = mpPhysicsWorld->getReferenceCount();

        // Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
        //physicsWorld->m_wantDeactivation = false;


        // When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
        // to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
        // must call markForRead / markForWrite before it modifies the world to enable these checks.
        //mpPhysicsWorld->markForWrite();


        // Register all collision agents, even though only box - box will be used in this particular example.
        // It's important to register collision agents before adding any entities to the world.
        hkpAgentRegisterUtil::registerAllAgents(mpPhysicsWorld->getCollisionDispatcher());

        // We need to register all modules we will be running multi-threaded with the job queue
        //physicsWorld->registerWithJobQueue( jobQueue );

        // Create all the physics rigid bodies
        //setupPhysics( physicsWorld );

        // add plane 
        hkVector4 groundRadii( 700.0f, 2.0f, 1400.0f );
        hkpConvexShape *shape = new hkpBoxShape(groundRadii , 0);

        hkpRigidBodyCinfo ci;

        ci.m_shape = shape;
        ci.m_motionType = hkpMotion::MOTION_FIXED;
        ci.m_position = hkVector4(0.0f, -20.0f, 0.0f);
        ci.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;

        mpPhysicsWorld->addEntity(new hkpRigidBody(ci))->removeReference();
        shape->removeReference();


        if (config::ReadBool("Debug/Collision/Debug", false))
        {
            mpDebugContext = new hkpPhysicsContext();
            hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
            mpDebugContext->addWorld(mpPhysicsWorld); // add the physics world so the viewers can see it

            hkArray<hkProcessContext*> contexts;
            contexts.pushBack(mpDebugContext);
            mpvdb = new hkVisualDebugger(contexts);
            mpvdb->serve();
        }

        Dispatcher& disp = REGISTRATE(Dispatcher);
        mTickSlot = disp[mtype::kFrameTick].connect(boost::bind(&Collision::onTick, this, _1));
        mShutdowSlot = disp[mtype::kShutdownBegin].connect(boost::bind(&Collision::onShutdown, this, _1));
        
        registrate::RegisterObject(static_cast<ICollision *>(this), "Collision");
    }

    return IPluginObject::ircDone;
}


void Collision::onTick(Message& msg)
{
    float deltaTime = msg.GetValue<float>();

    mDeltaTimeAccumulator += deltaTime;
    while (mDeltaTimeAccumulator >= kStepFrame)
    {
        mpPhysicsWorld->stepDeltaTime(kStepFrame);
        if (mpvdb)
        {
            mpvdb->step(kStepFrame * 1000);
        }

        mDeltaTimeAccumulator -= kStepFrame;
    }
}


void Collision::onShutdown(Message& msg)
{
    mShutdowSlot.disconnect();
    mTickSlot.disconnect();

    if (mpPhysicsWorld)
    {
        mpPhysicsWorld->removeReference();
        mpPhysicsWorld = 0;
    }

    if (mpvdb)
    {
        mpvdb->removeReference();
        mpvdb = 0;
    }

    if (mpDebugContext)
    {
        mpDebugContext->removeReference();
        mpDebugContext = 0;
    }

    if (mpThreadMemory)
    {
        mpThreadMemory->setStackArea(0, 0);
        hkDeallocate(mpStackBuffer);
        mpThreadMemory->removeReference();
        mpThreadMemory = 0;
    }

    // Quit base system
    hkBaseSystem::quit();
}


const Version Collision::GetVersion() const
{
    return YagetVersion;
}


collision::Response Collision::HitObjects(const collision::Request& request) const
{
    collision::Response response;
    if (mpPhysicsWorld)
    {
        Vector3 start = request.ray.Origin;
        Vector3 end = request.ray.Dir * 10000;
        hkpWorldRayCastInput input;
        input.m_from.set(start.x, start.y, start.z);
        input.m_to.set(end.x, end.y, end.z);

        hkpClosestRayHitCollector output;

        mpPhysicsWorld->castRay(input, output);
        if(output.hasHit())
        {
            const hkpWorldRayCastOutput& hitRecord = output.getHit();
            size_t bodyKey(reinterpret_cast<size_t>(hitRecord.m_rootCollidable));
            BodyMap_t::const_iterator it = mBodyMap.find(bodyKey);
            if (it != mBodyMap.end())
            {
                collision::Response::Record record;

                record.oId = (*it).second;
                record.hitPoint = ((end - start) * hitRecord.m_hitFraction) + start;
                record.hitNormal.set(hitRecord.m_normal(0), hitRecord.m_normal(1), hitRecord.m_normal(2));
                
                response.Hits.push_back(record);
            }
            else
            {
                wxLogWarning("Did not find body map entry for Collision query.");
            }
        }
    }

    return response;
}


void Collision::AddToSimulation(const hkpPhysicsSystem *sys, const component::ObjectId& oId)
{
    if (!mpPhysicsWorld)
    {
        wxLogError("Collision::AddToSimulation called but there is no hkpWorld created.");
        return;
    }

    const hkArray<hkpRigidBody*>& rigidBodies = sys->getRigidBodies();
    for (int i = 0; i < rigidBodies.getSize(); i++)
    {
        hkpRigidBody *pBody = rigidBodies[i];
        size_t bodyKey(reinterpret_cast<size_t>(pBody->getCollidable()));
        wxASSERT(mBodyMap.find(bodyKey) == mBodyMap.end());
        mBodyMap.insert(std::make_pair(bodyKey, oId));
    }

    mpPhysicsWorld->addPhysicsSystem(sys);

}


void Collision::RemoveFromSimulation(const hkpPhysicsSystem *sys, const component::ObjectId& /*oId*/)
{
    if (!mpPhysicsWorld)
    {
        wxLogError("Collision::RemoveFromSimulation called but there is no hkpWorld created.");
        return;
    }

    const hkArray<hkpRigidBody*>& rigidBodies = sys->getRigidBodies();
    for (int i = 0; i < rigidBodies.getSize(); i++)
    {
        hkpRigidBody *pBody = rigidBodies[i];
        size_t bodyKey(reinterpret_cast<size_t>(pBody->getCollidable()));
        wxASSERT(mBodyMap.find(bodyKey) != mBodyMap.end());
        mBodyMap.erase(bodyKey);
    }

    mpPhysicsWorld->removePhysicsSystem(sys);
}

} // namespace eg

