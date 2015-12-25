#include "Precompiled.h"
#include "ComponentWidget.h"
#include "Config/ConfigHelper.h"
#include "Plugin/ModuleHelper.h"
#include "Widget/WidgetAsset.h"
#include "File/AssetLoadSystem.h"
#include <boost/format.hpp>
//#include <wx/confbase.h>
//#include <wx/statline.h>


namespace eg
{


EG_MODULE_REGISTER_COMPONENT(ComponentWidget);


ComponentWidget::ComponentWidget(component::ObjectId oId)
: ComponentHelper(oId, widget::kGroup, ComponentHelper::ccAttached)
, mWidgetNames(get_propV<std::vector<std::string> >(mProperty, widget::kPropWidgetNames, std::vector<std::string>(), ""))
, mWidgetOptions(get_propV<std::vector<std::string> >(mProperty, widget::kPropWidgetOptions, std::vector<std::string>(), ""))
, mWidgetQuery(get_propV<std::string>(mProperty, widget::kPropWidgetQuery, "", ""))
{
}


ComponentWidget::~ComponentWidget()
{
}


void ComponentWidget::DeleteThis()
{
    delete this;
}


void ComponentWidget::Shutdown()
{
}


Dispatcher::Result_t ComponentWidget::HandleMessage(Message& /*msg*/)
{
}


void ComponentWidget::onPropertyChanged(const IProperty *pProperty)
{
    if (pProperty->GetPropId() == mWidgetNames.GetPropId())
    {
        //! \note we should preserve current state of widgets, since
        //! they could be in new mWidgetNames
        mWidgets.clear();

        AssetLoadSystem& als = registrate::ref_cast<AssetLoadSystem>("AssetLoadSystem");
        uint64_t objectId = OI().Id.GetValue();
        std::string objectName = boost::str(boost::format("0x%016X") % objectId);

        std::vector<std::string> names = mWidgetNames;
        for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it)
        {
            if (guid_t widgetType = widgets::GetWidgetType(*it))
            {
                std::string assetName = objectName + "_" + *it + ".wig";
                if (AssetLoadSystem::IAsset_t asset = als.GetAsset(assetName, widgetType, AssetLoadSystem::eLoadBlocking))
                {
                    mWidgets.insert(std::make_pair(*it, asset));
                    std::string oIdString = "ObjectId = ";
                    oIdString += ConvertProperty<component::ObjectId>(OI().Id);
                    Message msg(0, oIdString);
                    asset->HandleMessage(msg);
                }
            }
        }
    }
    else if (pProperty->GetPropId() == mWidgetOptions.GetPropId())
    {
        std::vector<std::string> options = mWidgetOptions;
        for (std::vector<std::string>::const_iterator it = options.begin(); it != options.end(); ++it)
        {
            std::string currOptions = *it;
            //! "widgetName {option1 = bla_bla; option2 = bla}"
            // extract widget name first
            std::string::size_type index = currOptions.find_first_of("{");
            if (index != std::string::npos)
            {
                std::string widgetName = currOptions.substr(0, index);
                boost::trim(widgetName);

                // find widget
                Widgets_t::iterator wit = mWidgets.find(widgetName);
                if (wit != mWidgets.end())
                {
                    // get actual options and remove '{' and '}'
                    std::string commands = currOptions.substr(index);
                    boost::erase_all(commands, std::string("{"));
                    boost::erase_all(commands, std::string("}"));
                    Message msg(0, commands);
                    (*wit).second->HandleMessage(msg);
                }
            }
        }
    }
    else if (pProperty->GetPropId() == mWidgetQuery.GetPropId())
    {
        std::string widgetName = mWidgetQuery;
        Widgets_t::iterator wit = mWidgets.find(widgetName);
        if (wit != mWidgets.end())
        {
            Message msg(0, std::string("QueryOptions"));
            (*wit).second->HandleMessage(msg);
            // msg.mAnyData will contain results (all available options)
            mWidgetQuery = msg.GetValue<std::string>();
			// also, let's trace this to a log
			wxLogTrace(TR_COMPONENTS, "%s", (std::string)mWidgetQuery);
        }
    }
}


Hash ComponentWidget::GetComponentTypeId() const
{
    return ComponentType::kWidget;
}


bool ComponentWidget::RegisterComponentType()
{
    InitComponentType(ComponentType::kWidget, "Widget");
    return true;
}


} // namespace eg
