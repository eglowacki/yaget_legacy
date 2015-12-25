//--------------------------------------------------------------------------------------------------------------------//
//                                                SWAP CHAIN CLASS                                                    //
//                                                (MULTIPLE VIEWS)                                                    //
// 22/05/02, Mythos                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_SWAPCHAIN_H_
#define   _MYTHOS_SWAPCHAIN_H_
#pragma    once

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Maths/VectMat.h"

#include  "Global/Defines.h"
#include  "Memory/SmartPtr.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CSwapChain                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace Mythos
{
  class CSwapChain
  {
  // public methods

  public:

    // constructors & destructor

                              CSwapChain          (u32 u32Handle,bool boWindowed,bool boZBuffer,HWND hWnd,u32 u32Width,u32 u32Height);
    virtual                  ~CSwapChain          (void);

    // get/set

    static u32                GetFreeHandle       (void);
    u32                       GetHandle           (void) const;

    bool                      IsWindowed          (void) const;
    bool                      HasZBuffer          (void) const;

    HWND                      GetHwnd             (void) const;
    u32                       GetWidth            (void) const;
    u32                       GetHeight           (void) const;

    CVect3D                   GetClearColor       (void) const;
    void                      SetClearColor       (const CVect3D& v3Color);

  // protected data

  protected:

    static u32                m_u32NextHandle;
    u32                       m_u32Handle;

    bool                      m_boWindowed;
    bool                      m_boZBuffer;

    HWND                      m_hWnd;
    u32                       m_u32Width,m_u32Height;

    CVect3D                   m_v3ClearColor;
  };

  // smart ptr

  _SMART_POINTER_(CSwapChain)
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "SwapChain.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_SWAPCHAIN_H_
