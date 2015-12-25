///////////////////////////////////////////////////////////////////////
// InputManager.h
//
//  Copyright 11/13/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Input system which abstracts platform specific code.
//      It does relay on input being fed to it by user.
//
//
//  #include "Input/InputManager.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef INPUT_INPUT_SYSTEM_H
#define INPUT_INPUT_SYSTEM_H
#pragma once

#include "Base.h"
#include "Math/Vector.h"
//#include "Registrate.h"
#include "Exception/Exception.h"
#include <BitArray.h>
#include <boost/bind.hpp>
#pragma warning(push)
#pragma warning (disable : 4244)  // '' : conversion from '' to '', possible loss of data
    #include <boost/function.hpp>
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
    #include <boost/signal.hpp>
#pragma warning(pop)

namespace eg
{
    class AssetLoadSystem;
    class Dispatcher;
    namespace { class ISData;}

    namespace mtype
    {
        //! Sned for every generated input event
        //! boost::any is a 'const InputManager::Input *'
        //! const InputManager::Input *input = msg.GetValue<const InputManager::Input *>();
        static const guid_t kInput = 0x6edf26c0;
    } // namespace mtype


    namespace InputTypes
    {
        static const uint32_t kButtonUp         = 0x0001;
        static const uint32_t kButtonDown       = 0x0002;
        static const uint32_t kButtonShift      = 0x0004;
        static const uint32_t kButtonCtrl       = 0x0008;
        static const uint32_t kButtonAlt        = 0x0010;
        static const uint32_t kButtonCaps       = 0x0020;
        static const uint32_t kMouseMove        = 0x0040;
        static const uint32_t kButtonNumLock    = 0x0080;
        static const uint32_t kInputSeqOred     = 0x0100;   ///< user can specify several input types


        /*!
        Some defines for keys.
        */
        static const uint32_t kBack             = 8;
        static const uint32_t kTab              = 9;
        static const uint32_t kReturn           = 13;

        static const uint32_t kArrowUp          = 255;
        static const uint32_t kArrowDown        = 254;
        static const uint32_t kArrowLeft        = 253;
        static const uint32_t kArrowRight       = 252;
        static const uint32_t kPageUp           = 251;
        static const uint32_t kPageDown         = 250;
        static const uint32_t kEnd              = 249;
        static const uint32_t kHome             = 248;
        static const uint32_t kInsert           = 247;
        static const uint32_t kDelete           = 246;

        static const uint32_t kMouseLeft        = 1;
        static const uint32_t kMouseRight       = 2;
        static const uint32_t kMouseMiddle      = 3;

    } // namespace InputTypes


    //! Predefined input context. This can be extended in user app to add new context
    namespace InputContext
    {
        //! We are in a game
        static const char *kGame = "INPUT_CONTEXT_GAME";
        //! Menu screen is up and active
        static const char *kMenu = "INPUT_CONTEXT_MENU";
    } // namespace InputTypes

    /*!
    This is used to be able just add Input Action Name and get called when
    that action is valid
    \param actionName - action name which got triggered
    \param timeStamp - time of this input
    \param mouseX - position of mouse X of this input
    \param mouseY - position of mouse Y of this input

    \code
    // first way - using class method to handle callback
    class App
    {
    public:
        void MyCallback(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY);
    };

    // and later
    App myApp;
    InputActionCallback_t myClassInputCallback = boost::bind(&App::MyCallback, &myApp, _1, _2, _3, _4);


    // ----------------------------------------
    // second way - using function object
    struct MyFunctionInputCallback
    {
        void operator()(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY) const
        {
            // ... process input here
        };
    };

    InputActionCallback_t myInputCallback = MyFunctionInputCallback();


    // ----------------------------------------
    // third way - using statndard function
    void MyCallback(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY);

    InputActionCallback_t myInputCallback = MyCallback;
    \endcode
    */
    typedef boost::function<void (const std::string& /*actionName*/, uint32_t /*timeStamp*/, int32_t /*mouseX*/, int32_t /*mouseY*/)> InputActionCallback_t;


    /*!
    \brief Main input system for all Game Objects to use.

    There is 2 ways to inquire about what input happened this frame/tick.

     - Use raw Input from NextInput() method.
     - Use IsAction(const char *actionName) const to check if some
       input combination was generated which was registered before
       using RegisterAction(const char *actionName, const Input *defaultInput).


    To get all the raw Input for the current frame use the following code:
    \code
        void GameCharacter::Tick()
        {
            InputManager::Get().LockQue();
            InputManager::Input *input = 0;
            while (input = InputManager::Get().NextInput())
            {
                // do something with the input like make character jump
                // when user preeses up arrow key
                if (IsKey(input, kArrowUp))
                {
                    JumpUp();
                }
            }

            InputManager::Get().UnlockQue();
        }
    \endcode

    If this is called several times in the same frame, it will return
    same results. The queue is changed only once per frame.

    The second way using is IsAction(const char *ActionName).

    Somewhere in the client code call once
    \code gInputManager.RegisterAction("Jump", kArrowUpI);\endcode
    then this is all you need in your Tick method
    \code
        void GameCharacter::Tick()
        {
            // do something with the input like make character jump
            // when user preeses up arrow key
            if (InputManager::Get().IsAction("Jump"))
            {
                JumpUp();
            }
        }
    \endcode
    */
    class InputManager
    {
        //@{
        //! no copy semantics
        InputManager(const InputManager&);
        InputManager& operator=(const InputManager&);
        //@}

    public:
        //! Specific input type classes.
        enum InputType {tKey = 1, tMouse, tJoyPad, tJoystick, tCustum};
        class Input;

        //! When input comes in, ImputManager calls attached filters
        //! to see if it can put it into a queue
        typedef boost::function<bool (const Input& /*input*/)> Filter;

        //! Base type class from which all type of input classes (key, mouse, joystick, etc) are derived from.
        class Input
        {
            //@{
            //! no copy semantics
            Input(const Input&);
            Input& operator=(const Input&);
            //@}

        public:
            const uint32_t mType;           ///< mType, one of the above enum or some unique number (ala RTTI)
            const uint32_t mFlags;          ///< Different flags, different meaning based on what kind of input.
            const uint32_t mTimeStamp;      ///< Time of that input.
            Input *mNext;                   ///< We can chain Inputs for replays and 'Action' matching.
            long mUser;                     ///< any user data atached to this input

            /*!
            \param type is unique type, which derived class will set it. The base implementation
            uses the enum above
            */
            Input(const uint32_t type, const uint32_t timeStamp, const uint32_t flags);

            virtual ~Input();

            virtual bool operator==(const Input& source) const
            {
                return mType == source.mType && mFlags == source.mFlags;
            }

            /*!
            \brief Returns TRUE if Target type matches and flags are equals.

            Will return TRUE if mType and mFlags are the same.
            Parameter And is used to see how we check this.mFlags against Source.mFlags.
            If TRUE then (Source.mFlags & this.mFlags) is used, otherwise
            (Source.mFlags == this.mFlags) is used.
            */
            virtual bool Is(const Input *target, bool and = true) const;

            //! Return string representation of this Input (Used in debugging).
            //virtual const char *Print() const;

            //! This will return TRUE if all the bits from Target are in Source.
            static bool CompareAllBits(const uint32_t source, const uint32_t target);
        };


        //! Key class.
        class Key : public Input
        {
        public:
            const unsigned char mValue;     ///<ASCII number if this key.

            Key(const uint32_t timeStamp, const uint32_t flags, const unsigned char key_value);
            Key(const uint32_t flags, const unsigned char key_value);

            /*!
            This ctor takes character strings and constructs chain of inputs to create valid Action.
            */
            Key(const char *String);

            virtual bool operator==(const Key& source) const
            {
                return mValue == source.mValue && Input::operator ==(source);
            }

            //! Is this Key printable character.
            inline bool IsChar() const;

            //! is this v equals this Key.
            inline bool Is(unsigned char v) const;

            // From Input
            virtual bool Is(const Input *tTarget, bool and = true) const;
            //virtual const char *Print() const;
        };

        //! mouse input class
        class Mouse : public Input
        {
        public:
            const Vector2 mPos;     ///<Absolute position of the mouse.
            BitArray mButtons;      ///<Bit pattern of all the buttons which the flags are true.

            Mouse(const uint32_t timeStamp, const uint32_t flags, const Vector2& pos = Vector2(-1.0f, -1.0f));
            Mouse(const uint32_t flags, const Vector2& pos = Vector2(-1.0f, -1.0f));

            virtual bool operator==(const Mouse& source) const
            {
                return mPos == source.mPos && mButtons == source.mButtons && Input::operator ==(source);
            }

            // from Input
            virtual bool Is(const Input *target, bool and = true) const;
        };

        //! Joystick pad input class.
        class JoyPad : public Input
        {
        public:
            // \note add support for strength and more then one pad
            float mDir;             ///<In degrees from 0 to 360.
            uint32_t mButtons;      ///<Bit pattern of all the buttons which the flags are true.

            JoyPad(const uint32_t timeStamp, const uint32_t flags);

            virtual bool operator==(const JoyPad& source) const
            {
                return mDir == source.mDir && mButtons == source.mButtons && Input::operator ==(source);
            }
        };

        InputManager(Dispatcher& disp, AssetLoadSystem& als, const std::string& fileKeyBinds);
        virtual ~InputManager();

        void SetFilter(Filter filter);
        /*!
        \brief This is called by the Framework and OS dependent input.

        This is called by the Framework when it has some input generated by the OS. The
        Framework provides input for keyboard and mouse
        */
        void ProcessInput(Input *input);
        //! Set current global mouse position
        void SetCurrentMousePos(const Vector2& pos);
        const Vector2& GetCurrentMousePos() const;

        //! Lock queue to start extracting input.
        void LockQue();

        //! Unlock queue after user is done with it.
        void UnlockQue();

        //! Call this between Lock and Unlock until returns null.
        const Input *NextInput();

        /*!
        Register some Action with the Input System and trigger that action to be TRUE
        when defaultInput matches input by a user.
        \param actionName unique name of this action to refer to our input
        \param defaultInput which input will trigger this action
        \param contextName in what context this input is valid
        \param inputActionCallback if this is valid, call this callback when input is valid.
        \param compareAsOr This only applies if there is a chain of Inputs. If true then match all in chain, otherwise match any input in chain.

        \note: I provide 2 overload methods, since I can not make default parameter
               for inputActionCallback because VC++ 7.1 gives me internal compiler error
        */
        void RegisterAction(const char *actionName, const Input *defaultInput, const char *contextName, InputActionCallback_t inputActionCallback, bool compareAsOr);
        void RegisterAction(const char *actionName, const Input *defaultInput, const char *contextName, bool compareAsOr);

        /*!
        Register some Action with the Input System and trigger that action to be TRUE
        when DefaultInput matches input by a user. This method is the wrapper for the
        RegisterAction(const char *ActionName, const CInput *DefaultInput), but it takes
        any ASCII value and converts to Key Down type.
        */
        void RegisterAction(const char *actionName, const unsigned char value, const char *contextName, InputActionCallback_t inputActionCallback);
        void RegisterAction(const char *actionName, const unsigned char value, const char *contextName);

        //! This structure is returned from ListenForAction and
        //! will disconnect and remove any connection on destruction
        struct ActionListener
        {
            virtual ~ActionListener() {}
        };

        typedef boost::shared_ptr<ActionListener> ActionListener_t;
        //! Attach this listener for specific action. This will load default input for this action, if there is none created yet
        ActionListener_t ListenForAction(const std::string& actionName, InputActionCallback_t inputActionCallback);

        //! Trigger any callbacks attached to this action
        //! \note this method returns right a way and trigger action will
        //! not happen to the end of frame.
        void TriggerAction(const char *actionName);

        //! Will remove action Name from this UserInput registered Actions.
        void UnregisterAction(const char *actionName);

        //! Will return TRUE if User Input matched this ActionName in the last frame.
        bool IsAction(const char *actionName) const;

        //! Pop top context off and return it
        std::string PopContext();

        //! Push new context onto top of a stack and make it current
        void PushContext(const std::string& newContextName);

        //! Return time of that Action. For meaningful results it should be called after IsAction(...).
        uint32_t ActionTime(const char *actionName) const;

        /*!
        This will return mouse position at the time when this Action become valid.
        For meaningful results it should be called after IsAction(...).
        */
        Vector2 ActionPos(const char *actionName) const;

        /*!
        This will map value key into InputManager type. This is needed
        since the native input values are different between platforms and OS
        and we provide our own enums for key input. This method
        uses Translation file. It makes this class (InputManager independent of
        external input events.

        \param value value to translate
        \return translated value to InputManager domain
        */
        int MapKey(int value) const;

        //! Should be called by framework once per frame to process all inputs and send
        //! notifications of actions if any
        //! \param deltaTime how many seonconds since last call
        void Tick(float deltaTime);

        //! Load key bindings from file
        void LoadBindings(const std::string& name);

    private:
        // private data for this instance of a class.
        ISData *mData;
        Vector2 mMousePosition;
    };

    /*!
    To use this as a singleton
    \code
    #include "Registrat.h"
    InputManager *im = registrate::p_cast<InputManager>(wxT("InputManager"));
    // or ...
    InputManager& im = registrate::ref_cast<InputManager>(wxT("InputManager"));
    // or ...
    InputManager& im = REGISTRATE(InputManager);   // macro alert
    \endcode
    */

    /*!
    This helper class will make sure that input is unregistered in dtor.
    Use this class in your dlls', since any memory allocated in dll, must be
    deleted before that dll is unloaded.

    \code
    mActionViewToggle("My Action Name",
                      new InputManager::Key(InputTypes::kButtonDown|InputTypes::kButtonCtrl, '~'),
                      0,
                      boost::bind(&EditorFrame::OnInput, this, _1, _2, _3, _4))
    \endcode
    */
    class InputAction
    {
    public:
        InputAction();
        InputAction(const char *actionName, const InputManager::Input *pInput, const char *contextName, bool compareAsOr);
        InputAction(const char *actionName, const InputManager::Input *pInput, const char *contextName, InputActionCallback_t inputActionCallback, bool compareAsOr);
        ~InputAction();

        void RegisterAction(const char *actionName, const InputManager::Input *pInput, const char *contextName, bool compareAsOr);
        void RegisterAction(const char *actionName, const InputManager::Input *pInput, const char *contextName, InputActionCallback_t inputActionCallback, bool compareAsOr);
        bool IsAction() const;

        //! Called this to disconnect any kind of input, and IsAction() will always return false
        void Clear();

    private:
        std::string mActionName;
    };

    //! This keeps track of action and will return true until action is no longer true
    class InputSwitch
    {
    public:
        InputSwitch(const char *actionNameOn, const char *actionNameOff);

        bool isOn() const {return mActionOn;}
        //! Return delta mouse since the last call.
        //! This is only valid if acion is on
        Vector2 getMouseDelta() const;
        //! Return current mouse position, this is always valid
        Vector2 getMousePos() const;

    private:
        void onRegisteredActionOn(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY);
        void onRegisteredActionOff(const std::string& actionName, uint32_t timeStamp, int32_t mouseX, int32_t mouseY);

        InputManager::ActionListener_t mConnectionOn;
        InputManager::ActionListener_t mConnectionOff;
        bool mActionOn;
        mutable Vector2 mLastMousePosition;
    };

} // namespace eg


#define tr_input "input"

#define INPUT_MANAGER_INCLUDE_IMPLEMENTATION
#include "InputManagerImpl.h"
#undef INPUT_MANAGER_INCLUDE_IMPLEMENTATION





#endif // INPUT_INPUT_SYSTEM_H
