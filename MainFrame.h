#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "core/GameFacade.h"

class BoardPanel;

class MainFrame : public wxFrame {
public:
    MainFrame();
    void OnCellClicked(int row, int col);
    GameFacade& Game() { return m_game; }
    const GameFacade& Game() const { return m_game; }

private:
    // UI build + helpers
    void BuildUI();
    void StartNewGame();
    void RefreshTurnLabel();
    void RefreshScore();

    // Controls
    wxPanel* m_top{ nullptr };
    wxSpinCtrl* m_sizeSpin{ nullptr };
    wxRadioBox* m_modeRadio{ nullptr };    // 0 = Simple, 1 = General
    wxRadioBox* m_letterRadio{ nullptr };  // 0 = S, 1 = O
    wxButton* m_newGameBtn{ nullptr };
    wxStaticText* m_turnLabel{ nullptr };
    wxStaticText* m_scoreBlue{ nullptr };
    wxStaticText* m_scoreRed{ nullptr };

    BoardPanel* m_boardPanel{ nullptr };
    GameFacade    m_game;
};
