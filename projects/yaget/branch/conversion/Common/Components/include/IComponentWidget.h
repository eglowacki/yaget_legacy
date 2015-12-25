///////////////////////////////////////////////////////////////////////
// IComponentWidget.h
//
//  Copyright 2/8/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface for Widget component
//
//
//  #include "IComponentWidget.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_WIDGET_H
#define I_COMPONENT_WIDGET_H

#include "ComponentsBase.h"
#include "IComponent.h"
#include "ComponentEnumsComponents.h"

namespace eg
{

    namespace widget
    {
        static const char *kGroup = "Widget";
        static const char *kPropWidgetNames = "WidgetNames";        ///< std::vector<std::string>
        static const char *kPropWidgetOptions = "WidgetOptions";    ///< std::vector<std::string>
        static const char *kPropWidgetQuery = "WidgetQuery";        ///< std::string
    } // namespace widget

    /*!
     /code
     -- create widget table in DB
     CREATE TABLE component_widget
     (
         object_id bigint NOT NULL,
         widget_names varchar[],
         widget_options varchar[],
         CONSTRAINT pk_widget PRIMARY KEY (object_id)
     );
     /endcode
    */
    class DLLIMPEXP_COMPONENTS IComponentWidget : public IComponent
    {
    public:
        //! Type of component
        const static uint32_t Type = 0xa181444d;//ComponentType::kWidget

    private:
    };

} // namespace eg

#endif // I_COMPONENT_WIDGET_H

