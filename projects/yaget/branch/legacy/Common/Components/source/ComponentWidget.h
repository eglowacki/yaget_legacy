///////////////////////////////////////////////////////////////////////
// ComponentWidget.h
//
//  Copyright 2/8/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private implementation of Widget model
//
//
//  #include "ComponentWidget.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_WIDGET_H
#define COMPONENT_WIDGET_H

#include "IComponentWidget.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"
#include "File/AssetLoadSystem.h"


namespace eg
{
    class ComponentWidget : public IComponentWidget, public ComponentHelper<ComponentWidget, PropertySet>
    {
    public:
        ComponentWidget(component::ObjectId oId);
        virtual ~ComponentWidget();

        // from IComponent
        virtual void DeleteThis();
        virtual Hash GetComponentTypeId() const;
        virtual Dispatcher::Result_t HandleMessage(Message& msg);
        virtual IPropertySet& GetPropertyData() {return CH_t::GetPropertyData();}
        virtual const ObjectInstance& OI() const {return CH_t::OI();}

        // Static methods
        static bool RegisterComponentType();

    private:
        virtual void onPropertyChanged(const IProperty *pProperty);

        // from IComponent
        virtual void Shutdown();

        //! list of widgets by name which can be used
        Property<std::vector<std::string> >& mWidgetNames;
        //! list of options in fallowing form
        //! "widgetName {option1 = bla_bla; option2 = bla}"
        Property<std::vector<std::string> >& mWidgetOptions;
        //! This will return all valid options for specific widget
        //! std::string options = mWidgetQuery = "plane";
        //! where options will have all options for 'plane' widget
        Property<std::string>& mWidgetQuery;

        typedef std::map<std::string, AssetLoadSystem::IAsset_t> Widgets_t;
        Widgets_t mWidgets;
    };

} // namespace eg

#endif // COMPONENT_WIDGET_H

