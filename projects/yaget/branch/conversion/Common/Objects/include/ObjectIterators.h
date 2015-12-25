///////////////////////////////////////////////////////////////////////
// ObjectIterators.h
//
//  Copyright 10/22/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      ObjectIterators which provide iterators over
//		objects, components and properties
//
//  #include "ObjectIterators.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef OBJECT_ITERATORS_H
#define OBJECT_ITERATORS_H
#pragma once

#include "ObjectsBase.h"
#include "IComponent.h"


namespace eg
{
	namespace casters
	{
		//! Return pointer to valid property set or NULL if instance does not exist
		inline IPropertySet *property_set_cast(const component::InstanceId& iId)
		{
			if (IComponent *pComp = component::get(iId))
			{
				return &pComp->GetPropertyData();
			}

			return 0;
		}

		//! Return pointer to valid named property or NULL if instance does not exist
		inline IProperty *property_cast(const component::InstanceId& iId, const std::string& propName)
		{
			if (IPropertySet *pPropSet = property_set_cast(iId))
			{
				return pPropSet->FindProperty(propName);
			}

			return 0;
		}

	} // namespace casters

} // namespace eg

#endif // OBJECT_ITERATORS_H
