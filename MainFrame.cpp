#include "MainFrame.h"
#include "BoardPanel.h"
#include "core/ComputerPlayer.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <wx/app.h>
#include <wx/event.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_TIMER(wxID_ANY, MainFrame::OnAITick)
wxEND_EVENT_TABLE()

static ComputerPlayer g_cpu;

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "SOS Game", wxDefaultPosition, wxSize(900, 700)),
    m_aiTimer(this)
{
    BuildUI();
    StartNewGame();
}

void MainFrame::BuildUI() {
    auto* root = new wxPanel(this);
    auto* rootSizer = new wxBoxSizer(wxVERTICAL);

    // settings + scores
    m_top = new wxPanel(root);
    auto* topSizer = new wxBoxSizer(wxHORIZONTAL);

    // Board size
    topSizer->Add(new wxStaticText(m_top, wxID_ANY, "Size:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_sizeSpin = new wxSpinCtrl(m_top, wxID_ANY, "5", wxDefaultPosition, wxSize(60, -1));
    m_sizeSpin->SetRange(3, 10);
    m_sizeSpin->SetValue(5);
    topSizer->Add(m_sizeSpin, 0, wxRIGHT, 12);

    // Simple or General mode
    wxString modes[] = { "Simple", "General" };
    m_modeRadio = new wxRadioBox(m_top, wxID_ANY, "Game Mode", wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(modes), modes, 1, wxRA_SPECIFY_ROWS);
    m_modeRadio->SetSelection(0);
    topSizer->Add(m_modeRadio, 0, wxRIGHT, 12);

    // Choose S or O
    wxString letters[] = { "S", "O" };
    m_letterRadio = new wxRadioBox(m_top, wxID_ANY, "Letter", wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(letters), letters, 1, wxRA_SPECIFY_ROWS);
    m_letterRadio->SetSelection(0);
    topSizer->Add(m_letterRadio, 0, wxRIGHT, 12);

    m_blueCPU = new wxCheckBox(m_top, wxID_ANY, "Blue is CPU");
    m_redCPU = new wxCheckBox(m_top, wxID_ANY, "Red is CPU");
    topSizer->Add(m_blueCPU, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    topSizer->Add(m_redCPU, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    // New game button
    m_newGameBtn = new wxButton(m_top, wxID_ANY, "New Game");
    topSizer->Add(m_newGameBtn, 0, wxRIGHT, 12);

    // Show whose turn
    m_turnLabel = new wxStaticText(m_top, wxID_ANY, "Current turn: Blue");
    topSizer->Add(m_turnLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    m_scoreBlue = new wxStaticText(m_top, wxID_ANY, "Blue: 0");
    m_scoreRed = new wxStaticText(m_top, wxID_ANY, "Red: 0");
    topSizer->Add(m_scoreBlue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    topSizer->Add(m_scoreRed, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    m_top->SetSizer(topSizer);
    rootSizer->Add(m_top, 0, wxEXPAND | wxALL, 6);

    m_boardPanel = new BoardPanel(root, this);
    rootSizer->Add(m_boardPanel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);

    root->SetSizer(rootSizer);

    m_newGameBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { StartNewGame(); });
    m_modeRadio->Bind(wxEVT_RADIOBOX, [this](wxCommandEvent&) { StartNewGame(); });
    m_sizeSpin->Bind(wxEVT_SPINCTRL, [this](wxCommandEvent&) { StartNewGame(); });
    m_blueCPU->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) { AITimer(); });
    m_redCPU->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) { AITimer(); });
}

void MainFrame::StartNewGame() {
    const int size = std::clamp(m_sizeSpin->GetValue(), 3, 10);

    GameConfig cfg;
    cfg.size = size;
    cfg.mode = (m_modeRadio->GetSelection() == 0) ? GameMode::Simple
        : GameMode::General;

    m_game.reset(cfg);
    m_boardPanel->SetBoardSize(size);

    RefreshTurnLabel();
    RefreshScore();
    m_boardPanel->Refresh();

    AITimer();
}

void MainFrame::RefreshTurnLabel() {
    const bool blueTurn = (m_game.currentPlayer() == Player::Blue);
    wxString who = blueTurn ? "Blue" : "Red";
    m_turnLabel->SetLabel("Current turn: " + who);

    m_turnLabel->SetForegroundColour(blueTurn ? wxColour(30, 144, 255)   // blue
        : wxColour(220, 20, 60));  // red
    m_turnLabel->Refresh();
}

void MainFrame::RefreshScore() {
    if (!m_scoreBlue || !m_scoreRed) return;
    m_scoreBlue->SetLabel(wxString::Format("Blue: %d", m_game.scoreBlue()));
    m_scoreRed->SetLabel(wxString::Format("Red: %d", m_game.scoreRed()));
}

void MainFrame::OnCellClicked(int row, int col) {
    if (m_game.result() != GameResult::Ongoing) return;

    const bool blueTurn = (m_game.currentPlayer() == Player::Blue);
    if ((blueTurn && m_blueCPU->IsChecked()) ||
        (!blueTurn && m_redCPU->IsChecked())) {
        return; // human shouldn't click during CPU turn
    }

    const Cell letter = (m_letterRadio->GetSelection() == 0) ? Cell::S : Cell::O;
    auto out = m_game.placeLetter(row, col, letter);

    RefreshScore();
    RefreshTurnLabel();
    m_boardPanel->Refresh();
    m_boardPanel->Update();
    if (wxTheApp) wxTheApp->Yield(true);

    if (m_game.result() != GameResult::Ongoing) {
        wxString msg = "Draw!";
        if (m_game.result() == GameResult::BlueWins) msg = "Blue wins!";
        else if (m_game.result() == GameResult::RedWins) msg = "Red wins!";
        wxMessageBox(msg, "Game Over", wxOK | wxICON_INFORMATION, this);
        return;
    }

    AITimer();

}

void MainFrame::AITimer() {
    if (m_game.result() != GameResult::Ongoing) { m_aiTimer.Stop(); return; }

    const bool blueTurn = (m_game.currentPlayer() == Player::Blue);
    const bool cpuTurn = (blueTurn && m_blueCPU->IsChecked()) ||
        (!blueTurn && m_redCPU->IsChecked());

    if (cpuTurn) {
        if (!m_aiTimer.IsRunning()) m_aiTimer.Start(1100);
    }
    else {
        m_aiTimer.Stop();
    }
}

void MainFrame::OnAITick(wxTimerEvent&) {
    if (m_game.result() != GameResult::Ongoing) { m_aiTimer.Stop(); return; }

    const bool blueTurn = (m_game.currentPlayer() == Player::Blue);
    const bool cpuTurn = (blueTurn && m_blueCPU->IsChecked()) ||
        (!blueTurn && m_redCPU->IsChecked());
    if (!cpuTurn) { m_aiTimer.Stop(); return; }

    // pick AI move
    auto move = g_cpu.chooseMove(m_game.board());
    if (!move) { m_aiTimer.Stop(); return; }

    auto [r, c, L] = *move;
    auto out = m_game.placeLetter(r, c, L);

    RefreshScore();
    RefreshTurnLabel();
    m_boardPanel->Refresh();
    m_boardPanel->Update();
    if (wxTheApp) wxTheApp->Yield(true);

    if (m_game.result() != GameResult::Ongoing) {
        m_aiTimer.Stop();
        wxString msg = "Draw!";
        if (m_game.result() == GameResult::BlueWins) msg = "Blue wins!";
        else if (m_game.result() == GameResult::RedWins) msg = "Red wins!";
        wxMessageBox(msg, "Game Over", wxOK | wxICON_INFORMATION, this);
        return;
    }

    AITimer();
}
