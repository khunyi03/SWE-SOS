#include "MainFrame.h"
#include "BoardPanel.h"
#include <algorithm>
#include <wx/app.h>
#include <wx/event.h>

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "SOS Game", wxDefaultPosition, wxSize(900, 700))
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

    const Cell letter = (m_letterRadio->GetSelection() == 0) ? Cell::S : Cell::O;
    auto out = m_game.placeLetter(row, col, letter);

    RefreshScore();
    RefreshTurnLabel();
    m_boardPanel->Refresh();
    m_boardPanel->Update();
    if (m_top) { m_top->Refresh(); m_top->Update(); }

    if (wxTheApp) wxTheApp->Yield(true);

    // If the game ended show popup
    if (m_game.result() != GameResult::Ongoing) {
        wxString msg = "Draw!";
        if (m_game.result() == GameResult::BlueWins) msg = "Blue wins!";
        else if (m_game.result() == GameResult::RedWins) msg = "Red wins!";
        wxMessageBox(msg, "Game Over", wxOK | wxICON_INFORMATION, this);
    }
}
