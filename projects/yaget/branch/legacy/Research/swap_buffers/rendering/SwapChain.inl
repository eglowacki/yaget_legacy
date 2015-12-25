//--------------------------------------------------------------------------------------------------------------------//
//                                                SWAP CHAIN CLASS                                                    //
//                                                (MULTIPLE VIEWS)                                                    //
// 22/05/02, Mythos                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_SWAPCHAIN_INL_
#define   _MYTHOS_SWAPCHAIN_INL_
#pragma    once

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE Mythos::CSwapChain::CSwapChain(u32 u32Handle,bool boWindowed,bool boZBuffer,HWND hWnd,u32 u32Width,u32 u32Height)
  {
  m_u32Handle  = u32Handle;
  m_boWindowed = boWindowed;
  m_boZBuffer  = boZBuffer;
  m_hWnd       = hWnd;
  m_u32Width   = u32Width;
  m_u32Height  = u32Height;

  m_v3ClearColor.Set(0.5f);
  }

INLINE Mythos::CSwapChain::~CSwapChain()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE u32 Mythos::CSwapChain::GetFreeHandle()
  {
  return m_u32NextHandle++;
  }

INLINE u32 Mythos::CSwapChain::GetHandle() const
  {
  return m_u32Handle;
  }

INLINE bool Mythos::CSwapChain::IsWindowed() const
  {
  return m_boWindowed;
  }

INLINE bool Mythos::CSwapChain::HasZBuffer() const
  {
  return m_boZBuffer;
  }

INLINE HWND Mythos::CSwapChain::GetHwnd() const
  {
  return m_hWnd;
  }

INLINE u32 Mythos::CSwapChain::GetWidth() const
  {
  return m_u32Width;
  }

INLINE u32 Mythos::CSwapChain::GetHeight() const
  {
  return m_u32Height;
  }

INLINE Mythos::CVect3D Mythos::CSwapChain::GetClearColor() const
  {
  return m_v3ClearColor;
  }

INLINE void Mythos::CSwapChain::SetClearColor(const CVect3D& v3Color)
  {
  m_v3ClearColor = v3Color;
  }

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_SWAPCHAIN_INL_
