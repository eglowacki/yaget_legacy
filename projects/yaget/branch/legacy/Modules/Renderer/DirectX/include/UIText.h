/////////////////////////////////////////////////////////////////////////
// UIText.h
//
//  Copyright 5/9/2009 Edgar Glowacki.
//
// NOTES:
//      This class provides runtime instance of UIAsset for text rendering
//
//
// #include "UIText.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef UI_TEXT_H
#define UI_TEXT_H
#pragma once

#include "Base.h"
#include "Math/Vector.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>

class GFxDrawTextManager;
namespace {class PlateText;}


namespace eg
{
    class Message;

    class UIText// : boost::noncopyable
    {
        // no copy semantics
        UIText(const UIText&);
        UIText& operator=(const UIText&);

    public:
        //! This class represents one unique instance of user text.
        //! It is only return by reference.
        class Plate
        {
            // no copy semantics
            Plate(const Plate&);
            Plate& operator=(const Plate&);

        public:
            virtual void Print(int32_t x, int32_t y, const Color4_t& color, const std::string& text);
            uint32_t Token() const {return mToken;}

        protected:
            Plate(uint32_t token);
            ~Plate();

            uint32_t mToken;
        };

        UIText(GFxDrawTextManager *mTextRenderer);
        ~UIText();
        void Resize(const math::Rect_t& rect);
        void Render();
        void Tick(float deltaTime);

        //! Return reference to Plate
        //!
        //! \param token if -1, then it will generate new Plate
        //!              otherwise return existing one
        //!
        //! \return Plate&
        Plate& GetPlate(uint32_t token);

        virtual void HandleMessage(const Message &msg);

    private:
        GFxDrawTextManager *mTextRenderer;
        boost::ptr_vector<PlateText> mPlates;
        math::Rect_t mRect;
    };

    typedef boost::shared_ptr<UIText> UIText_t;

} // namespace eg

#endif // UI_TEXT_H

