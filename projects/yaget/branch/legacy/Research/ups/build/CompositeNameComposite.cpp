#include "CompositeName.h"

/**
 * Example of a member function that may be performed on 
 * this object. 
 * TODO: Rename and implement this function
 */
void CompositeNameComposite::MemberFunction() {
    CompositeNameIter it;
    // iterate over each child
    for (it = _children.begin(); it != _children.end(); ++it)
    {
        // call that child's member function
        CompositeNameBase* child = *it;
        child->MemberFunction();
    }
}

/**
 * Example number property getter.  Simply gets the number
 * property from each child object and adds them.
 * 
 * @return   The sum of all numbers contained in the leaves
 *         under this node in the composite tree.
 */
int CompositeNameComposite::get_NumberProperty() {
    int sum = 0;
    CompositeNameIter it;
    // traverse the children
    for (it = _children.begin(); it != _children.end(); ++it)
    {
        // sum their values
        CompositeNameBase* child = *it;
        sum += child->get_NumberProperty();
    }
    return sum;
}

/**
 * Example of a numeric property setter
 */
void CompositeNameComposite::set_NumberProperty(int value) {
    // No-op
}

/**
 * Returns the number of children contained by this composite node
 */
int CompositeNameComposite::get_ChildCount() {
    return(int)_children.size();
}

/**
 * Returns an iterator for the child collection
 */
CompositeNameIter CompositeNameComposite::Children() {
    return _children.begin();
}

/**
 * Adds a new child to the node
 * 
 * @param child A new child to be added to the child collection
 * 
 * @return The number of children owned by this object
 */
int CompositeNameComposite::AddChild(CompositeNameBase* child) {
    _children.push_back(child);
    return(int)_children.size();
}

/**
 * Removes a specific child from the node
 * 
 * @param child The child object to be removed from the child
 *              collection
 * 
 * @return The number of children owned by this object
 */
int CompositeNameComposite::RemoveChild(CompositeNameBase* child) {
    _children.remove(child);
    return(int)_children.size();
}


