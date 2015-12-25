/////////////////////////////////////////////////////////////////////////
// bind_window.cpp
//
//  Copyright 5/1/2009 Edgar Glowacki.
//
// NOTES:
//
//
// #include "bind_window.cpp"
//
/////////////////////////////////////////////////////////////////////////
//! \file
#include "App/AppWindow.h"
#include "MessageInterface.h"
#include "Logger/Log.h"
#include "Script/ScriptErrorHandler.h"

using namespace eg;

namespace
{
    const int kNumTries = 5;

    struct windowCallback
    {
        windowCallback(const luabind::object &callback)
        : mErrorCounter(kNumTries)
        , callback(callback)
        {}

        windowCallback()
        : mErrorCounter(kNumTries)
        {}

        void operator()(const Clock& clock)
        {
            if (!mErrorCounter)
            {
                return;
            }

            try
            {
                float deltaTime = static_cast<float>(clock.GetFrameDuration());
                luabind::call_function<void>(callback, deltaTime);
                mErrorCounter = kNumTries;
            }
            catch (luabind::error& e)
            {
                mErrorCounter--;
                lua_State *L = e.state();

                script::luaError(L, false);
                luabind::object error_msg(luabind::from_stack(L, -1));
                log_error << "Script runtime error, tries left(" << mErrorCounter << "):\n" << error_msg << ">";
                lua_pop(L, 1);
            }
        }

    private:
        int mErrorCounter;
        luabind::object callback;
    };


    class LuaWindow : public app::Window
    {
    public:
        LuaWindow(const std::string& name) : app::Window(name)
        {}

        luabind::object eventClose;
        luabind::object eventSize;

    private:
        virtual void onClose()
        {
            if (!eventClose)
            {
                return;
            }

            try
            {
                luabind::call_function<void>(eventClose);
            }
            catch (luabind::error& e)
            {
                lua_State* L = e.state();
                script::luaError(L, false);
                luabind::object error_msg(luabind::from_stack(L, -1));
                log_error << "Script runtime error :\n" << error_msg << ">";
                lua_pop(L, 1);
            }
        }

        virtual void onSize()
        {
            if (!eventSize)
            {
                return;
            }

            try
            {
                luabind::call_function<void>(eventSize);
            }
            catch (luabind::error& e)
            {
                lua_State* L = e.state();
                script::luaError(L, false);
                luabind::object error_msg(luabind::from_stack(L, -1));
                log_error << "Script runtime error :\n" << error_msg << ">";
                lua_pop(L, 1);
            }
        }
    };

    void window_run(const luabind::object &callback)
    {
        windowCallback winCallback(callback);
        app::Window::Run(winCallback);
    }

    void *l_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
    {
        ud;osize;
        if (nsize == 0)
        {
            if (ptr)
            {
                log_trace("main") << "free(" << (size_t)ptr << ")";
                free(ptr);
            }
            return NULL;
        }
        else
        {
            void *new_ptr = realloc(ptr, nsize);
            log_trace("main") << (size_t)new_ptr << " = new(" << nsize << ")";
            return new_ptr;
        }
    }

} // namespace

        //luabind::class_<Dispatcher::Channel_t>("signal_void"),
        //lb::class_<Dispatcher>("Dispatcher")
        //    .def("connect", &VirtualFileSystem::triggerFile)

namespace eg { namespace script {


void init_window(lua_State *L)
{
    using namespace luabind;

    //lua_setallocf(L, l_alloc, 0);

    module(L, "yaget")
    [
        def("run", &window_run),
        class_<LuaWindow>("Window")
            .def(constructor<const std::string&>())
            .def("show", &LuaWindow::Show)
            .def("close", &LuaWindow::Close)
            .property("handle", &LuaWindow::Handle)
            .def_readwrite("eventClose", &LuaWindow::eventClose)
            .def_readwrite("eventSize", &LuaWindow::eventSize)
    ];
}


}} // namespace script // namespace eg


