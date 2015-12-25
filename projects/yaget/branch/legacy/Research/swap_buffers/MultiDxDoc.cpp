// MultiDxDoc.cpp : implementation of the CMultiDxDoc class
//

#include "stdafx.h"
#include "MultiDx.h"
#include "MultiDxDoc.h"

#include "maths/VectMat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiDxDoc

IMPLEMENT_DYNCREATE(CMultiDxDoc, CDocument)

BEGIN_MESSAGE_MAP(CMultiDxDoc, CDocument)
	//{{AFX_MSG_MAP(CMultiDxDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiDxDoc construction/destruction

CMultiDxDoc::CMultiDxDoc()
{
  m_fAngleY = 0.f;
}

CMultiDxDoc::~CMultiDxDoc()
{
}

BOOL CMultiDxDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMultiDxDoc serialization

void CMultiDxDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMultiDxDoc diagnostics

#ifdef _DEBUG
void CMultiDxDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMultiDxDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMultiDxDoc commands

void CMultiDxDoc::UpdateAngleY()
  {
  m_fAngleY += _DEG2RAD_(0.1f);

  if(m_fAngleY > 2.f*_PI_) m_fAngleY -= 2.f*_PI_;
  }
