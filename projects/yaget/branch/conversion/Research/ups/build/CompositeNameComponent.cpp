#include "CompositeName.h"

/**
 * Example of a member function that may be performed on 
 * this object.
 */
void CompositeNameComponent::MemberFunction() {
    // TODO: Component method implementation
}

/**
 * Example of a numeric property getter
 */
int CompositeNameComponent::get_NumberProperty() {
    return _numberPropertyValue;
}

/**
 * Example of a numeric property setter
 */
void CompositeNameComponent::set_NumberProperty(int value) {
    _numberPropertyValue = value;
}

/**
 * A component has no children, so return 0
 */
int CompositeNameComponent::get_ChildCount() {
    return 0;
}

/**
 * A component has no children, so return null
 */
CompositeNameIter CompositeNameComponent::Children() {
    return 0;
}

/**
 * AddChild has no implementation in the component class
 */
int CompositeNameComponent::AddChild(CompositeNameBase* child) {
    return 0;
}

/**
 * RemoveChild has no implementation in the component class
 */
int CompositeNameComponent::RemoveChild(CompositeNameBase* child) {
    return 0;
}


