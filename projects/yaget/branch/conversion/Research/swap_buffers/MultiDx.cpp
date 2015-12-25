// MultiDx.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MultiDx.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "MultiDxDoc.h"
#include "MultiDxView.h"

#include "Rendering/dx9/RendererDX9.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiDxApp

BEGIN_MESSAGE_MAP(CMultiDxApp, CWinApp)
	//{{AFX_MSG_MAP(CMultiDxApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiDxApp construction

CMultiDxApp::CMultiDxApp()
{
  m_pRenderer   = NULL;
  m_boActivated = true;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMultiDxApp object

CMultiDxApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMultiDxApp initialization

BOOL CMultiDxApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_MULTIDTYPE,
		RUNTIME_CLASS(CMultiDxDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		NULL);//RUNTIME_CLASS(CMultiDxView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

  // create renderer
  m_pRenderer = new Mythos::CRendererDX9;
  if(!m_pRenderer) return FALSE;
  u32 u32Flags = Mythos::IRenderer::_WINDOWED_ | Mythos::IRenderer::_ZBUFFER_;
  if(!m_pRenderer->Create(m_pMainWnd->m_hWnd,u32Flags,16,16))
    {
    MessageBox(NULL,"renderer initialization failed",m_pRenderer->GetSignature().c_str(),MB_ICONEXCLAMATION|MB_OK);
    delete m_pRenderer;
    return FALSE;
    }

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);                 // maximize app
	pMainFrame->UpdateWindow();

	return TRUE;
}

//

int CMultiDxApp::ExitInstance() 
{
  delete m_pRenderer;
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMultiDxApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMultiDxApp message handlers

BOOL CMultiDxApp::OnIdle(LONG lCount) 
  {
	if(CWinApp::OnIdle(lCount)) return TRUE;
  if(!m_boActivated) return FALSE;

  // find active document - The MFC Answer Book, 2.18
  CWnd* pWnd = AfxGetMainWnd();
  if(pWnd)
    {
    CFrameWnd* pMainFrame   = static_cast<CFrameWnd*>(pWnd);
    CFrameWnd* pActiveFrame = pMainFrame->GetActiveFrame();

    if(pActiveFrame)
      {
      CDocument* pDoc = pActiveFrame->GetActiveDocument();
      if(pDoc)
        {
        pDoc->UpdateAllViews(NULL,_IDLE_);
        ((CMultiDxDoc*)pDoc)->UpdateAngleY();
        return TRUE;                                        // give me more processing time
        }
      }
    }

  return FALSE;
  }
