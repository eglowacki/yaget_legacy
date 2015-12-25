//--------------------------------------------------------------------------------------------------------------------//
//                                                RENDERER INTERFACE                                                  //
// 04/05/02, Mythos                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_RENDERER_H_
#define   _MYTHOS_RENDERER_H_
#pragma    once

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Global/StlIncludes.h"

#include  "Containers/StdString.h"
#include  "Global/Defines.h"
#include  "Maths/VectMat.h"
#include  "Memory/SmartPtr.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace Mythos
{
  class CEngineCamera;
  class CEngineMaterial;
  class CEngineMesh;

  class CIBuffer;
  class CSwapChain;
  class CVBuffer;

  class CMat4x4;
  class CVect3D;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                IRenderer                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace Mythos
{
  class IRenderer
  {
  // public enums

  public:

    typedef enum
      {
      _WINDOWED_              = 1 << 0,
      _ZBUFFER_               = 1 << 1,
      } RENDERER_CREATE;

    typedef enum
      {
      _CLEAR_BACK_            = 1 << 0,
      _CLEAR_DEPTH_           = 1 << 1,
      _CLEAR_STENCIL_         = 1 << 2,

      _CLEAR_BACKDEPTH_       = _CLEAR_BACK_ | _CLEAR_DEPTH_,
      _CLEAR_ALL_             = 0xffffffff,
      } RENDERER_CLEAR;

    typedef enum
      {
      _TRILIST_               = 0,
      _LINELIST_,
      } RENDERER_PRIMITIVE;

  // public methods

  public:

    // constructors & destructor

                              IRenderer           (void) {}
    virtual                  ~IRenderer           (void) {}

    // basic ops

    virtual bool              Create              (const HWND hWnd,const u32 u32Flags=_WINDOWED_|_ZBUFFER_,
                                                   const u32 u32Width=0,const u32 u32Height=0) = 0;
    virtual void              Destroy             (void) = 0;

    virtual u32               CreateSwapChain     (const HWND hWnd,const u32 u32Flags=_WINDOWED_|_ZBUFFER_,
                                                   const u32 u32Width=0,const u32 u32Height=0) = 0;
    virtual u32               ResizeSwapChain     (const u32 u32Chain,const u32 u32Width=0,const u32 u32Height=0) = 0;
    virtual void              ReleaseSwapChain    (const u32 u32Chain) = 0;
    virtual bool              SelectSwapChain     (const u32 u32Chain) = 0;
    virtual bool              AddSwapChain        (CSwapChain* pSwapChain) = 0;
    virtual bool              RemoveSwapChain     (CSwapChain* pSwapChain) = 0;

    virtual bool              BeginScene          (void) = 0;
    virtual bool              EndScene            (void) = 0;
    virtual bool              Clear               (const u32 u32Flags=_CLEAR_BACKDEPTH_) = 0;
    virtual bool              Swap                (const RECT* pSrcRect=NULL,const RECT* pDestRect=NULL,const u32 u32Vbls=0) = 0;

    // get

    virtual CStdString        GetSignature        (void) const = 0;
    virtual bool              IsInitialized       (void) const = 0;
    virtual CSwapChain*       GetSwapChain        (const u32 u32Chain) = 0;
    virtual CSwapChain*       GetCurrentChain     (void) const = 0;

    virtual CVect3D           GetClearColor       (void) = 0;
    virtual bool              SetClearColor       (const CVect3D& v3Color) = 0;

    // draw

    virtual u32               DrawTeapot          (void) = 0;

    // trf

    virtual bool              SetModel2World      (const CMat4x4& m4Matrix) = 0;
    virtual bool              SetWorld2View       (const CMat4x4& m4Matrix) = 0;
    virtual bool              SetPerspective      (const float fFov,const float fNear,const float fFar,const float fRatio=4.f/3.f) = 0;
    virtual bool              SetOrtho            (const CVect3D& v3Min,const CVect3D& v3Max) = 0;
  };

  // smart ptr

  _SMART_POINTER_(IRenderer)
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CRenderer                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace Mythos
{
  class CRenderer : public IRenderer
  {
  // typedefs

  public:

    typedef std::list< CSwapChain* >              listChain;
    typedef listChain::iterator                   iterChain;

  // public methods

  public:

    // constructors & destructor

                              CRenderer           (void);
    virtual                  ~CRenderer           (void);

    // basic ops

    virtual void              Destroy             (void);
    virtual bool              AddSwapChain        (CSwapChain* pSwapChain);
    virtual bool              RemoveSwapChain     (CSwapChain* pSwapChain);

    // get

    virtual CStdString        GetSignature        (void) const;
    virtual bool              IsInitialized       (void) const;
    virtual CSwapChain*       GetSwapChain        (const u32 u32Chain);
    virtual CSwapChain*       GetCurrentChain     (void) const;

    virtual CVect3D           GetClearColor       (void);
    virtual bool              SetClearColor       (const CVect3D& pfColor);

  // protected data

  protected:

    bool                      m_boInit;
    listChain                 m_SwapChains;
    u32                       m_u32CurrentChain;
    CSwapChain*               m_pCurrentChain;

    bool                      m_boLighted;
    bool                      m_boZTest;
    bool                      m_boTexture;
  };

  // smart ptr

  _SMART_POINTER_(CRenderer)
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "Renderer.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_RENDERER_H_
