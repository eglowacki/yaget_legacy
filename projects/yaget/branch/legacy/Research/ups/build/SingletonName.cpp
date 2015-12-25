#include "SingletonName.h"

SingletonName* SingletonName::_instance = 0;

/**
 * Returns the single instance of the object.
 */
SingletonName* SingletonName::getInstance() {
    // check if the instance has been created yet
    if (0 == _instance)
    {
        // if not, then create it
        _instance = new SingletonName;
    }
    // return the single instance
    return _instance;
};


