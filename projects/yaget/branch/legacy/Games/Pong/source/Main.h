/////////////////////////////////////////////////////////////////////
// Main.h
//
//  Copyright 12/20/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES: Main Pong game object
//
//
//  #include "Main.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef PONG_GAME_MAIN_H
#define PONG_GAME_MAIN_H
#pragma once

#include "App/MainFrame.h"
#include "File/AssetLoadSystem.h"
#include "IRenderer.h"
#include "Math/Vector.h"
#include "Math/InterpolationType.h"
#include "Message/Dispatcher.h"
#include "Plugin/PluginManager.h"
#include "Timer/ClockManager.h"
#include "Config/Console.h"


namespace pong
{
    class SplashCamera;

    struct Graph
    {
        Graph() : begIndex(0), endIndex(0), bWrapSamples(false), screenSize(eg::v2::ZERO()) {}

        class const_iterator : public std::iterator<std::forward_iterator_tag, uint32_t>
        {
        public:
            //! initialize this iterator
            //! \param samples collection of samples
            //! \param begC start of current sample range
            //! \param endC end of current sample range
            //! \param bEnd initialize as end() iterator or begin()
            const_iterator(const std::vector<uint32_t>& samples, uint32_t begC, uint32_t endC, bool bEnd);
            uint32_t operator *() const;
            const_iterator& operator ++();
            const_iterator operator ++(int);
            friend bool operator ==(const const_iterator& lhs, const const_iterator& rhs);
            friend bool operator !=(const const_iterator& lhs, const const_iterator& rhs);

        private:
            const std::vector<uint32_t>& mSamples;
            std::vector<uint32_t>::const_iterator mCurrentIt;
            uint32_t begCursor;
            uint32_t endCursor;
        };

        const_iterator begin() const;
        const_iterator end() const;
        bool empty() const;

        //! add sample to next slot pointing by cursor index
        //! \param s sample value which should be in 0 to 1 range
        //! \param deltaTime delta in seconds since last time to call
        void addSample(float s);
        void resize(const eg::Vector2& size);
        //! Render samples using line rendering
        void render(eg::IRenderer& renderer, const eg::Color4_t& lineColor);

        // values, one for each pixel in width
        std::vector<uint32_t> values;
        // begging of this sample range
        uint32_t begIndex;
        // current index position where next add sample will be
        uint32_t endIndex;
        // do already wrapped, or use start of a values for reading
        bool bWrapSamples;
        // size of a screen in pixels
        eg::Vector2 screenSize;
    };

    /*!
    This is our main Pong application object
    */
    class PongApp : public eg::MainApp
    {
    public:
        PongApp();
        virtual ~PongApp();

        // from MainApp
        //virtual wxFrame *createMainFrame();
        virtual bool OnInit();
        virtual int OnExit();

        //! accessors to our subsystem
        //! making getters here, since this is in flux
        //! and it might change
        eg::Dispatcher& Dispacher() {return mDispacher;}
        eg::VirtualFileSystem& VitualFileSystem() {return mVitualFileSystem;}
        eg::AssetLoadSystem& AssetLoadSystem() {return mAssetLoadSystem;}
        eg::InputManager& InputSystem() {return mInputSystem;}
        eg::PluginManager& PluginManager() {return mPluginManager;}
        eg::config::Konsole& Konsole() {return mKonsole;}
        eg::ClockManager& Clock() {return mClock;}
        eg::IdGameCache& IdCache() {return mIdCache;}

    private:
        // this are basic subsystems we want to use from base yaget framework
        // and must be in this specific order. 
        // since mDispacher is used by other object for construction
        eg::Dispatcher mDispacher;
        eg::VirtualFileSystem mVitualFileSystem;
        eg::AssetLoadSystem mAssetLoadSystem;
        eg::InputManager mInputSystem;
        eg::PluginManager mPluginManager;
        eg::config::Konsole mKonsole;
        eg::ClockManager mClock;
        eg::IdGameCache mIdCache;
        eg::IRenderer::ViewToken mMainViewToken;
    };

    class GameWindow : public eg::MainFrame
    {
    public:
        GameWindow(eg::Dispatcher& dispatcher, eg::InputManager& im, eg::ClockManager& clock, const std::string& title, const wxPoint& pos, const wxSize& size);
        virtual ~GameWindow();

    private:
        virtual void Tick(float deltaTime);
        virtual void Size(const wxSize& size);
        virtual void onSizing(const wxSize& size);
        void onShutdown(eg::Message& msg);
        void setTitle(const wxSize& size);

        boost::shared_ptr<SplashCamera> mSplashCamera;

        Graph mFrameGraph;
    };

} // namespace pong

#endif // PONG_GAME_MAIN_H

