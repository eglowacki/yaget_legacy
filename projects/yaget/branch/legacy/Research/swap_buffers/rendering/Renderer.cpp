//--------------------------------------------------------------------------------------------------------------------//
//                                                RENDERER BASE CLASS                                                 //
// 04/05/02, Mythos                                                                                                   //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "Renderer.h"
#ifdef    _DEBUG
#include  "Renderer.inl"
#endif

#include  "SwapChain.h"

//--------------------------------------------------------------------------------------------------------------------//
//                                                BASIC OPS                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Destroy ------------------------------------------------------------//
// delete & remove all swap chains
// 22/05/02, Mythos
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void Mythos::CRenderer::Destroy()
  {
  iterChain iter = m_SwapChains.begin();

  while(iter != m_SwapChains.end())
    {
    delete (*iter);
    ++iter;
    }

  m_SwapChains.clear();
  m_pCurrentChain = NULL;
  }

//----------------------------------------------- AddSwapChain -------------------------------------------------------//
// add swap chain to list
// 23/05/02, Mythos
// in : swap chain
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRenderer::AddSwapChain(CSwapChain* pSwapChain)
  {
  if(!pSwapChain) return false;

  iterChain iter = std::find(m_SwapChains.begin(),m_SwapChains.end(),pSwapChain);
  if(iter != m_SwapChains.end()) return false;              // already in the list

  m_SwapChains.push_back(pSwapChain);
  return true;
  }

//----------------------------------------------- RemoveSwapChain ----------------------------------------------------//
// remove swap chain from list
// 23/05/02, Mythos
// in : swap chain
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRenderer::RemoveSwapChain(CSwapChain* pSwapChain)
  {
  iterChain iter = std::find(m_SwapChains.begin(),m_SwapChains.end(),pSwapChain);
  if(iter == m_SwapChains.end()) return false;

  m_SwapChains.remove(pSwapChain);

  if(pSwapChain == m_pCurrentChain)
    {
    m_pCurrentChain   = NULL;
    m_u32CurrentChain = 0;
    }
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- GetSwapChain -------------------------------------------------------//
// return swap chain object by handle
// 22/05/02, Mythos
// in : chain handle
// out: chain object
//--------------------------------------------------------------------------------------------------------------------//

Mythos::CSwapChain* Mythos::CRenderer::GetSwapChain(const u32 u32Chain)
  {
  iterChain iter = m_SwapChains.begin();

  while(iter != m_SwapChains.end())
    {
    if((*iter)->GetHandle() == u32Chain) return *iter;
    ++iter;
    }

  return NULL;
  }

//----------------------------------------------- GetClearColor ------------------------------------------------------//
// return clear color of current swap chain
// 24/05/02, Mythos
// in :
// out: color
//--------------------------------------------------------------------------------------------------------------------//

Mythos::CVect3D Mythos::CRenderer::GetClearColor()
  {
  if(!m_pCurrentChain) return NULL;
  return m_pCurrentChain->GetClearColor();
  }

//----------------------------------------------- SetClearColor ------------------------------------------------------//
// set clear color of current swap chain
// 24/05/02, Mythos
// in : color
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRenderer::SetClearColor(const CVect3D& v3Color)
  {
  if(!m_pCurrentChain) return false;
  m_pCurrentChain->SetClearColor(v3Color);
  return true;
  }
