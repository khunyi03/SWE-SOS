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
#include <vector>

struct RecordedMove {
    int    row;
    int    col;
    Cell   letter;
    Player player; // Blue or Red who made this move
};

class BoardPanel;

class MainFrame : public wxFrame {
public:
    MainFrame();
    void OnCellClicked(int row, int col);
    const GameFacade& Game() const { return m_game; }

private:
    // UI build + helpers
    void BuildUI();
    void CreateRootLayout(wxPanel* root);
    void CreateTopPanel(wxPanel* parent);
    void CreateBoardPanel(wxPanel* parent);
    void BindEvents();
    void StartNewGame();
    void RefreshTurnLabel();
    void RefreshScore();
    void AITimer();   // start timer if it's a CPU's turn
    void OnAITick(wxTimerEvent&);

    std::unique_ptr<Players> m_blueCtrl;
    std::unique_ptr<Players> m_redCtrl;

    Players& controllerFor(Player p);

    ComputerPlayer m_computer;
    bool m_blueIsComputer = false;
    bool m_redIsComputer = true;
    void OnComputerTurn();

    void StartRecording();
    void SaveRecordingToFile();
    void ReplayFromFile(wxCommandEvent& evt);
    void ReplayTimer(wxTimerEvent& evt);

    void ApplyMove(int row, int col, Cell letter, bool fromReplay = false);

    // Controls
    wxPanel* m_root{ nullptr };
    wxPanel* m_top{ nullptr };
    wxSpinCtrl* m_sizeSpin{ nullptr };
    wxRadioBox* m_modeRadio{ nullptr };    // 0 = Simple, 1 = General
    wxRadioBox* m_letterRadio{ nullptr };  // 0 = S, 1 = O
    wxButton* m_newGameBtn{ nullptr };
    wxStaticText* m_turnLabel{ nullptr };
    wxStaticText* m_scoreBlue{ nullptr };
    wxStaticText* m_scoreRed{ nullptr };
    wxCheckBox* m_blueCPU{ nullptr };
    wxCheckBox* m_redCPU{ nullptr };

    wxCheckBox* m_recordCheck{ nullptr };
    wxButton* m_replayBtn{ nullptr };

    BoardPanel* m_boardPanel{ nullptr };
    GameFacade  m_game;

    wxTimer m_aiTimer;
    wxTimer m_replayTimer;

    bool m_isRecording = false;
    bool m_isReplaying = false;

    std::vector<RecordedMove> m_recordedMoves;
    std::vector<RecordedMove> m_replayMoves;
    std::size_t               m_replayIndex = 0;

    wxDECLARE_EVENT_TABLE();
};
