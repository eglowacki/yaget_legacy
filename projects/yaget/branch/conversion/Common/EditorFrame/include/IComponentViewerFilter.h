//////////////////////////////////////////////////////////////////////
// IComponentViewerFilter.h
//
//  Copyright 12/25/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Abstract class to provide interface for filtering components and to show it in a tree control
//      \note do we need to be constrain by type of control we are using?
//
//
//  #include "IComponentViewerFilter.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef I_COMPONENT_VIEWER_FILTER_H
#define I_COMPONENT_VIEWER_FILTER_H
#pragma once

#include "EditorBase.h"
#include "ObjectID.h"
#include <wx/treectrl.h>
#include <vector>

class wxTreeCtrl;

namespace eg
{
    //-------------------------------------------------------------------------------------
    /*!
    We need to associate our object data with each tree item
    which will represent ObjectID and Component ID.
    */
    class TreeItemData : public wxTreeItemData
    {
    public:
        TreeItemData(ObjectId objectID, Hash componentID, bool ghost) : wxTreeItemData(),
            mId(objectID, componentID), mGhost(ghost)
        {
        }

        TreeItemData(ComponentInstanceId_t id) : wxTreeItemData(),
            mId(id), mGhost(false)
        {
        }

        bool operator ==(const TreeItemData& source) const {return mId == source.mId;}
        const ComponentInstanceId_t& GetId() const {return mId;}
        bool IsGhost() const {return mGhost;}

    private:
        ComponentInstanceId_t mId;
        bool mGhost;
    };


    //-------------------------------------------------------------------------------------
    class IComponentViewerFilter
    {
    public:
        virtual ~IComponentViewerFilter() = 0 {}
        //! Attach this filter to this control
        virtual void AttachToControl(wxTreeCtrl *pTreeControl) = 0;
        //! Called to actually run
        virtual void Run() = 0;
    };


    //-------------------------------------------------------------------------------------
    //! Helper method to get tree item data
    inline ComponentInstanceId_t GetInstanceId(wxTreeCtrl *pTreeControl, wxTreeItemId item)
    {
        if (TreeItemData *pTreeItem = static_cast<TreeItemData *>(pTreeControl->GetItemData(item)))
        {
            return pTreeItem->GetId();
        }

        return ComponentInstanceId_t();
    }

    //-------------------------------------------------------------------------------------
    //! Get all the instances under parentTree, not including parentTree
    inline bool GetAllInstances(wxTreeCtrl *pTreeControl, wxTreeItemId root, std::vector<eg::ComponentInstanceId_t>& result)
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId search;
        wxTreeItemId item = pTreeControl->GetFirstChild(root, cookie);
        wxTreeItemId child;

        while (item.IsOk())
        {
            if (pTreeControl->ItemHasChildren(item))
            {
                GetAllInstances(pTreeControl, item, result);
            }

            eg::ComponentInstanceId_t id = eg::GetInstanceId(pTreeControl, item);
            result.push_back(id);
            item = pTreeControl->GetNextChild(root, cookie);
        }

        return !result.empty();
    }



} // namespace eg

#endif // I_COMPONENT_VIEWER_FILTER_H

