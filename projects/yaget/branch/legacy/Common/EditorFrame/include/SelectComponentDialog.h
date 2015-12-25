//////////////////////////////////////////////////////////////////////
// SelectComponentDialog.h
//
//  Copyright 12/25/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Select Component from a list of valid components
//
//
//  #include "SelectComponentDialog.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef SELECT_COMPONENT_DIALOG_H
#define SELECT_COMPONENT_DIALOG_H
#pragma once

#include "IComponentViewerFilter.h"
#include "IComponent.h"
#include <wx/treectrl.h>

namespace eg
{
    //----------------------------------------------------------------
    /*!
    Displays dialog box which allows selecting component
    */
    class SelectComponentDialog: public wxDialog
    {
        DECLARE_EVENT_TABLE()

    public:
        /// Constructors
        SelectComponentDialog();
        SelectComponentDialog(ComponentInstanceId_t instanceId, wxWindow *pParent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style, IComponentViewerFilter *pFilter);

        /// Creation
        bool Create(ComponentInstanceId_t instanceId, wxWindow *pParent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style, IComponentViewerFilter *pFilter);
        //! Return picked component type from the list.
        ComponentInstanceId_t GetResult() const;
        //! Return object name. This can be name typed by a user (read rigth) or already existing object (read only)
        //! Which one depends on instanceId param in ctor or Create methods
        wxString SelectComponentDialog::GetName() const;

    private:
        /// Initialises member variables
        void Init();

        /// Creates the controls and sizers
        void CreateControls();

        /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_FILTER_TEXT
        void OnTextFilter(wxCommandEvent& event);
        /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_NAME_TEXT
        void OnNameTextUpdated(wxCommandEvent& event);
        /// wxEVT_COMMAND_TREE_SEL_CHANGED event handler for ID_TREECTRL1
        void OnComponentSelected(wxTreeEvent& event);
        /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
        //void OnCancel(wxCommandEvent& event);
        /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
        void OnOK(wxCommandEvent& event);

        boost::shared_ptr<IComponentViewerFilter> mFilter;
        ComponentInstanceId_t mSelectedValue;
    };

    //-----------------------------------------------------------------------------
    #define ID_FILTER_TEXT  10001
    #define ID_TREE_CONTROL 10002
    #define ID_NAME_TEXT    10003

    // everything is in line
    BEGIN_EVENT_TABLE(SelectComponentDialog, wxDialog)
        EVT_TEXT(ID_FILTER_TEXT, SelectComponentDialog::OnTextFilter)
        EVT_TEXT(ID_NAME_TEXT, SelectComponentDialog::OnNameTextUpdated)
        //EVT_TREE_SEL_CHANGED(ID_TREE_CONTROL, SelectComponentDialog::OnComponentSelected)
        //EVT_BUTTON(wxID_CANCEL, SelectComponentDialog::OnCancel)
        EVT_BUTTON(wxID_OK, SelectComponentDialog::OnOK)
    END_EVENT_TABLE()


    //-----------------------------------------------------------------------------
    inline SelectComponentDialog::SelectComponentDialog()
    {
        Init();
    }

    //-----------------------------------------------------------------------------
    inline SelectComponentDialog::SelectComponentDialog(ComponentInstanceId_t instanceId, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style, IComponentViewerFilter *pFilter) :
        mSelectedValue(instanceId)
    {
        Init();
        Create(instanceId, parent, id, caption, pos, size, style, pFilter);
    }


    //-----------------------------------------------------------------------------
    inline bool SelectComponentDialog::Create(ComponentInstanceId_t instanceId, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style, IComponentViewerFilter *pFilter)
    {
        mSelectedValue = instanceId;
        SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
        wxDialog::Create( parent, id, caption, pos, size, style );

        CreateControls();

        if (wxTextCtrl *pTextControl = static_cast<wxTextCtrl *>(wxWindow::FindWindowById(ID_NAME_TEXT, this)))
        {
            // let's validate exisiting object id against object manager
            if (mSelectedValue.first.IsValid())
            {
                wxASSERT(GetObjectManager().GetObjectName(mSelectedValue.first) != wxT(""));
                // this is already created object, let's get the name
                pTextControl->ChangeValue(GetObjectManager().GetObjectName(mSelectedValue.first));
                pTextControl->SetEditable(false);
            }
            else
            {
                // since name needs to be entered, we set focus so user can enter new name
                // immediately
                pTextControl->SetFocus();
                // there is no valid name selected yet
                // let's disable OK button until there is something typed in it
                if (wxWindow *pOkButton = wxWindow::FindWindowById(wxID_OK, this))
                {
                    pOkButton->Enable(false);
                }
            }
        }

        if (pFilter)
        {
            // le't find our tree control
            if (wxTreeCtrl *pTreeControl = static_cast<wxTreeCtrl *>(wxWindow::FindWindowById(ID_TREE_CONTROL, this)))
            {
                pFilter->AttachToControl(pTreeControl);
                pFilter->Run();
            }

            mFilter.reset(pFilter);
        }
        return true;
    }

    inline void SelectComponentDialog::Init()
    {
    }


    //-----------------------------------------------------------------------------
    inline void SelectComponentDialog::CreateControls()
    {
        SelectComponentDialog* itemDialog1 = this;

        wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
        itemDialog1->SetSizer(itemBoxSizer2);

        wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
        itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 3);

        wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, _("Filter:"), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);

        wxTextCtrl* itemTextCtrl5 = new wxTextCtrl( itemDialog1, ID_FILTER_TEXT, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer3->Add(itemTextCtrl5, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 3);

        wxTreeCtrl* itemTreeCtrl6 = new wxTreeCtrl( itemDialog1, ID_TREE_CONTROL, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS |wxTR_LINES_AT_ROOT|wxTR_HIDE_ROOT|wxTR_ROW_LINES|wxTR_SINGLE );
        itemBoxSizer2->Add(itemTreeCtrl6, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 3);

        wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
        itemBoxSizer2->Add(itemBoxSizer7, 0, wxGROW|wxALL, 3);

        wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, wxID_STATIC, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer7->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);

        wxTextCtrl* itemTextCtrl9 = new wxTextCtrl( itemDialog1, ID_NAME_TEXT, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer7->Add(itemTextCtrl9, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 3);

        wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
        itemBoxSizer2->Add(itemBoxSizer10, 0, wxALIGN_RIGHT|wxALL, 3);

        wxButton* itemButton11 = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer10->Add(itemButton11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);

        wxButton* itemButton12 = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer10->Add(itemButton12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);

////@end SelectComponentDialog content construction
    }

    /*
    //-----------------------------------------------------------------------------
    inline void SelectComponentDialog::OnCancel( wxCommandEvent& event )
    {
        event.Skip();
        SetReturnCode(0);
    }
    */

    //-----------------------------------------------------------------------------
    inline void SelectComponentDialog::OnOK( wxCommandEvent& event )
    {
        event.Skip();
        if (wxTreeCtrl *pTreeControl = static_cast<wxTreeCtrl *>(wxWindow::FindWindowById(ID_TREE_CONTROL, this)))
        {
            wxTreeItemId treeItem = pTreeControl->GetSelection();
            ComponentInstanceId_t newValue = GetInstanceId(pTreeControl, treeItem);
            mSelectedValue.second = newValue.second;
        }
        //SetReturnCode(1);
    }

    //-----------------------------------------------------------------------------
    inline void SelectComponentDialog::OnTextFilter( wxCommandEvent& event )
    {
        event.Skip();
    }

    //-----------------------------------------------------------------------------
    void SelectComponentDialog::OnNameTextUpdated( wxCommandEvent& event )
    {
        event.Skip();
        if (wxWindow *pOkButton = wxWindow::FindWindowById(wxID_OK, this))
        {
            wxString currName = GetName();
            if (currName.empty())
            {
                pOkButton->Enable(false);
            }
            else
            {
                // we only will validate name against object manager if user actually
                // requested to create new object (using instanceId in ctor and Create methods)
                if (!mSelectedValue.first.IsValid())
                {
                    // also before we reactivate OK button, make sure
                    // that object name can pass some validation
                    // for now we just call object manager to see if that name already exist
                    if (GetObjectManager().GetObjectName(ObjectId(currName.c_str())) == wxT(""))
                    {
                        // this object does not exist in ObjectManager, so
                        // this is valid name
                        pOkButton->Enable(true);
                    }
                    else
                    {
                        // there is object by this name already
                        pOkButton->Enable(false);
                    }
                }
                else
                {
                    pOkButton->Enable(true);
                }
            }
        }
    }

    //-----------------------------------------------------------------------------
    inline void SelectComponentDialog::OnComponentSelected( wxTreeEvent& event )
    {
        // \note stopped working, using OnOK()
        event.Skip();

        if (wxTreeCtrl *pTreeControl = static_cast<wxTreeCtrl *>(wxWindow::FindWindowById(ID_TREE_CONTROL, this)))
        {
            ComponentInstanceId_t newValue = GetInstanceId(pTreeControl, event.GetItem());
            mSelectedValue.second = newValue.second;
        }
    }

    //-------------------------------------------------------------------------------------
    inline ComponentInstanceId_t SelectComponentDialog::GetResult() const
    {
        return mSelectedValue;
    }

    //-------------------------------------------------------------------------------------
    inline wxString SelectComponentDialog::GetName() const
    {
        if (wxTextCtrl *pTextControl = static_cast<wxTextCtrl *>(wxWindow::FindWindowById(ID_NAME_TEXT, this)))
        {
            return pTextControl->GetValue();
        }

        return wxEmptyString;
    }


} // namespace eg

#endif // SELECT_COMPONENT_DIALOG_H

