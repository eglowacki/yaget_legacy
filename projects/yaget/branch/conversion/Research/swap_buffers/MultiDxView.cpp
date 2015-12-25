// MultiDxView.cpp : implementation of the CMultiDxView class
//

#include "stdafx.h"
#include "MultiDx.h"

#include "MultiDxDoc.h"
#include "MultiDxView.h"

#include "Rendering/Renderer.h"
#include "Rendering/SwapChain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiDxView

IMPLEMENT_DYNCREATE(CMultiDxView, CView)

BEGIN_MESSAGE_MAP(CMultiDxView, CView)
	//{{AFX_MSG_MAP(CMultiDxView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiDxView construction/destruction

CMultiDxView::CMultiDxView()
{
  m_u32Renderer = 0;
}

CMultiDxView::~CMultiDxView()
{
  if(m_u32Renderer && theApp.m_pRenderer) theApp.m_pRenderer->ReleaseSwapChain(m_u32Renderer);
}

BOOL CMultiDxView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMultiDxView drawing

void CMultiDxView::OnDraw(CDC* pDC)
{
	CMultiDxDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMultiDxView diagnostics

#ifdef _DEBUG
void CMultiDxView::AssertValid() const
{
	CView::AssertValid();
}

void CMultiDxView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMultiDxDoc* CMultiDxView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMultiDxDoc)));
	return (CMultiDxDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMultiDxView message handlers

BOOL CMultiDxView::OnEraseBkgnd(CDC* pDC) 
{
  DrawFrame();
  return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}

//

void CMultiDxView::OnInitialUpdate() 
  {
	CView::OnInitialUpdate();

  if(!CreateRenderer())
    {
    _ASSERT_(false,"OnInitialUpdate : CreateRenderer failed");
    if(m_u32Renderer) theApp.m_pRenderer->ReleaseSwapChain(m_u32Renderer);
    m_u32Renderer = 0;
    return;
    }
  }

//

void CMultiDxView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
  {
  switch(lHint)
    {
    case _IDLE_ :
      {
      DrawFrame();
      break;
      }
    }
  }

//

void CMultiDxView::DrawFrame()
  {
  if(!m_u32Renderer || !theApp.m_pRenderer) return;

  Mythos::IRenderer* pRenderer = theApp.m_pRenderer;
  if(!pRenderer->SelectSwapChain(m_u32Renderer))
    {
    _ASSERT_(false,"DrawFrame : SelectSwapChain failed");
    return;
    }

  // rotate mesh

  Mythos::CMat4x4 m4Y;
  m4Y.RotationY(GetDocument()->m_fAngleY);
  pRenderer->SetModel2World(m4Y);

  // camera

  Mythos::CMat4x4 m4W2C;
  Mythos::CMat4x4 m4Rot = Mythos::CMat4x4::Identity;
  m4W2C.Translation(Mythos::CVect3D(0.f,0.f,-5.f));

  switch(m_u32Renderer)
    {
    case 1 :
             break;
    case 2 : m4Rot.RotationY(-_PI_/2.f);
             break;
    case 3 : m4Rot.RotationX(-_PI_/2.f);
             break;
    case 4 : m4Rot.RotationXYZ(Mythos::CVect3D(-_PI_/4.f,_PI_/6.f,_PI_/4.f));
             break;
    }

  m4W2C *= m4Rot;
  pRenderer->SetWorld2View(m4W2C);

  // perspective

  RECT rect;
  GetClientRect(&rect);
  pRenderer->SetPerspective(_DEG2RAD_(60.f),1.f,1000.f,(float)rect.right/rect.bottom);

  // draw

  _ASSERT_(pRenderer->Clear(),     "DrawFrame : Clear failed");
  _ASSERT_(pRenderer->BeginScene(),"DrawFrame : BeginScene failed");

  pRenderer->DrawTeapot();

  _ASSERT_(pRenderer->EndScene(),  "DrawFrame : EndScene failed");
  _ASSERT_(pRenderer->Swap(),      "DrawFrame : Swap failed");
  }

//

bool CMultiDxView::CreateRenderer()
  {
  Mythos::IRenderer* pRenderer = theApp.m_pRenderer;
  if(!pRenderer) return false;

  CFrameWnd* pFrame = GetParentFrame();                     // CChildFrame
  if(!pFrame) return false;
  RECT Rect;
  pFrame->GetClientRect(&Rect);
  Rect.right  /= 2;
  Rect.bottom /= 2,

  m_u32Renderer = pRenderer->CreateSwapChain(m_hWnd,Mythos::IRenderer::_WINDOWED_ | Mythos::IRenderer::_ZBUFFER_,Rect.right,Rect.bottom);
  if(!m_u32Renderer) return false;

  if(!pRenderer->SelectSwapChain(m_u32Renderer)) return false;

  Mythos::CVect3D v3Red   (.5f,0.f,0.f);
  Mythos::CVect3D v3Green (0.f,.5f,0.f);
  Mythos::CVect3D v3Blue  (0.f,0.f,.5f);
  Mythos::CVect3D v3Yellow(.5f,.5f,0.f);
  Mythos::CVect3D v3Color;

  static DWORD dwColor = 0;
  switch(dwColor)
    {
    case 0  : v3Color = v3Red;    break;
    case 1  : v3Color = v3Green;  break;
    case 2  : v3Color = v3Blue;   break;
    case 3  : v3Color = v3Yellow; break;
    default : v3Color.Set(0.f);   break;
    }
  dwColor++;
  dwColor &= 3;

  pRenderer->SetClearColor(v3Color);
  return true;
  }

//

void CMultiDxView::OnSize(UINT nType, int cx, int cy) 
  {
	CView::OnSize(nType, cx, cy);

  Mythos::IRenderer* pRenderer = theApp.m_pRenderer;
  if(pRenderer && m_u32Renderer)
    {
    pRenderer->ResizeSwapChain(m_u32Renderer,cx,cy);
    }
  }
