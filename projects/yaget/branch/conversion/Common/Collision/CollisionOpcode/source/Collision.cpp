#include "Precompiled.h"
#include "Collision.h"
#include "ComponentEnumsCollision.h"
#include "MessageInterface.h"
#include "YagetVersion.h"
#include <wx/confbase.h>
#include <wx/dynload.h>

namespace eg {


IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS(Collision, IPluginObject)


Collision::Collision()
{
    wxLogTrace(TR_COLLISION, "CollisionObject Created.");
}


Collision::~Collision()
{
    registrate::RegisterObject(boost::any(), "Collision");
    wxLogTrace(TR_COLLISION, "CollisionObject Deleted.");
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
        registrate::RegisterObject(static_cast<ICollision *>(this), "Collision");
    }

    return IPluginObject::ircDone;
}


const Version Collision::GetVersion() const
{
    return YagetVersion;
}


collision::Response Collision::HitObjects(const collision::Request& request) const
{
    request;
    collision::Response response;
    response.Hits.push_back(45);
    return response;
}

} // namespace eg

