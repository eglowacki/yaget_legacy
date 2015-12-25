/////////////////////////////////////////////////////////////////////////
// UISheet.h
//
//  Copyright 5/9/2009 Edgar Glowacki.
//
// NOTES:
//      This class provides runtime instance of UIAsset
//
//
// #include "UISheet.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef UISHEET_H
#define UISHEET_H
#pragma once

#include "Base.h"
#include "Math/Vector.h"
#include <boost/shared_ptr.hpp>

class GFxMovieView;


namespace eg
{
    class Message;

    class UISheet
    {
        // no copy semantics
        UISheet(const UISheet&);
        UISheet& operator=(const UISheet&);

    public:
        UISheet(GFxMovieView *movie);
        ~UISheet();
        void Resize(const math::Rect_t& rect);
        void Render();
        void Tick(float deltaTime);

        virtual void HandleMessage(const Message &msg);

    private:
        GFxMovieView *mMovie;
    };

    typedef boost::shared_ptr<UISheet> UISheet_t;

} // namespace eg

#endif // UISHEET_H

