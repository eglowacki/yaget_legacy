#ifndef __OBSERVEDSUBJECTNAME_H_INCL__
#define __OBSERVEDSUBJECTNAME_H_INCL__

#include <list>
#include <vector>

using namespace std;

class ObservedSubjectName;

/**
 * IObservedSubjectNameObserver defines the interface
 * that all observerss must implement to receive
 * notifications of updates to the subject.
 * 
 * @author   Administrator
 */
class IObservedSubjectNameObserver
{
public:
    // generic update function
    virtual void Update(ObservedSubjectName* subject) = 0;
    // TODO: define any other notification functions
    // for observing subjects.  This is an interface, 
    // so all functions must be pure virtual ones.
};

/**
 * ObservedSubjectName defines the class that all
 * observers will be observing.  It maintains 
 * an internal vector of listening subjects. 
 * The subject does not know what type of objects 
 * are observing it, only that they all implement 
 * the IObservedSubjectNameObserver interface.
 */
class ObservedSubjectName
{
public:
    // Constructor
    ObservedSubjectName();

    // Destructor
    virtual ~ObservedSubjectName();

    void Attach(IObservedSubjectNameObserver*);
    void Detach(IObservedSubjectNameObserver*);
    void Notify();

private:
    vector<IObservedSubjectNameObserver*> _observers;
};

#endif // __OBSERVEDSUBJECTNAME_H_INCL__
