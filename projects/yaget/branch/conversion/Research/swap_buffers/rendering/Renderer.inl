//--------------------------------------------------------------------------------------------------------------------//
//                                                RENDERER BASE CLASS                                                 //
// 04/05/02, Mythos                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_RENDERER_INL_
#define   _MYTHOS_RENDERER_INL_
#pragma    once

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE Mythos::CRenderer::CRenderer()
  {
  m_boInit = false;
  m_u32CurrentChain = 0;
  m_pCurrentChain   = NULL;
  }

INLINE Mythos::CRenderer::~CRenderer()
  {
  Destroy();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString Mythos::CRenderer::GetSignature() const
  {
  return "NO RENDERER";
  }

INLINE bool Mythos::CRenderer::IsInitialized() const
  {
  return m_boInit;
  }

INLINE Mythos::CSwapChain* Mythos::CRenderer::GetCurrentChain() const
  {
  return m_pCurrentChain;
  }

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_RENDERER_INL_
