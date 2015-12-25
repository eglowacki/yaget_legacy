// TestScriptComponent
#pragma warning (disable : 4127) // conditional expression is constant
#include "ComponentEnumsObjects.h"
#include "IObjectManager.h"
#include "IComponentScript.h"
#include "IdGameCache.h"
#include "Input/InputManager.h"
#include "wx/utils.h"

#include <UnitTest++.h>

using namespace eg;

namespace
{
    const float kFrameRate = 1.0/20;

    void TakeTime(uint32_t ticks)
    {
        for (uint32_t i = 0; i < ticks; i++)
        {
            Message(mtype::kFrameTick, kFrameRate).Send();
            wxSleep(kFrameRate);
        }
    }

} // namespace

TEST(Components_Script)
{
    /*
    InputAction cameraMoveAction;

    InputManager::Mouse *mInput = new InputManager::Mouse(InputTypes::kMouseMove);
    mInput->mButtons[InputTypes::kMouseLeft] = true;
    cameraMoveAction.RegisterAction("Camera.Move", mInput, 0, false);

    IdGameCache& igc = REGISTRATE(IdGameCache);
    component::ObjectId oId(igc.GetId(IdGameCache::itBurnable));

    IComponentScript *pScript = component::newc<IComponentScript>("StartUpScript", oId);
    CHECK(pScript);

    IPropertySet& propSet = pScript->GetPropertyData();
    propSet[script::kPropPackageName] = "Test";
    TakeTime(1);
    propSet[script::kPropCommandNames] = "onTick";
    TakeTime(1);
    propSet[script::kPropCommandNames] = "onTick";
    TakeTime(1);

    //oId.Set(igc.GetId(IdGameCache::itBurnable));
    //IComponentScript *pTest = component::newc<IComponentScript>("TestComponent", oId);
    //TakeTime(2);

    //propSet["Script Name"] = std::string("PongScoreboard");
    //propSet["Command Name"]= std::string("pong.Scoreboard");
    //TakeTime(2);
    */
}

