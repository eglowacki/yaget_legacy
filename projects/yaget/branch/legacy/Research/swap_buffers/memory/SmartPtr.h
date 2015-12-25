//--------------------------------------------------------------------------------------------------------------------//
//                                                SMART POINTER TEMPLATE                                              //
// 18/06/00, Mythos                                                                                                   //
//      rem: smart pointer to T : T must inherit from CRefCount                                                       //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_SMARTPTR_H_
#define   _MYTHOS_SMARTPTR_H_
#pragma    once

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                TSmartPtr                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace Mythos
{
  template <class CObj> class TSmartPtr
  {
  // public methods

  public:
                    TSmartPtr           (CObj* pObj=NULL);
                    TSmartPtr           (const TSmartPtr& Smart);
                   ~TSmartPtr           (void);

    // operators

    TSmartPtr&      operator =          (const TSmartPtr& Smart);
    TSmartPtr&      operator =          (CObj* pObj);

    CObj*           operator ->         (void) const;
    CObj&           operator *          (void) const;
    operator        CObj*               (void) const;

    bool            operator ==         (CObj* pObj) const;
    bool            operator !=         (CObj* pObj) const;
    bool            operator ==         (const TSmartPtr& Smart) const;
    bool            operator !=         (const TSmartPtr& Smart) const;

    // dumb pointer

    CObj*           GetDumb             (void) const;

  // protected members

  protected:

    CObj*           m_pDumbPtr;                             // pointed object
  };
}

//----------------------------------------------- MACROS -------------------------------------------------------------//

// _SMART_POINTER_(Class); => defines the ClassSP type (smart ptr to an object of class 'Class')

#define   _SMART_POINTER_(Class)  \
  class   Class;                  \
  typedef Mythos::TSmartPtr<Class> Class##SP;

//----------------------------------------------- INLINES ------------------------------------------------------------//

#include  "SmartPtr.inl"

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_SMARTPTR_H_
