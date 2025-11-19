#pragma once
#include <wx/wx.h>

class MainFrame;

class BoardPanel : public wxPanel {
public:
    BoardPanel(wxWindow* parent, MainFrame* frame);

    void SetBoardSize(int n) { m_boardSize = n; }

private:
    void OnPaint(wxPaintEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);

    void DrawGrid(wxDC& dc, const wxRect& r) const;
    void DrawLetters(wxDC& dc, const wxRect& r) const;
    void DrawHighlights(wxDC& dc, const wxRect& r) const;

    MainFrame* m_frame{ nullptr };
    int        m_boardSize{ 3 };
};
