///////////////////////////////////////////////////////////////////////
// InputManagerImpl.h
//
//  Copyright 11/20/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "InputManagerImpl.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef INPUT_MANAGER_IMPL_H
#define INPUT_MANAGER_IMPL_H

#ifndef INPUT_MANAGER_INCLUDE_IMPLEMENTATION
    #error "Do not include this file explicitly."
#endif // INPUT_MANAGER_INCLUDE_IMPLEMENTATION


namespace eg
{

    //---------------------------------------------------------------------------------------------------------
    //! Inline implementation
    // input
    inline InputManager::Input::Input(const uint32_t type, const uint32_t timeStamp, const uint32_t flags)
    : mType(type)
    , mTimeStamp(timeStamp)
    , mFlags(flags)
    , mNext(0)
    , mUser(0)
    {
    }

    inline InputManager::Input::~Input()
    {
        if ( mNext )
            delete mNext;
    }

    //---------------------------------------------------------------------------------------------------------
    inline bool InputManager::Input::Is(const InputManager::Input *target, bool and) const
    {
        //And = false;
        // if type is the same and this flags can be and'ed
        // with Source flags then Input is the same.
        if (mType == target->mType)
        {
            // the only time we can compare the flags if Target
            // has any flags set.
            if (target->mFlags)
            {
                if (and)
                    return CompareAllBits(mFlags, target->mFlags);
                else
                    return mFlags == target->mFlags;
            }
            else
                // since our Target does not have any flags,
                // we do not check it and just return TRUE.
                return true;
        }

        return false;
    }

    //---------------------------------------------------------------------------------------------------------
    inline bool InputManager::Input::CompareAllBits(const uint32_t source, const uint32_t target)
    {
        bool result = true;

        // step trough all the bits by extracting the least significant one after shifting,
        // then compare that bit with Source.
        for (int i = 0; i < 32; i++)
        {
            uint32_t CurrBit = target & (1 << i);
            if (CurrBit)
            {
                if (!(source & CurrBit))
                {
                    result = false;
                    break;
                }
            }
        }

        return result;
    }


    //-----------------------------------------------------------------------------------------------------
    // key
    inline InputManager::Key::Key(const uint32_t timeStamp, const uint32_t flags, const unsigned char key_value) : InputManager::Input(tKey, timeStamp, flags),
        mValue(key_value)
    {
    }

    //---------------------------------------------------------------------------------------------------------
    inline InputManager::Key::Key(const uint32_t flags, const unsigned char key_value) : InputManager::Input(tKey, 0, flags),
        mValue(key_value)
    {
    }

    //---------------------------------------------------------------------------------------------------------
    inline InputManager::Key::Key(const char *String) : Input(tKey, 0, InputTypes::kButtonDown),
        mValue(*String)
    {
        assert(String);
        mNext = new Key(0, InputTypes::kButtonUp, *String);
        String++;
        if (*String)
        {
            mNext->mNext = new Key(String);
        }
    }

    //---------------------------------------------------------------------------------------------------------
    inline bool InputManager::Key::IsChar() const
    {
        if (mValue >= 32 && mValue <= 126)
            return true;

        return false;
    }

    //---------------------------------------------------------------------------------------------------------
    inline bool InputManager::Key::Is(unsigned char v) const
    {
        return (mValue && mValue == v);
    }

    //---------------------------------------------------------------------------------------------------------
    inline bool InputManager::Key::Is(const Input *target, bool and) const
    {
        if (InputManager::Input::Is(target, and) && mValue == static_cast<const Key *>(target)->mValue)
        {
            bool This_shift = false, This_ctrl = false;
            if (mFlags & eg::InputTypes::kButtonShift)
            {
                This_shift = true;
            }
            if (mFlags & InputTypes::kButtonCtrl)
            {
                This_ctrl = true;
            }

            bool Target_shift = false, Target_ctrl = false;
            if (target->mFlags & InputTypes::kButtonShift)
            {
                Target_shift = true;
            }
            if (target->mFlags & InputTypes::kButtonCtrl)
            {
                Target_ctrl = true;
            }

            if (This_shift == Target_shift && This_ctrl == Target_ctrl)
            {
                return true;
            }
        }

        return false;
    }


    //-----------------------------------------------------------------------------------------------------
    // mouse
    inline InputManager::Mouse::Mouse(const uint32_t timeStamp, const uint32_t flags, const Vector2& pos) : InputManager::Input(tMouse, timeStamp, flags),
        mPos(pos),
        mButtons(32)
    {
        mButtons.Clear();
    }

    //---------------------------------------------------------------------------------------------------------
    inline InputManager::Mouse::Mouse(const uint32_t flags, const Vector2& pos) : InputManager::Input(tMouse, 0, flags),
        mPos(pos),
        mButtons(32)
    {
        mButtons.Clear();
    }

    //---------------------------------------------------------------------------------------------------------
    inline bool InputManager::Mouse::Is(const Input *target, bool and) const
    {
        if (Input::Is(target, and))
        {
            and = false;    // when we check mouse buttons, it must be perfect match.
            const Mouse *S = static_cast<const Mouse *>(target);
            // check for the same position or not if used -1 as the value
            if (mPos == S->mPos || S->mPos == Vector2(-1.0f, -1.0f))
            {
                // if Target does not have any buttons set, then we'll disregard the bits
                // and return TRUE, since position does match
                if (S->mButtons.AllBitsFalse())
                {
                    return true;
                }

                if (and)
                {
                    // \note so if we force and tp be false, why are we checking this here
                    BitArray Diff = mButtons & S->mButtons;
                    // if all the bits are cleared, then this is not the same key
                    return !Diff.AllBitsFalse();
                }
                else
                {
                    if (mButtons == S->mButtons)
                    {
                        return true;
                    }

                    /*
                    // this is button up event, we only have to make sure that the one
                    // we are looking for is the orginal one from defualtInput
                    if (mFlags & InputTypes::kButtonUp)
                    {
                        bool result = 0;
                        if (S->mButtons[InputTypes::kMouseLeft])
                        {
                            if (mButtons[InputTypes::kMouseLeft])
                            {
                            }

                        }
                        BitArray Diff = mButtons & S->mButtons;
                        // if all the bits are cleared, then this is not the same key
                        return !Diff.AllBitsFalse();
                    }
                    */
                }
            }
        }

        return false;
    }


    //-----------------------------------------------------------------------------------------------------
    // joypad
    inline InputManager::JoyPad::JoyPad(const uint32_t timeStamp, const uint32_t flags) : Input(tJoyPad, timeStamp, flags),
        mDir(-1.0f),
        mButtons(0)
    {
    }

    /*!
    If Input is KeyInput type and mValue is same as Key or if mValue is 0, return TRUE.
    This allows us to test for specific key as well if the Input is Key type.
    */
    inline bool IsKey(const InputManager::Input *input, const unsigned char value = 0, bool keyDown = true)
    {
        bool result = false;
        if (input && InputManager::tKey == input->mType)
        {
            const InputManager::Key *Key = static_cast<const InputManager::Key *>(input);
            if ((keyDown && Key->mFlags& InputTypes::kButtonDown) || (!keyDown && Key->mFlags& InputTypes::kButtonUp))
            {
                if (value && Key->Is(value))
                {
                    result = true;
                }
                else if (!value)
                {
                    result = true;
                }
            }
        }

        return result;
    }

    //---------------------------------------------------------------------------------------------------------
    inline bool IsShift(const InputManager::Input *input)
    {
        if (input && input->mFlags & InputTypes::kButtonShift)
        {
            return true;
        }

        return false;
    }


    inline bool IsCtrl(const InputManager::Input *input)
    {
        if (input && input->mFlags & InputTypes::kButtonCtrl)
        {
            return true;
        }

        return false;
    }
    //---------------------------------------------------------------------------------------------------------


    //---------------------------------------------------------------------------------------------------------
    inline InputAction::InputAction()
    {
    }

    inline InputAction::InputAction(const char *actionName, const InputManager::Input *pInput, const char *contextName, bool compareAsOr) :
        mActionName(actionName)
    {
        RegisterAction(actionName, pInput, contextName, compareAsOr);
    }

    inline InputAction::InputAction(const char *actionName, const InputManager::Input *pInput, const char *contextName, InputActionCallback_t inputActionCallback, bool compareAsOr) :
        mActionName(actionName)
    {
        RegisterAction(actionName, pInput, contextName, inputActionCallback, compareAsOr);
    }

    inline InputAction::~InputAction()
    {
        Clear();
    }

    inline void InputAction::RegisterAction(const char *actionName, const InputManager::Input *pInput, const char *contextName, bool compareAsOr)
    {
        mActionName = actionName;
        compareAsOr;contextName;pInput;
        throw ex::program("InputAction::RegisterAction not implemented yet.");
        //if (InputManager *im = registrate::p_cast<InputManager>("InputManager"))
        //{
        //    im->RegisterAction(actionName, pInput, contextName, compareAsOr);
        //}
    }

    inline void InputAction::RegisterAction(const char *actionName, const InputManager::Input *pInput, const char *contextName, InputActionCallback_t inputActionCallback, bool compareAsOr)
    {
        mActionName = actionName;
        compareAsOr;contextName;pInput;
        throw ex::program("InputAction::RegisterAction not implemented yet.");
        //if (InputManager *im = registrate::p_cast<InputManager>("InputManager"))
        //{
        //    im->RegisterAction(actionName, pInput, contextName, inputActionCallback, compareAsOr);
        //}
    }

    inline bool InputAction::IsAction() const
    {
        throw ex::program("InputAction::IsAction not implemented yet.");
        //if (InputManager *im = registrate::p_cast<InputManager>("InputManager"))
        //{
        //    return im->IsAction(mActionName.c_str());
        //}

        return false;
    }

    inline void InputAction::Clear()
    {
        if (!mActionName.empty())
        {
            throw ex::program("InputAction::Clear not implemented yet.");
            //if (InputManager *im = registrate::p_cast<InputManager>("InputManager"))
            //{
            //    im->UnregisterAction(mActionName.c_str());
            //    mActionName.clear();
            //}
        }
    }
    //---------------------------------------------------------------------------------------------------------


    //---------------------------------------------------------------------------------------------------------
    inline InputSwitch::InputSwitch(const char *actionNameOn, const char *actionNameOff)
    : mActionOn(false)
    , mLastMousePosition(v2::ZERO())
    {
        actionNameOn;actionNameOff;
        throw ex::program("InputSwitch::InputSwitch not implemented yet.");
        //InputManager& im = REGISTRATE(InputManager);
        //mConnectionOn = im.ListenForAction(actionNameOn, boost::bind(&InputSwitch::onRegisteredActionOn, this, _1, _2, _3, _4));
        //mConnectionOff = im.ListenForAction(actionNameOff, boost::bind(&InputSwitch::onRegisteredActionOff, this, _1, _2, _3, _4));

    }

    inline Vector2 InputSwitch::getMouseDelta() const
    {
        if (isOn())
        {
            Vector2 currMousePos = getMousePos();
            Vector2 delta = currMousePos - mLastMousePosition;
            mLastMousePosition = currMousePos;
            return delta;
        }

        return v2::ZERO();
    }

    inline Vector2 InputSwitch::getMousePos() const
    {
        throw ex::program("InputSwitch::getMousePos not implemented yet.");
        //InputManager& im = REGISTRATE(InputManager);
        //return im.GetCurrentMousePos();
    }

    inline void InputSwitch::onRegisteredActionOn(const std::string& /*actionName*/, uint32_t /*timeStamp*/, int32_t /*mouseX*/, int32_t /*mouseY*/)
    {
        mActionOn = true;
        mLastMousePosition = getMousePos();
    }

    inline void InputSwitch::onRegisteredActionOff(const std::string& /*actionName*/, uint32_t /*timeStamp*/, int32_t /*mouseX*/, int32_t /*mouseY*/)
    {
        mActionOn = false;
    }
    //---------------------------------------------------------------------------------------------------------


} // namespace eg

#endif // INPUT_MANAGER_IMPL_H

