//////////////////////////////////////////////////////////////////////
// EditorHelpers.h
//
//  Copyright 12/25/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Hellper objects and functions for dealing with UI and editor functionality
//
//
//  #include "EditorHelpers.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef EDITOR_HELPERS_H
#define EDITOR_HELPERS_H
#pragma once

namespace eg
{
    /*!
    Popup menu helper, which will return Id if selected menu or -1 if there
    was no selection.
    */
    class PopupMenu : public wxMenu
    {
    public:
        //! If pWindow is valid, then it will trigger displaying of menu popup
        //! and then call GetSelection() method to get which menu was selected
        PopupMenu(wxWindow *pWindow = 0);
        virtual ~PopupMenu();

        //! Activate popup and return selected menu Id or -1 if canceld.
        int32_t Choose(wxWindow *pWindow);
        int32_t GetSelection() const;

    private:
        void OnSelection(wxMenuEvent& event) {mSelection = event.GetId();}
        int32_t mSelection;
    };


    //-------------------------------------------------------------------------------------
    inline PopupMenu::PopupMenu(wxWindow *pWindow) : wxMenu(), mSelection(-1)
    {
        Connect(wxEVT_COMMAND_MENU_SELECTED,
               (wxObjectEventFunction)
               (wxEventFunction)
               (wxMenuEventFunction)
               (&PopupMenu::OnSelection));

        if (pWindow)
        {
            pWindow->PopupMenu(this);
        }
    }

    inline PopupMenu::~PopupMenu()
    {
        Disconnect(wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction)
                  (wxEventFunction)
                  (wxMenuEventFunction)
                  (&PopupMenu::OnSelection));
    }

    inline int32_t PopupMenu::Choose(wxWindow *pWindow)
    {
        wxASSERT(pWindow);
        if (pWindow)
        {
            pWindow->PopupMenu(this);
        }

        return GetSelection();
    }

    inline int32_t PopupMenu::GetSelection() const {return mSelection;}


} // namespace eg

#endif // EDITOR_HELPERS_H

