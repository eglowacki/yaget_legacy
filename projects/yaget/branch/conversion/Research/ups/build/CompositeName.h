#ifndef __COMPOSITENAME_H_INCL__
#define __COMPOSITENAME_H_INCL__

#include <list>

class CompositeNameBase;

typedef std::list<CompositeNameBase*> CompositeNameCollection;
typedef std::list<CompositeNameBase*>::const_iterator CompositeNameIter;

/**
 * Any object that is to be stored in the composite tree 
 * structure must implement this interface.
 * 
 * @author Administrator
 */
class CompositeNameBase
{
protected:
    CompositeNameBase(){}

public:
    virtual void MemberFunction() = 0;
    virtual int get_NumberProperty() = 0;
    virtual void set_NumberProperty(int value) = 0;
    virtual int get_ChildCount() = 0;
    virtual CompositeNameIter Children() = 0;
    virtual int AddChild(CompositeNameBase* child) = 0;
    virtual int RemoveChild(CompositeNameBase* child) = 0;
};

/**
 * This class provides a standard implementation of the Component
 * class (leaf) as part of the Composite design pattern.  Objects 
 * of this type may not contain children.
 */
class CompositeNameComponent : public CompositeNameBase
{
public:
    CompositeNameComponent() : _numberPropertyValue(0){}
    CompositeNameComponent(int value) : _numberPropertyValue(value){}
private:

    int _numberPropertyValue;
public:
    // Base class overrides
    void MemberFunction();
    int get_NumberProperty();
    void set_NumberProperty(int value);
    int get_ChildCount();
    CompositeNameIter Children();
    int AddChild(CompositeNameBase* child);
    int RemoveChild(CompositeNameBase* child);
};

/**
 * This class provides a standard implementation of the 
 * Composite class (node) as part of the Composite 
 * design pattern.  A Composite node may have one or
 * many children, but a Component (leaf) may not.
 */
class CompositeNameComposite : public CompositeNameBase
{
public:
    CompositeNameComposite(){}
private:
    CompositeNameCollection _children;
public:
    // Base class overrides
    void MemberFunction();
    int get_NumberProperty();
    void set_NumberProperty(int value);
    int get_ChildCount();
    CompositeNameIter Children();
    int AddChild(CompositeNameBase* child);
    int RemoveChild(CompositeNameBase* child);
};


#endif // __COMPOSITENAME_H_INCL__

