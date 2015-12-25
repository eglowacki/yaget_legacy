// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "MultiDx.h"

#include "ChildFrm.h"
#include "MultiDxView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	nCmdShow = SW_SHOWMAXIMIZED;                              // maximize child frame
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

//

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  RECT Rect;
  GetParent()->GetClientRect(&Rect);                        // so we have the correct size even if the frame is not maximized yet
  SIZE csSize;
  csSize.cx = Rect.right /2;
	csSize.cy = Rect.bottom/2;

  // create splitter & views

  m_Splitter.CreateStatic(this,2,2);

  m_Splitter.CreateView(0,0,RUNTIME_CLASS(CMultiDxView),csSize,pContext);
  m_Splitter.CreateView(0,1,RUNTIME_CLASS(CMultiDxView),csSize,pContext);
  m_Splitter.CreateView(1,0,RUNTIME_CLASS(CMultiDxView),csSize,pContext);
  m_Splitter.CreateView(1,1,RUNTIME_CLASS(CMultiDxView),csSize,pContext);

  // resize

  m_Splitter.SetColumnInfo(0,csSize.cx,0);
  m_Splitter.SetColumnInfo(1,csSize.cx,0);
  m_Splitter.SetRowInfo   (0,csSize.cy,0);
  m_Splitter.SetRowInfo   (1,csSize.cy,0);

  m_Splitter.RecalcLayout();
  return TRUE;
}
