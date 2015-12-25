#include "BaseRegistrate.h"
#include "Config/InitSystem.h"
#include "Config/ConfigHelper.h"
#include "App/MainConsole.h"
#include "IObjectManager.h"
#include "IComponentArea.h"
#include "IComponentEntity.h"
#include "IComponentReplicate.h"
#include "Timer/ClockUtilities.h"
#include <wx/utils.h>
#include <wx/init.h>
#include <wx/log.h>

#ifndef YAGET_UT_MAIN

using namespace eg;


std::string MyFunction()
{
    std::string configFile;
    configFile += "[Plugins/Prerequisite]\n";
    configFile +=     "Objects = required\n";
    configFile += "[Plugins/Dynamic]\n";
    configFile +=     "Replicate = required\n";
    configFile +=     "Components = optional\n";
    return configFile;
}

eg::FunctionSetter functionSetter(&MyFunction);


class UT_Loop : public MainConsole
{
public:
    UT_Loop()
    : MainConsole("Unit Test: Replicate Loop", wxPoint(0, 0), wxSize(640, 480))
    , mMonster(10)
    , mPosX(0)
    {
        ClearConsole();
        SetColor(FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
        ConPrint("== Welcome to Unit Testing for [Replicate Component] ==\n");
        SetColor(FOREGROUND_GREEN|FOREGROUND_RED);
        ConPrint("-------------------------------------------------------\n");
        SetColor(FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN);
        ConPrint("Press ESC key to quit at any time...n");

        // load appropriate components to exercise replication code
        component::InstanceId iId(mMonster, ComponentType::kArea);
        /*IComponent * */component::newc("Monster", iId);

        iId.second = ComponentType::kReplicate;
        /*IComponent * */component::newc("Monster", iId);

        iId.second = ComponentType::kEntity;
        /*IComponent * */component::newc("Monster", iId);

        // let's get some notification message every so often
        mTimer.reset(new IntervalTimer(boost::bind(&UT_Loop::onTimer, this, _1), 2));
    }

private:
    void onTimer(Message& msg)
    {
        float deltaTime = msg.GetValue<float>();

        component::InstanceId iId(mMonster, ComponentType::kEntity);
        if (IComponent *pComponent = component::get(iId))
        {
            IPropertySet& propSet = pComponent->GetPropertyData();
            propSet["Position"] = Vector3(mPosX++, 0, 0);
        }
    }

    //! Provide this in derived class
    virtual void Tick(float deltaTime)
    {
    }

    boost::shared_ptr<IntervalTimer> mTimer;
    component::ObjectId mMonster;
    int mPosX;
};


int main(int argc, char* argv[])
{
    return StartConsole<UT_Loop>(argc, argv);
}


#endif // YAGET_UT_MAIN