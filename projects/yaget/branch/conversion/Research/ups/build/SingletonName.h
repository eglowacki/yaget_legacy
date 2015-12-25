#ifndef __SINGLETONNAME_H_INCL__
#define __SINGLETONNAME_H_INCL__

/**
 * TODO: Add class description
 * 
 * @author   Administrator
 */
class SingletonName
{
public:
    // Method to fetch singleton instance.
    static SingletonName* getInstance();

    // Destructor
    virtual ~SingletonName();

protected:
    // Constructor - protected so users cannot call it.
    SingletonName();

private: 
    // private static member referencing the single instance of the object
    // TODO: Replace SingletonName with the type of object you want to
    // return the single instance of
    static SingletonName* _instance;

    // Copy constructor
    // Declared but not defined to prevent auto-generated
    // copy constructor.  Refer to "Effective C++" by Meyers
    SingletonName(const SingletonName& src);

    // Assignment operator
    // Declared but not defined to prevent auto-generated
    // assignment operator.  Refer to "Effective C++" by Meyers
    SingletonName& operator=(const SingletonName& src);

};

// Constructor implementation
inline SingletonName::SingletonName()
{
}

// Destructor implementation
inline SingletonName::~SingletonName()
{
}

// TODO: Uncomment the copy constructor when you need it.
//inline SingletonName::SingletonName(const SingletonName& src)
//{
//   // TODO: copy
//}

// TODO: Uncomment the assignment operator when you need it.
//inline SingletonName& SingletonName::operator=(const SingletonName& rhs)
//{
//   if (this == &rhs) {
//      return *this;
//   }
//
//   // TODO: assignment
//
//   return *this;
//}

#endif // __SINGLETONNAME_H_INCL__
