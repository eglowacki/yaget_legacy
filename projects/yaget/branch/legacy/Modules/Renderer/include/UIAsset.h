/////////////////////////////////////////////////////////////////////////
// UIAsset.h
//
//  Copyright 5/9/2009 Edgar Glowacki.
//
// NOTES:
//
//
// #include "UIAsset.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file


#ifndef UI_ASSET_H
#define UI_ASSET_H
#pragma once

#include "Asset/IAsset.h"
#include "UISheet.h"
#include "UIText.h"


class GFxMovieDef;
class GFxRenderConfig;
struct IDirect3DDevice9;
struct _D3DPRESENT_PARAMETERS;

namespace eg
{
    class AssetLoadSystem;
    class IdGameCache;
    class Dispatcher;
    class InputManager;
    //class PluginManager;
    namespace {class UIAssetFactory;}


    class UIAsset : public IAsset
    {
    public:
        static const guid_t kType = 0x33e8ccdc;

        virtual ~UIAsset();

        UISheet_t GetSheet() const;
        UIText_t GetText() const;

        // from IAsset
        virtual uint64_t GetId() const;
        virtual const std::string& Name() const;
        virtual const std::string& Fqn() const;
        virtual Locker_t Lock();
        virtual guid_t Type() const;
        virtual void HandleMessage(const Message &msg);

        static Token Register(AssetLoadSystem &als);
        static bool Attach(Token token, IDirect3DDevice9* device, _D3DPRESENT_PARAMETERS *presentParams);

    private:
        UIAsset(uint64_t id, const std::string& name, GFxRenderConfig *renderConfig);

        uint64_t mId;
        std::string mName;
        std::string mFqn;
        GFxMovieDef *mMovieDef;
        GFxRenderConfig *mRenderConfig;

        friend UIAssetFactory;
    };

    typedef boost::shared_ptr<UIAsset> UIAsset_t;

} // namespace eg


#define tr_ui "ui"

#endif // UI_ASSET_H

