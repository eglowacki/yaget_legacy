//--------------------------------------------------------------------------------------------------------------------//
//                                                DX9 SWAP CHAIN CLASS                                                //
//                                                (MULTIPLE VIEWS)                                                    //
// 22/05/02, Mythos                                                                                                   //
// 31/07/03: DX9
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_SWAPCHAINDX9_INL_
#define   _MYTHOS_SWAPCHAINDX9_INL_
#pragma    once

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE Mythos::CSwapChainDX9::CSwapChainDX9(u32 u32Handle,bool boWindowed,bool boZBuffer,HWND hWnd,u32 u32Width,u32 u32Height) :
                              CSwapChain(u32Handle,boWindowed,boZBuffer,hWnd,u32Width,u32Height)
  {
  m_pInterface  = NULL;
  m_pBackBuffer = NULL;
  m_pZStencil   = NULL;
  }

INLINE Mythos::CSwapChainDX9::~CSwapChainDX9()
  {
  if(m_pBackBuffer) m_pBackBuffer->Release();
  if(m_pZStencil)   m_pZStencil  ->Release();
  if(m_pInterface)  m_pInterface ->Release();               // order is important to avoid D3D warnings in output

  m_pInterface  = NULL;
  m_pBackBuffer = NULL;
  m_pZStencil   = NULL;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE IDirect3DSwapChain9* Mythos::CSwapChainDX9::GetInterface() const
  {
  return m_pInterface;
  }

INLINE void Mythos::CSwapChainDX9::SetInterface(IDirect3DSwapChain9* pInterface)
  {
  if(m_pInterface) m_pInterface->Release();
  m_pInterface = pInterface;
  }

INLINE IDirect3DSurface9* Mythos::CSwapChainDX9::GetBackBuffer() const
  {
  return m_pBackBuffer;
  }

INLINE void Mythos::CSwapChainDX9::SetBackBuffer(IDirect3DSurface9* pBackBuffer)
  {
  if(m_pBackBuffer) m_pBackBuffer->Release();
  m_pBackBuffer = pBackBuffer;
  }

INLINE IDirect3DSurface9* Mythos::CSwapChainDX9::GetZStencil() const
  {
  return m_pZStencil;
  }

INLINE void Mythos::CSwapChainDX9::SetZStencil(IDirect3DSurface9* pZStencil)
  {
  if(m_pZStencil) m_pZStencil->Release();
  m_pZStencil = pZStencil;
  }

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_SWAPCHAINDX9_INL_
