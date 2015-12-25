///////////////////////////////////////////////////////////////////////
// IComponent.h
//
//  Copyright 10/16/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Base class for all components and helper templates to
//      provide easy of derivation of new component types.
//
//
//  #include "IComponent.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_H
#define I_COMPONENT_H
#pragma once


#include "IObjectManager.h"
#include "Property/IProperty.h"
#include <algorithm>
#include "Property/PropertyDataConversion.h"
//#include <boost/format.hpp>

namespace eg
{
    // Forward declarations
    class Message;
    class ObjectManager;

    /*!
     * This represents read only object instance info data
     * It provides unique ID associated with this instance
     * and user defined name for it. This structure is immutable
     * during run time.
     */
    class ObjectInstance
    {
    public:
        const std::string& Name;
        const component::ObjectId& Id;

    protected:
        ObjectInstance(const std::string& name, const component::ObjectId& id)
        : Name(name)
        , Id(id)
        {
        }
    };

    //-----------------------------------------------------------------------------------------------
    //! The IComponent class. All our component classes inherit from this
    class IComponent
    {
    public:
        //! ComponentAllocator will use this on this instance destruction.
        virtual void DeleteThis() = 0;

        //! Return const reference to ObjectInstance.
        virtual const ObjectInstance& OI() const = 0;

        //! This will return component ID this object represents
        virtual component::ComponentId GetComponentTypeId() const = 0;

        //! This will return reference to all readable and writable
        //! data for this component. It exposes those properties to runtime
        //! engine, scripting and network replication if appropriate.
        //! It is also used in editor to manipulate Components
        //! in class agnostic way.
        virtual IPropertySet& GetPropertyData() = 0;

        //! This allows component to catch any messages directed toward it.
        virtual void HandleMessage(Message& msg) = 0;

    protected:
        virtual ~IComponent() = 0 {}

    private:
        //! Called when we need to clear this object to it's virgin state
        virtual void Shutdown() = 0;

        friend ObjectManager;
    };

    /*!
    \page guide

    \section helperfunctions Component helper functions
    Helper functions for conversion, creation and deletion of components.

    Sample usage of various casting functions.
    \code
    #include "IComponent.h"

    using namespace eg::component;

    // Get already created instance, or NULL if there wasn't one.
    // Convert from ObjectId --> IComponent* of type T
    ObjectId objectId = // ... some object id we already have
    IComponentEntity *pEntity = get<IComponentEntity>(objectId);

    // convert from InstanceId --> IComponent*
    InstanceId iId = // ... some instance id we already have
    IComponent *pComponent = get(iId);

    // get instance id based on component
    // convert from IComponent* --> InstanceId
    IComponent *pComponent = // ... some Component we have pointer to
    InstanceId iId = getId(pComponent);

    // cast one component into another
    IComponentEntity *pEntity = // ... some Component we have pointer to
    IComponentArea *pArea = cast<IComponentArea>(pEntity);
    \endcode
    */

    //! Helper functions for conversion, creation and deletion of components.
    namespace component
    {
        //-----------------------------------------------------------------------------------------------
        //! Return IComponent for specific InstanceId
        inline IComponent *get(const InstanceId& iId)
        {
			IComponent *pResult = REGISTRATE(IObjectManager).FindComponentInstance(iId);
			return pResult;
        }

        //-----------------------------------------------------------------------------------------------
        //! return component pointer casted to T if one exist and it's of type T
        //! or NULL, if there wasn't one
        template <typename T>
        inline T *get(const ObjectId& oId)
        {
            T *pResult = dynamic_cast<T *>(REGISTRATE(IObjectManager).FindComponentInstance(InstanceId(oId, component::ComponentId(T::Type))));
            return pResult;
        }

        namespace internal
        {
            struct firstComponent : public EnumComponentCallback
            {
                inline firstComponent() : First(0) {}

                inline virtual bool OnComponent(IComponent *pComponent)
                {
                    First = pComponent;
                    return false;
                }

                IComponent *First;
            };

        } // namespace internal

        //! This is specialized function to return first valid component for object id
        //! if any components exist. or 0 if none.
        template <typename T>
        inline T *get(const InstanceId& iId)
        {
            return get<T>(iId.first);
        }

        //! This is specialized function to return first valid component for object id
        //! if any components exist. or 0 if none.
        template <>
        inline IComponent *get(const ObjectId& oId)
        {
            IObjectManager& om = REGISTRATE(IObjectManager);

            internal::firstComponent fc;
            om.EnumObjectsByObject(oId, fc);
            return fc.First;
        }


        //! Return instance id for this pComponent
        inline InstanceId getId(const IComponent *pComponent)
        {
            return InstanceId(pComponent->OI().Id, pComponent->GetComponentTypeId());
        }

        //! Cast one pointer of IComponent type to another based on T
        //! If there was no component of type T, return 0
        template <typename T>
        inline T *cast(const IComponent *pComponent)
        {
            return get<T>(getId(pComponent).first);
        }

        //-----------------------------------------------------------------------------------------------
        //! Checks to see if this instance is valid, which means that at lease one
        //! of the id's (object or type) must be valid.
        inline bool isValid(const InstanceId& iId)
        {
            return (iId.first.IsValid() == false && iId.second.IsValid() == false) != true;
        }

        //-----------------------------------------------------------------------------------------------
        //! Checks to see if this instance is valid, which means that at lease one
        //! of the id's (object or type) must be valid.
        inline bool isInstanceValid(const InstanceId& iId)
        {
            return iId.first.IsValid() && iId.second.IsValid();
        }

        //-----------------------------------------------------------------------------------------------
        //! Return object name if exist, otherwise return empty string.
        inline std::string objectName(const InstanceId& iId)
        {
            if (const IComponent *pComponent = get<IComponent>(iId.first))
            {
                return pComponent->OI().Name;
            }

            return "";
        }

        inline std::string objectName(const ObjectId& oId) { return objectName(InstanceId(oId, ComponentId()));}


        //-----------------------------------------------------------------------------------------------
        //! Return component name if second is valid, else if first is valid, return object name
        inline std::string componentName(const InstanceId& iId)
        {
            std::string compName;
            if (iId.second.IsValid())
            {
                compName = REGISTRATE(IObjectManager).GetComponentName(iId.second);
            }
            else if (iId.first.IsValid())
            {
                compName = "Object";
            }

            return compName;
        }

        //-----------------------------------------------------------------------------------------------
        //@{
        //! Return name of this instance in a format 'name:type'
        //! ex: 'Oak:Entity'
        inline std::string instanceName(const InstanceId& iId)
        {
            if (const IComponent *pComponent = get<IComponent>(iId.first))
            {
                //std::string name = boost::str(boost::format("'%1%:%2%'") % objectName(iId) % componentName(iId));
				std::string name = ConvertProperty<std::string>(objectName(iId)) + ":" + ConvertProperty<std::string>(componentName(iId));
                return name;
            }

            return "";
        }

        inline std::string instanceName(const IComponent *pComponent)
        {
            return instanceName(getId(pComponent));
        }
        //@}


        inline IComponent *newc(const std::string& name, const component::InstanceId& iId)
        {
            IObjectManager& om = REGISTRATE(IObjectManager);
            return om.CreateComponent(iId, name);
        }

        //! Return already created component instant or create one if it does not exist
        //!
        //! \param T Type of component we want
        //! \param name name of an object this component belongs to
        //!
        //! \return T* pointer to valid component cast it to correct type
        template <typename T>
        inline T *newc(const std::string& name, const component::ObjectId& oId)
        {
            if (IComponent *pComponent = newc(name, component::InstanceId(oId, component::ComponentId(T::Type))))
            {
                return cast<T>(pComponent);
            }

            return 0;
        }


        //! Template to delete component
        //! It will null out pComponent
        template <typename T>
        inline void freec(T *&pComponent)
        {
            IObjectManager& om = REGISTRATE(IObjectManager);
            om.DestroyComponent(pComponent);
            pComponent = 0;
        }

        template <>
        inline void freec(IComponent *&pComponent)
        {
            IObjectManager& om = REGISTRATE(IObjectManager);
            om.DestroyComponent(pComponent);
            pComponent = 0;
        }

        //! Delete this instance
        inline void freec(const component::InstanceId& iId)
        {
            if (IComponent *pComponent = get(iId))
            {
                freec(pComponent);
            }
        }

		/*
        inline void save(const component::ObjectId& oId, VirtualFileFactory::ostream_t ostream)
        {
            REGISTRATE(IObjectManager).SaveObject(oId, ostream);
        }

        inline bool load(const component::ObjectId& oId, VirtualFileFactory::istream_t istream)
        {
            return REGISTRATE(IObjectManager).LoadObject(oId, istream);
        }
		*/

    } // namespace component


    //-----------------------------------------------------------------------------------------------
    //! Predicate used in std::sort algorithm
    //! The component must provide GetSortValue() method.
    template <typename T>
    inline bool ComponentSort(const T *pComponentA, const T *pComponentB)
    {
        return pComponentA->GetSortValue() < pComponentB->GetSortValue();
    }


    //-----------------------------------------------------------------------------------------------
    //! Grab all components belonging to object
    struct ObjectListCallback : public EnumComponentCallback
    {
        typedef std::vector<IComponent *> List;

        ObjectListCallback(const component::ObjectId& oId)
        {
            REGISTRATE(IObjectManager).EnumObjectsByObject(oId, *this);
        }

        //! This is called for all enumerated component
        virtual bool OnComponent(IComponent *pComponent)
        {
            ComponentList.push_back(pComponent);
            return true;
        }

        List ComponentList;
    };

    //-----------------------------------------------------------------------------------------------
    /*!
    This template allows us to enumerate all of the components and put them
    into list. Also provides sort method
    */
    template <typename T>
    struct ComponentListCallback : public eg::EnumComponentSafeCallback<T>
    {
        ComponentListCallback()
        {
            component::ComponentId cId(T::Type);
            REGISTRATE(IObjectManager).EnumObjectsByType(cId, *this);
        }

        ComponentListCallback(const component::ComponentId& cId)
        {
            REGISTRATE(IObjectManager).EnumObjectsByType(cId, *this);
        }

        virtual bool ProcessComponent(CompType_t *pComponent)
        {
            mComponentList.push_back(pComponent);
            return true;
        }

        virtual void OnEnd()
        {
            Rewind();
        }

        //! Components are not sorted initially, so call this to do so.
        //! \note it will reset current iterator to beginning
        void Sort()
        {
            std::sort(mComponentList.begin(), mComponentList.end(), eg::ComponentSort<T>);
            Rewind();
        }

        bool IsFilled() const
        {
            return !mComponentList.empty();
        }

        //! Call this method to get next object in the list. It will return NULL, if there is no more
        CompType_t *Next()
        {
            if (mCurrComponent != mComponentList.end())
            {
                CompType_t *pCurrComponent = (*mCurrComponent);
                ++mCurrComponent;
                return pCurrComponent;
            }

            return 0;
        }

        //! Reset iterator to beginning of list
        void Rewind()
        {
            mCurrComponent = mComponentList.begin();
        }

        size_t Size() const
        {
            return mComponentList.size();
        }

        typename typedef std::vector<CompType_t *>::iterator it_C;
        typename typedef std::vector<CompType_t *>::const_iterator cit_C;

        std::vector<CompType_t *> mComponentList;
        it_C mCurrComponent;
    };

    typedef ComponentListCallback<IComponent> ComponentsGetter_t;

    template <typename T, typename V>
    struct VisibleComponentCallback : public ComponentListCallback<T>
    {
        VisibleComponentCallback(const V *pView) 
        : ComponentListCallback()
        , mpView(pView)
        {}

        virtual bool ProcessComponent(CompType_t *pComponent)
        {
            // \notr add support to filter model components which are not in view
            if (1)//mpView && pComponent)
            {
                return ComponentListCallback<T>::ProcessComponent(pComponent);
            }

            return true;
        }

        const V *mpView;
    };

    //-----------------------------------------------------------------------------------------------
    /*!
    This section provides some helper templates to easy creation and registration
    of derived components.
    */

    //-----------------------------------------------------------------------------------------------
    /*!
    This is our Component allocator, which can be customized
    to provide different memory management.
    */
    template <typename T> class ComponentAllocator
    {
    public:
        //! Return new instance of this class
        static IComponent *New(component::ObjectId id)
        {
            return new T(id);
        }

        //! Delete instance
        static bool Delete(IComponent *This)
        {
            This->DeleteThis();
            return true;
        }
    };

    //-----------------------------------------------------------------------------------------------
    /*!
    This template should be derived into IComponent derived class. It provides
    all the New and Delete method using ComponentAllocator template and methods to
    register the component with ObjectManager.

    usage:
    \code
    // header file
    class ComponentEntity : public IComponentEntity, public ComponentHelper<ComponentEntity>
    {
    public:
        // some methods for this component...
        // ...

        // This must be provided in your derived class, since it's used for registration
        // with ObjectMananger. On the side note, it does not matter
        // if you return TRUE or FALSE, since it's only used to make this function
        // to be called from within macro EG_REGISTER_COMPONENT defined below.
        static bool RegisterComponentType();
    };

    // optionally, you can provide your own allocator to manage creation and deletion of components
    // where EntityAllocator conforms to ComponentAllocator template signature
    class ComponentEntity : public IComponentEntity, public ComponentHelper<ComponentEntity, EntityAllocator>
    {
    public:
        // ...
    };

    // cpp file
    EG_REGISTER_COMPONENT(ComponentEntity);

    bool ComponentEntity::RegisterComponentType()
    {
        InitComponentType(ComponentType::kEntity, "Entity");

        // and optionally
        eg::GetObjectManager().SubscribeToMessageType(ComponentType::kEntity, MType::kComponentNew);

        return true;
    }
    \endcode
    */
    template <typename T, typename Property = PropertySetNull, typename Allocator = ComponentAllocator<T> >
    class ComponentHelper
    {
    protected:
        IPropertySet& GetPropertyData() {return mProperty;}
        const ObjectInstance& OI() const {return mObjectInstance;}

        typedef ComponentHelper<T, Property, Allocator> CH_t;
        //! This is only used to specify extra parameter to allow callback method
        //! when property is changed
        enum ChangedCallback {ccEmpty, ccAttached};

        /*!
        Initialize this object
        \param groupName name of this property set
        \param changedCallback if ccAttached, then provide callback method which derive class can override
               otherwise if ccEmpty, then there is no callback method attached to property set.
        */
        ComponentHelper(component::ObjectId id, const std::string& groupName, ChangedCallback changedCallback)
        : mProperty(component::InstanceId(id, component::ComponentId(groupName.c_str())), groupName, (changedCallback == ccAttached) ? boost::bind(&ComponentHelper::propertyChanged, this, _1) : PropertyChangedCallback_t())
        , mObjectInstance(REGISTRATE(IObjectManager).OI(id))
        {
        }

        static void InitComponentType(Hash compID, const wxChar *pReadableName)
        {
            REGISTRATE(IObjectManager).RegisterComponentType(compID, Allocator::New, Allocator::Delete, pReadableName);
        }

        Property mProperty;

    private:
        //! This can be overridden in derive class to provide custom handling of property changes
        virtual void onPropertyChanged(const IProperty * /*pProperty*/) {}
        void propertyChanged(const IProperty *pProperty)
        {
            onPropertyChanged(pProperty);
        }

        const ObjectInstance& mObjectInstance;
    };


    //-----------------------------------------------------------------------------------------------
    //! This must added to cpp file to allow automatic registration with ObjectManager
    #define EG_REGISTER_COMPONENT(classType) static bool dummy##classType = classType::RegisterComponentType()

    class PropertyNull : public IProperty
    {
    public:
        virtual ~PropertyNull() {}
        virtual uint32_t GetPropId() const {return 0;}
        virtual const std::string& GetName() const {static std::string empty("Null"); return empty;}
        virtual std::string GetValue() const {return "";}
        virtual void SetValue(const std::string&) {}
        virtual bool IsReadOnly() const {return true;}
        virtual const std::type_info& GetPropType() const {return typeid(bool);}
		virtual const std::string& GetPropTypeOptions() const {static std::string dummy; return dummy;}
        virtual const std::type_info& GetPropValue() const {return typeid(bool);}
        virtual component::InstanceId GetInstanceId() const {return component::InstanceId();}
        virtual PropertyChangedCallback_t SetChangeCallback(PropertyChangedCallback_t) {return PropertyChangedCallback_t();}
        IProperty& operator =(const std::string&) {return *this;}
        operator std::string() const {return "";}
    };

    //-----------------------------------------------------------------------------------------------
    //! Null property set to use as a default template value for components
    class PropertySetNull : public IPropertySet
    {
    public:
        virtual bool IsValid() const {return false;}
        virtual const std::string& GetGroupName() const {static std::string empty; return empty;}
        virtual void SetGroupName(const std::string&) {}
        virtual IProperty *GetFirstProperty() const {return 0;}
        virtual IProperty *GetNextProperty() const {return 0;}
        virtual IProperty *FindProperty(const std::string&) const {return 0;}
        virtual component::InstanceId GetInstanceId() const {return component::InstanceId();}
		virtual void AddNotifySlot(const IPropertySet::NotifySlot_t&, const std::string&) {}
        virtual void RemoveNotifySlot(const std::string&) {}
        virtual bool operator ==(const IPropertySet&) const {return false;}
        virtual bool operator !=(const IPropertySet&) const {return true;}
        virtual IProperty& operator[](const std::string&) {static PropertyNull empty; return empty;}
        virtual const IProperty& operator[](const std::string&) const {static PropertyNull empty; return empty;}
    };


    namespace property
    {
        //-----------------------------------------------------------------------------------------------
        //! Return PropertyData for requested component instance.
        inline IPropertySet& cast(const component::InstanceId& iId)
        {
            if (IComponent *pComponent = component::get(iId))
            {
                return pComponent->GetPropertyData();
            }

            static PropertySetNull dummyValue;
            return dummyValue;
        }

        //-----------------------------------------------------------------------------------------------
        //! Return pointer to property using specific name
        // \return NULL if there was no property of that name or there is no valid IPropertySet.
        inline IProperty *cast(const component::InstanceId& iId, const std::string& propName)
        {
            IPropertySet& propSet = cast(iId);
            if (propSet.IsValid())
            {
                return propSet.FindProperty(propName);
            }

            return 0;
        }

		/*
        template <typename T>
        Property<T>& prop_cast(const component::InstanceId& iId, const std::string& propName)
        {
            if (IProperty *pProperty = cast(iId, propName))
            {
                if (pProperty->IsValue<T>())
                {
                    return static_cast<Property<T>&>(*pProperty);
                }
            }

            static BlankProperty<T> blankProp;
            return blankProp;
        }

        template <typename T>
        Property<T>& prop_cast(IComponent *pComponent, const std::string& propName)
        {
            return prop_cast<T>(pComponent->GetPropertyData(), propName);
        }
		*/


        /*!
         This class servs as a handle for component properties.
         It allows us to create it on the stack with instance id
         and use [] operato to get and set properties
         \code
         property::hProp entity(iId);
         entity["Position"] = Vector3(1, 4, 6);
         Vector3 pos = entity["Position"];
         // or ...
         property::hProp entity(iId);
         if (entity.IsValid())
         {
             entity["Position"] = Vector3(1, 4, 6);
             Vector3 pos = entity["Position"];
         }
         \endcode
         */
        class hProp
        {
        public:
            hProp(const component::InstanceId& iId)
            : mId(iId)
            {
            }

            virtual IProperty& operator[](const std::string& propName)
            {
                if (IComponent *pEntity = component::get(mId))
                {
                    IPropertySet& propSet = pEntity->GetPropertyData();
                    return propSet[propName];
                }

                static PropertyNull propertyNull;
                return propertyNull;
            }

            virtual const IProperty& operator[](const std::string& propName) const
            {
                if (IComponent *pEntity = component::get(mId))
                {
                    IPropertySet& propSet = pEntity->GetPropertyData();
                    return propSet[propName];
                }

                static PropertyNull propertyNull;
                return propertyNull;
            }

            bool IsValid() const
            {
                return component::get(mId) != 0;
            }

        private:
            component::InstanceId mId;
        };

    } // namespace property


} // namespace eg

#endif //I_COMPONENT_H
