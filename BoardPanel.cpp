#include "BoardPanel.h"
#include "MainFrame.h"
#include "core/Board.h"
#include "core/GameFacade.h"
#include "core/Cell.h"
#include <wx/dcbuffer.h>

static wxColour PlayerBlueColour() { return wxColour(30, 144, 255); } // blue
static wxColour PlayerRedColour() { return wxColour(220, 20, 60); }   // red
static wxColour NeutralColour() { return *wxBLACK; }

BoardPanel::BoardPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent, wxID_ANY), m_frame(frame)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &BoardPanel::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &BoardPanel::OnLeftDown, this);
}

void BoardPanel::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxRect r = GetClientRect();
    DrawGrid(dc, r);
    DrawLetters(dc, r);
    DrawHighlights(dc, r); // draws red lines when SOS found
}

void BoardPanel::OnLeftDown(wxMouseEvent& evt) {
    wxRect r = GetClientRect();
    const int N = std::max(1, m_boardSize);
    const int cellW = std::max(1, r.GetWidth() / N);
    const int cellH = std::max(1, r.GetHeight() / N);

    // Figure out which cell the mouse clicked in.
    const wxPoint p = evt.GetPosition();
    int col = (p.x - r.GetX()) / cellW;
    int row = (p.y - r.GetY()) / cellH;

    // Make sure click is inside the board.
    if (row >= 0 && row < N && col >= 0 && col < N) {
        m_frame->OnCellClicked(row, col);
    }
}

void BoardPanel::DrawGrid(wxDC& dc, const wxRect& r) const {
    const int N = std::max(1, m_boardSize);
    const int cellW = std::max(1, r.GetWidth() / N);
    const int cellH = std::max(1, r.GetHeight() / N);

    dc.SetPen(wxPen(*wxBLACK, 1));

    // Board grid
    for (int c = 0; c <= N; ++c) {
        int x = r.GetX() + c * cellW;
        dc.DrawLine(x, r.GetY(), x, r.GetBottom());
    }
    for (int rr = 0; rr <= N; ++rr) {
        int y = r.GetY() + rr * cellH;
        dc.DrawLine(r.GetX(), y, r.GetRight(), y);
    }
}

void BoardPanel::DrawLetters(wxDC& dc, const wxRect& r) const {
    const auto& g = m_frame->Game();
    const auto& b = g.board();
    const int N = b.size();
    if (N <= 0) return;

    const int cellW = std::max(1, r.GetWidth() / N);
    const int cellH = std::max(1, r.GetHeight() / N);

    wxFont font(std::min(cellW, cellH) * 0.6, wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc.SetFont(font);

    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            Cell L = b.at(row, col);
            if (L == Cell::Empty) continue;

            // Change color depending on who placed it.
            auto owner = g.ownerAt(row, col);
            if (owner && *owner == Player::Blue)      dc.SetTextForeground(PlayerBlueColour());
            else if (owner && *owner == Player::Red)  dc.SetTextForeground(PlayerRedColour());
            else                                      dc.SetTextForeground(NeutralColour());

            wxString ch = (L == Cell::S) ? "S" : "O";

            int x = r.GetX() + col * cellW;
            int y = r.GetY() + row * cellH;
            wxSize tw = dc.GetTextExtent(ch);

            int cx = x + (cellW - tw.GetWidth()) / 2;
            int cy = y + (cellH - tw.GetHeight()) / 2;

            dc.DrawText(ch, cx, cy);
        }
    }
}

void BoardPanel::DrawHighlights(wxDC& dc, const wxRect& r) const {
    const auto& g = m_frame->Game();
    const auto& seqs = g.lastSequences();
    const int N = g.board().size();
    if (seqs.empty() || N <= 0) return;

    const int cellW = std::max(1, r.GetWidth() / N);
    const int cellH = std::max(1, r.GetHeight() / N);

    // Red lines to show the SOS found.
    wxPen pen(wxColour(255, 0, 0), std::max(3, std::min(cellW, cellH) / 10));
    dc.SetPen(pen);

    auto center = [&](int rr, int cc) {
        int cx = r.GetX() + cc * cellW + cellW / 2;
        int cy = r.GetY() + rr * cellH + cellH / 2;
        return wxPoint(cx, cy);
        };

    for (const auto& s : seqs) {
        wxPoint a = center(s.a.r, s.a.c);
        wxPoint b = center(s.b.r, s.b.c);
        wxPoint c = center(s.c.r, s.c.c);
        dc.DrawLine(a, b);
        dc.DrawLine(b, c);
    }
}
