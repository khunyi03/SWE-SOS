#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "core/GameFacade.h"
#include "core/ComputerPlayer.h"
#include "core/Cell.h"
#include "core/Player.h"
#include "core/GameConfig.h"
#include <wx/timer.h>
#include <thread>
#include <chrono>


class BoardPanel;

class MainFrame : public wxFrame {
public:
    MainFrame();
    void OnCellClicked(int row, int col);
    const GameFacade& Game() const { return m_game; }

private:
    // UI build + helpers
    void BuildUI();
    void StartNewGame();
    void RefreshTurnLabel();
    void RefreshScore();
    void AITimer();   // start timer if it's a CPU's turn
    void OnAITick(wxTimerEvent&);

    ComputerPlayer m_computer;
    bool m_blueIsComputer = false;
    bool m_redIsComputer = true;
    void OnComputerTurn();

    // Controls
    wxPanel* m_top{ nullptr };
    wxSpinCtrl* m_sizeSpin{ nullptr };
    wxRadioBox* m_modeRadio{ nullptr };    // 0 = Simple, 1 = General
    wxRadioBox* m_letterRadio{ nullptr };  // 0 = S, 1 = O
    wxButton* m_newGameBtn{ nullptr };
    wxStaticText* m_turnLabel{ nullptr };
    wxStaticText* m_scoreBlue{ nullptr };
    wxStaticText* m_scoreRed{ nullptr };
    wxCheckBox* m_blueCPU = nullptr;
    wxCheckBox* m_redCPU = nullptr;

    BoardPanel* m_boardPanel{ nullptr };
    GameFacade    m_game;

    wxTimer m_aiTimer;
    wxDECLARE_EVENT_TABLE();
};
