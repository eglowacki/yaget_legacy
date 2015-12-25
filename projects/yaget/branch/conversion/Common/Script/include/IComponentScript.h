///////////////////////////////////////////////////////////////////////
// IComponentScript.h
//
//  Copyright 05/30/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface for Script component
//
//
//  #include "IComponentScript.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_SCRIPT_H
#define I_COMPONENT_SCRIPT_H

#include "ScriptBase.h"
#include "IComponent.h"
#include "ComponentEnumsScript.h"



namespace eg
{
    namespace script
    {
        static const char *kGroup = "Script";
        static const char *kPropPackageName = "PackageName";        ///< std::string
        static const char *kPropCommandNames = "CommandNames";      ///< std::vector<std::string>
    } // namespace script

    /*!
     /code
     -- create script table in DB
     CREATE TABLE component_script
     (
         object_id bigint NOT NULL,
         package_name varchar,
         command_names varchar[],
         CONSTRAINT pk_script PRIMARY KEY (object_id)
     );
     /endcode
    */
    class DLLIMPEXP_SCRIPT IComponentScript : public IComponent
    {
    public:
        //! Type of component
        const static uint32_t Type = 0xc5cc1ed8;//ComponentType::kScript

    private:
    };

} // namespace eg

#endif // I_COMPONENT_SCRIPT_H

