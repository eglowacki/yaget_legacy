//#include "Precompiled.h"
#include "UISheet.h"
#include "Input/InputManager.h"
#include "MessageInterface.h"
#include "GFxPlayer.h"


namespace eg {

UISheet::UISheet(GFxMovieView *movie)
: mMovie(movie)
{
    mMovie->SetBackgroundAlpha(0.0f);
}


UISheet::~UISheet()
{
    mMovie->Release();
}


void UISheet::Resize(const math::Rect_t& rect)
{
    DWORD windowWidth = static_cast<DWORD>(rect.z - rect.x);
    DWORD windowHeight = static_cast<DWORD>(rect.w - rect.y);
    mMovie->SetViewport(windowWidth, windowHeight, 0, 0, windowWidth, windowHeight);
}


void UISheet::Render()
{
    mMovie->Display();
}


void UISheet::Tick(float deltaTime)
{
    mMovie->Advance(deltaTime, 0);
}


void UISheet::HandleMessage(const Message &msg)
{
    if (msg.mType == mtype::kInput)
    {
        const InputManager::Input *input = msg.GetValue<const InputManager::Input *>();

        if (input->mType == InputManager::tMouse)
        {
            const InputManager::Mouse *mouse = static_cast<const InputManager::Mouse *>(input);

            if (mouse->mFlags & InputTypes::kMouseMove)
            {
                GFxMouseEvent mevent(GFxEvent::MouseMove, 0, mouse->mPos.x, mouse->mPos.y);
                mMovie->HandleEvent(mevent);
            }
            else if (mouse->mFlags & InputTypes::kButtonDown)
            {
                GFxMouseEvent mevent(GFxEvent::MouseDown, 0, mouse->mPos.x, mouse->mPos.y);
                mMovie->HandleEvent(mevent);
            }
            else if (mouse->mFlags & InputTypes::kButtonUp)
            {
                GFxMouseEvent mevent(GFxEvent::MouseUp, 0, mouse->mPos.x, mouse->mPos.y);
                mMovie->HandleEvent(mevent);
            }
        }
        else if (input->mType == InputManager::tKey)
        {
        }
    }
}



} // namespace eg

