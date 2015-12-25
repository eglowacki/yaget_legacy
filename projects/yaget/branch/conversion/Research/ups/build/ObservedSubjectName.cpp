#include "ObservedSubjectName.h"

/**
 * Default constructor
 */
ObservedSubjectName::ObservedSubjectName()
{
    // TODO: Add constructor code here
}

/**
 * Destructor
 */
ObservedSubjectName::~ObservedSubjectName()
{
    // TODO: Add destructor code here
}

/**
 * Adds an observer to the internal notification list.
 * An observer is any object of a class that implements 
 * the IObservedSubjectNameObserver interface.
 * 
 * @param o An object that wants to be added to the 
 *          notification list
 */
void ObservedSubjectName::Attach (IObservedSubjectNameObserver* o) {
    // add the observer to the list
    _observers.push_back(o);
}

/**
 * Removes an observer from the internal notification list.
 * 
 * @param o An object that wants to be removed from the 
 *          notification list
 */
void ObservedSubjectName::Detach (IObservedSubjectNameObserver* o) {
    int count = (int)_observers.size();
    int i;

    // iterate over the list of observers and find the one that 
    // is requesting to be removed
    for (i = 0; i < count; i++)
    {
        // see if we've found it.  If so, exit the loop
        if (_observers[i] == o)
            break;
    }
    // determine if the observer was found.  If so, then 
    // remove it from the list
    if (i < count)
        _observers.erase(_observers.begin() + i);
}

/**
 * This is an example of a function that notifies all observers.
 * The list of observers is traversed and Update is called on each.
 */
void ObservedSubjectName::Notify () {
    int count = (int)_observers.size();
    // iterate over the list of observers
    for (int i = 0; i < count; i++)
    {
        // call Update on each, passing a reference to this instance
        (_observers[i])->Update(this);
    }
}



