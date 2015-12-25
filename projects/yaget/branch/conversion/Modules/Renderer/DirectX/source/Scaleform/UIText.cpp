//#include "Precompiled.h"
#include "UIText.h"
#include "MessageInterface.h"
#include "GRefCount.h"
#include "GFxDrawText.h"
#include <boost/foreach.hpp>


namespace {
    using namespace eg;

    /*
    struct rgba
    {
        rgba(const Color4_t& color)
        : r(static_cast<uint8_t>(color.x/1.0f))
        , g(static_cast<uint8_t>(color.y/1.0f))
        , b(static_cast<uint8_t>(color.z/1.0f))
        , a(static_cast<uint8_t>(color.w/1.0f))
        {}

        operator GColor() const
        {
            return GColor(r, g, b, a);
        }

        uint8_t r, g, b, a;
    };
    */


    GColor rgba(const Color4_t& color)
    {
        return GColor(static_cast<uint8_t>(color.x*255), static_cast<uint8_t>(color.y*255), static_cast<uint8_t>(color.z*255), static_cast<uint8_t>(color.w*255));
    }

    GRectF rect_size(const math::Rect_t& rect)
    {
        return GRectF(rect.x, rect.y, rect.z, rect.w);
    }


    class PlateText : public UIText::Plate
    {
    public:
        PlateText(uint32_t token, GPtr<GFxDrawText> text, GFxDrawTextManager *textRenderer)
        : UIText::Plate(token)
        , mText(text)
        , mEmpty(true)
        , mTextRenderer(textRenderer)
        {
        }

        void Render()
        {
            if (!mEmpty)
            {
                mText->Display();
            }
        }

        // from UIText::Plate
        virtual void Print(int32_t x, int32_t y, const Color4_t& color, const std::string& text)
        {
            mEmpty = text.empty();
            mText->SetText(text.c_str());
            mText->SetFont("Tahoma");
            mText->SetFontSize(12);
            //mText->SetFontStyle(GFxDrawText::Bold);
            mText->SetColor(rgba(color));
            //mText->SetBackgroundColor(GColor(50, 50, 50, 50));

            GFxDrawTextManager::TextParams defParams = mTextRenderer->GetDefaultTextParams();
            defParams.FontSize = 12;
            defParams.FontName = "Tahoma";
            //defParams.FontStyle = GFxDrawText::Bold;
            GSizeF textSize = mTextRenderer->GetTextExtent(text.c_str(), 0, &defParams);
            float left = static_cast<float>(x);
            float top = static_cast<float>(y);
            mText->SetRect(GRectF(left, top, textSize.Width+left, textSize.Height+top));
        }

    private:
        GPtr<GFxDrawText> mText;
        bool mEmpty;
        GFxDrawTextManager *mTextRenderer;
    };


} // namespace

namespace eg {

UIText::Plate::Plate(uint32_t token)
: mToken(token)
{
}


UIText::Plate::~Plate()
{
}


void UIText::Plate::Print(int32_t x, int32_t y, const Color4_t& color, const std::string& text)
{
    x;y;color;text;
}


UIText::UIText(GFxDrawTextManager *textRenderer)
: mTextRenderer(textRenderer)
, mRect(v4::ZERO())
{
}


UIText::~UIText()
{
    mPlates.erase(mPlates.begin(), mPlates.end());
    if (mTextRenderer)
    {
        mTextRenderer->Release();
    }
}


void UIText::Resize(const math::Rect_t& rect)
{
    mRect = rect;
}


void UIText::Render()
{
    int32_t width = static_cast<int32_t>(mRect.z - mRect.x);
    int32_t height = static_cast<int32_t>(mRect.w - mRect.y);
    GViewport vp(width, height, 0, 0, width, height, 0);

    mTextRenderer->BeginDisplay(vp);
    BOOST_FOREACH(PlateText& plate, mPlates)
    {
        plate.Render();
    }
    mTextRenderer->EndDisplay();
}


void UIText::Tick(float /*deltaTime*/)
{
}


UIText::Plate& UIText::GetPlate(uint32_t token)
{
    if (token == (uint32_t)-1)
    {
        GPtr<GFxDrawText> text = *mTextRenderer->CreateText();
        token = mPlates.size();
        mPlates.push_back(new PlateText(token, text, mTextRenderer));
    }

    return mPlates[token];
}


void UIText::HandleMessage(const Message& /*msg*/)
{
}



} // namespace eg

