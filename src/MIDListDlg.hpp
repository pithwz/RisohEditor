#ifndef MZC4_MIDLISTDLG_HPP_
#define MZC4_MIDLISTDLG_HPP_

#include "MWindowBase.hpp"
#include "MTextToText.hpp"
#include "id_string.hpp"
#include "resource.h"

class MIDListDlg : public MDialogBase
{
public:
    typedef std::map<MString, MString>      assoc_map_type;
    typedef std::map<MStringA, MStringA>    id_map_type;
    const assoc_map_type *m_assoc_map;
    const id_map_type *m_id_map;

    MString GetAssoc(const assoc_map_type& assoc_map, const MString& name)
    {
        MString str;
        assoc_map_type::const_iterator it, end = assoc_map.end();
        for (it = assoc_map.begin(); it != end; ++it)
        {
            if (name.find(it->second) == 0)
            {
                if (str.empty())
                {
                    str = it->first;
                }
                else
                {
                    str += TEXT("/");
                    str += it->first;
                }
            }
        }
        return str;
    }

    static int CALLBACK
    CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        MIDListDlg *this_ = (MIDListDlg *)lParamSort;
        HWND m_hLst1 = this_->m_hLst1;

        LV_FINDINFO find;

        ZeroMemory(&find, sizeof(find));
        find.flags = LVFI_PARAM;
        find.lParam = lParam1;
        INT i1 = ListView_FindItem(m_hLst1, -1, &find);

        ZeroMemory(&find, sizeof(find));
        find.flags = LVFI_PARAM;
        find.lParam = lParam2;
        INT i2 = ListView_FindItem(m_hLst1, -1, &find);

        TCHAR sz1[64], sz2[64];
        if (i1 != -1 && i2 != -1)
        {
            ListView_GetItemText(m_hLst1, i1, 1, sz1, _countof(sz1));
            ListView_GetItemText(m_hLst1, i2, 1, sz2, _countof(sz2));
            int cmp = lstrcmp(sz1, sz2);
            if (cmp != 0)
                return cmp;

            ListView_GetItemText(m_hLst1, i1, 2, sz1, _countof(sz1));
            ListView_GetItemText(m_hLst1, i2, 2, sz2, _countof(sz2));
            MIdOrString id1(sz1);
            MIdOrString id2(sz2);
            if (id1 < id2)
                return -1;
            if (id1 > id2)
                return 1;
        }
        return 0;
    }

    void SetItems(const assoc_map_type& assoc_map, const id_map_type& id_map)
    {
        m_assoc_map = &assoc_map;
        m_id_map = &id_map;
        ListView_DeleteAllItems(m_hLst1);

        INT iItem = 0;
        id_map_type::const_iterator it, end = id_map.end();
        for (it = id_map.begin(); it != end; ++it)
        {
            LV_ITEM item;

            MString text1 = MAnsiToText(it->first.c_str()).c_str();
            MString text2 = GetAssoc(assoc_map, text1);
            MString text3 = MAnsiToText(it->second.c_str()).c_str();
            if (text2.empty())
                continue;

            ZeroMemory(&item, sizeof(item));
            item.iItem = iItem;
            item.mask = LVIF_TEXT | LVIF_PARAM;
            item.iSubItem = 0;
            item.pszText = &text1[0];
            item.lParam = iItem;
            ListView_InsertItem(m_hLst1, &item);

            ZeroMemory(&item, sizeof(item));
            item.iItem = iItem;
            item.mask = LVIF_TEXT;
            item.iSubItem = 1;
            item.pszText = &text2[0];
            ListView_SetItem(m_hLst1, &item);

            ZeroMemory(&item, sizeof(item));
            item.iItem = iItem;
            item.mask = LVIF_TEXT;
            item.iSubItem = 2;
            item.pszText = &text3[0];
            ListView_SetItem(m_hLst1, &item);

            ++iItem;
        }

        ListView_SortItems(m_hLst1, CompareFunc, (LPARAM)this);
    }

    MIDListDlg() : MDialogBase(IDD_IDLIST)
    {
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        m_hLst1 = GetDlgItem(hwnd, lst1);
        ListView_SetExtendedListViewStyle(m_hLst1, LVS_EX_FULLROWSELECT);

        LV_COLUMN column;
        ZeroMemory(&column, sizeof(column));

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 160;
        column.pszText = LoadStringDx(IDS_NAME);
        column.iSubItem = 0;
        ListView_InsertColumn(m_hLst1, 0, &column);

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 90;
        column.pszText = LoadStringDx(IDS_IDTYPE);
        column.iSubItem = 1;
        ListView_InsertColumn(m_hLst1, 1, &column);

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 80;
        column.pszText = LoadStringDx(IDS_VALUE);
        column.iSubItem = 2;
        ListView_InsertColumn(m_hLst1, 2, &column);

        return TRUE;
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDCANCEL:
            ::DestroyWindow(hwnd);
            break;
        }
    }

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_SIZE, OnSize);
        default:
            return DefaultProcDx();
        }
    }

    void OnSize(HWND hwnd, UINT state, int cx, int cy)
    {
        MoveWindow(m_hLst1, 0, 0, cx, cy, TRUE);
    }

protected:
    HWND m_hLst1;
};

#endif  // ndef MZC4_MIDLISTDLG_HPP_