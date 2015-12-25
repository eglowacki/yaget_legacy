// Pong game main entry point
#include "Precompiled.h"
#include "Main.h"
#include "Registrate.h"
#include "Config/InitSystem.h"
#include "IComponentScript.h"
#include "Profiler/prof.h"
#include "SplashCamera.h"

using namespace eg;

namespace
{
    //! make coordinates from bottom (0) to top ()-1)
    uint32_t pixelFix(uint32_t currValue, uint32_t height)
    {
        return height - currValue;
    }

    const uint32_t kLegendArea = 20;

} // namespace


namespace pong
{

IMPLEMENT_APP(PongApp)


PongApp::PongApp() 
: eg::MainApp()
, mVitualFileSystem(mDispacher)
, mAssetLoadSystem(mDispacher, mVitualFileSystem)
, mInputSystem(mDispacher)
, mPluginManager(mDispacher)
, mIdCache(mKonsole)
{
    // \note Finish adding support to allow module filtering for boost::logging
    /*
    std::vector<std::string> logList;
    if (config::EnumerateGroup("Logs", logList))
    {
        for (std::vector<std::string>::const_iterator it = logList.begin(); it != logList.end(); ++it)
        {
            wxLog::AddTraceMask(*it);
        }
    }
    */
}


PongApp::~PongApp()
{

}


/*
wxFrame *PongApp::createMainFrame()
{
    std::string appName = registrate::GetAppName();
    config::PanelUI panelConfig(appName.c_str(), 0, mVitualFileSystem);
    return new pong::GameWindow(mDispacher, mPluginManager, mInputSystem, mClock, appName, panelConfig.Pos, panelConfig.Size);
}
*/


bool PongApp::OnInit()
{
    if (eg::MainApp::OnInit())
    {
        registrate::RegisterObject(&mDispacher, "Dispatcher");
        registrate::RegisterObject(&mKonsole, "Konsole");
        registrate::RegisterObject(&mVitualFileSystem, "VirtualFileSystem");
        registrate::RegisterObject(&mAssetLoadSystem, "AssetLoadSystem");
        registrate::RegisterObject(&mInputSystem, "InputManager");
        registrate::RegisterObject(&mIdCache, "IdGameCache");

        mPluginManager.Load();

        std::string appName = registrate::GetAppName();
        config::PanelUI panelConfig(appName.c_str(), 0, mVitualFileSystem);
        pong::GameWindow *pMainWindow = new pong::GameWindow(mDispacher, mInputSystem, mClock, appName, panelConfig.Pos, panelConfig.Size);

        if (IRenderer *pRenderer = mPluginManager.p_cast<IRenderer>("Renderer"))
        {
            mMainViewToken = pRenderer->CreateViewToken(0);
            pMainWindow->Show(true);

            return true;
        }
    }

    return false;
}


int PongApp::OnExit()
{
    return eg::MainApp::OnExit();
}


GameWindow::GameWindow(eg::Dispatcher& dispatcher, eg::InputManager& im, ClockManager& clockSource, const std::string& title, const wxPoint& pos, const wxSize& size)
: MainFrame(dispatcher, im, clockSource, title, pos, size)
{
    //mSplashCamera.reset(new SplashCamera(dispatcher));

    std::string packageName = config::ReadString("AutoStart/PackageName");
    if (!packageName.empty())
    {
        component::InstanceId scriptId = component::make(idspace::get_burnable(), ComponentType::kScript);
        component::newc("AutoStart", scriptId);
        property::hProp prop(scriptId);
        prop[script::kPropPackageName] = packageName;
        wxLogTrace("pong", "AutoStart script '%s' loaded.", packageName);
    }
}


GameWindow::~GameWindow()
{
    std::string appName = registrate::GetAppName();
    config::PanelUI panelConfig(appName.c_str(), this, wxGetApp().VitualFileSystem());
}


void GameWindow::Tick(float deltaTime)
{
    /*
    //mFrameGraph.addSample(GetRandom());
    IRenderer& renderer = registrate::ref_cast<IRenderer>("Renderer");
    if (renderer.UseViewToken(mMainViewToken))
    {
        wxSize clientSize = GetClientSize();
        wxRect viewport(0, 0, clientSize.x, clientSize.y);
        ComponentListCallback<IComponentView> viewList;
        viewList.Sort();

        while (IComponentView *pView = viewList.Next())
        {
           // main window only render camera with bit 1 set
           if (pView->GetRenderId() & 0x1)
           {
              Prof(ViewRender);
              // and render the current set of geometry
              pView->Render(renderer, 0, deltaTime, viewport);
           }
        }

        //mFrameGraph.render(renderer, Color4_t(1, 1, 1, 1));
        renderer.Flip();
    }
    */
}


void GameWindow::Size(const wxSize& size)
{
    mFrameGraph.resize(Vector2(size.x, size.y));
    IRenderer& renderer = registrate::ref_cast<IRenderer>("Renderer");
    renderer.Reset();

    wxSize clientsize = GetClientSize();
    setTitle(clientsize);
}


void GameWindow::onSizing(const wxSize& size)
{
    setTitle(size);
}


void GameWindow::setTitle(const wxSize& size)
{
    std::string appName = registrate::GetAppName();

    appName += " [";
#if _DEBUG
    appName += "Debug, ";
#else // _DEBUG
    appName += "Release, ";
#endif // _DEBUG

    appName += "Size: " + boost::lexical_cast<std::string>(size.x) + "x" + boost::lexical_cast<std::string>(size.y) + "";
    appName += "]";
    SetTitle(appName.c_str());
}


void Graph::addSample(float s)
{
    // first calculate what s is in pixel value
    uint32_t pixelValue = s * screenSize.y-1;
    pixelValue = pixelFix(pixelValue, screenSize.y);
    values[endIndex++] = pixelValue;
    if (endIndex >= screenSize.x)
    {
        // we need to wrap around
        endIndex = 0;
        bWrapSamples = true;
    }

    if (bWrapSamples)
    {
        begIndex++;

        if (begIndex >= screenSize.x)
        {
            begIndex = 0;
        }
    }
}


void Graph::resize(const Vector2& size)
{
    screenSize = size;
    screenSize.y -= kLegendArea;
    values.resize(screenSize.x);
    endIndex = 0;
    begIndex = 0;
    bWrapSamples = false;
}


void Graph::render(IRenderer& renderer, const Color4_t& lineColor)
{
    // render bottom horizontal line first
    uint32_t legendLine = screenSize.y;//pixelFix(5, screenSize.y+kLegendArea);
    Vector3 begPos(0, legendLine, 0);
    Vector3 endPos(screenSize.x-1, legendLine, 0);

    Color4_t legendLineColor(0.1f, 0.8f, 0.1f, 1.0f);
    renderer.DrawLine(begPos, endPos, legendLineColor, legendLineColor, 0);

    Graph::const_iterator it = begin();
    Graph::const_iterator it_end = end();

    begPos.set(0, 0, 0);
    endPos.set(1, 0, 0);
    for (; it != it_end;)
    {
        begPos.y = *it;
        ++it;
        endPos.y = it != it_end ? *it : begPos.y;

        begPos.x += 1;
        endPos.x += 1;

        renderer.DrawLine(begPos, endPos, lineColor, lineColor, 0);
    }
}


Graph::const_iterator Graph::begin() const
{
    return const_iterator(values, begIndex, endIndex, false);
}


Graph::const_iterator Graph::end() const
{
    return const_iterator(values, begIndex, endIndex, true);
}

bool Graph::empty() const
{
    return begIndex == endIndex;
}


Graph::const_iterator::const_iterator(const std::vector<uint32_t>& samples, uint32_t begC, uint32_t endC, bool bEnd)
: mSamples(samples)
, mCurrentIt(bEnd ? samples.begin() + endC : samples.begin() + begC)
, begCursor(begC)
, endCursor(endC)
{
}


uint32_t Graph::const_iterator::operator *() const
{
    uint32_t it(*mCurrentIt);
    return it;
}


Graph::const_iterator& Graph::const_iterator::operator++()
{
    ++mCurrentIt;
    if (mCurrentIt == mSamples.end() && begCursor > 0)
    {
        mCurrentIt = mSamples.begin();

    }
    return *this;
}


Graph::const_iterator Graph::const_iterator::operator++(int)
{
    const_iterator tmp(*this);
    ++(*this);
    return tmp;
}


bool operator ==(const Graph::const_iterator& lhs, const Graph::const_iterator& rhs)
{
    return lhs.mCurrentIt == rhs.mCurrentIt;
}


bool operator !=(const Graph::const_iterator& lhs, const Graph::const_iterator& rhs)
{
    return !(lhs == rhs);
}


} // namespace pong
