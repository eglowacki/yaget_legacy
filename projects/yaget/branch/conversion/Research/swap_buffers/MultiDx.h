// MultiDx.h : main header file for the MULTIDX application
//

#if !defined(AFX_MULTIDX_H__0DB5CB85_AC99_11D6_9CD9_444553540000__INCLUDED_)
#define AFX_MULTIDX_H__0DB5CB85_AC99_11D6_9CD9_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

namespace Mythos
  {
  class IRenderer;
  }

#define _IDLE_  WM_USER+1

/////////////////////////////////////////////////////////////////////////////
// CMultiDxApp:
// See MultiDx.cpp for the implementation of this class
//

class CMultiDxApp : public CWinApp
{
public:
	CMultiDxApp();

  Mythos::IRenderer*  m_pRenderer;
  bool                m_boActivated;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiDxApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMultiDxApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern CMultiDxApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIDX_H__0DB5CB85_AC99_11D6_9CD9_444553540000__INCLUDED_)
