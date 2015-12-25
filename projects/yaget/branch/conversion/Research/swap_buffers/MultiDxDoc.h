// MultiDxDoc.h : interface of the CMultiDxDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIDXDOC_H__0DB5CB8D_AC99_11D6_9CD9_444553540000__INCLUDED_)
#define AFX_MULTIDXDOC_H__0DB5CB8D_AC99_11D6_9CD9_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMultiDxDoc : public CDocument
{
protected: // create from serialization only
	CMultiDxDoc();
	DECLARE_DYNCREATE(CMultiDxDoc)

// Attributes
public:

  float   m_fAngleY;

  void    UpdateAngleY  (void);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiDxDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMultiDxDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMultiDxDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIDXDOC_H__0DB5CB8D_AC99_11D6_9CD9_444553540000__INCLUDED_)
