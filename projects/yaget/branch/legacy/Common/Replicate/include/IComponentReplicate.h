///////////////////////////////////////////////////////////////////////
// IComponentReplicate.h
//
//  Copyright 12/01/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface for replicate component
//
//
//  #include "IComponentReplicate.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_REPLICATE_H
#define I_COMPONENT_REPLICATE_H

#include "ReplicateBase.h"
#include "IComponent.h"
#include "Math/Vector.h"
#include "ComponentEnumsReplicate.h"
#include <boost/function.hpp>

namespace eg
{
    namespace network
    {
        /*!
        * This function signature is used internally by concrete implementation of IComponentReplicate
        * to send data across network (RPC). This function is provided by owning process
        * which has needs to replicate data across network.
        */
        typedef boost::function<void (const eg::component::InstanceId& /*iId*/, const std::string& /*propName*/)> Transport_t;

    } // namespace network

    /*!
     This component provides replication of specified instance components
     with the same object id. across network wire.

     /code
     -- create replicate table in DB
     CREATE TABLE component_replicate
     (
         object_id bigint NOT NULL,
         replicate_rate real,
         CONSTRAINT pk_replicate PRIMARY KEY (object_id)
     );
     /endcode
    */
    class DLLIMPEXP_REPLICATE IComponentReplicate : public IComponent
    {
    public:
        //! Type of component
        const static uint32_t Type = 0x2f0adebe;//ComponentType::kReplicate;

        network::Transport_t Transport;

    private:
    };

} // namespace eg

#endif // I_COMPONENT_REPLICATE_H

