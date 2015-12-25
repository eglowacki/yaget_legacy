// MultiDxView.h : interface of the CMultiDxView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIDXVIEW_H__0DB5CB8F_AC99_11D6_9CD9_444553540000__INCLUDED_)
#define AFX_MULTIDXVIEW_H__0DB5CB8F_AC99_11D6_9CD9_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMultiDxDoc;

class CMultiDxView : public CView
{
protected: // create from serialization only
	CMultiDxView();
	DECLARE_DYNCREATE(CMultiDxView)

// Attributes
public:
	CMultiDxDoc* GetDocument();

// Operations
public:

  void    DrawFrame           (void);
  bool    CreateRenderer      (void);

  DWORD   m_u32Renderer;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiDxView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMultiDxView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMultiDxView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MultiDxView.cpp
inline CMultiDxDoc* CMultiDxView::GetDocument()
   { return (CMultiDxDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIDXVIEW_H__0DB5CB8F_AC99_11D6_9CD9_444553540000__INCLUDED_)
