#include "MainFrame.h"
#include "BoardPanel.h"
#include "core/ComputerPlayer.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/filedlg.h>
#include <fstream>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_TIMER(wxID_ANY, MainFrame::OnAITick)
wxEND_EVENT_TABLE()

Players& MainFrame::controllerFor(Player p)
{
    return (p == Player::Blue) ? *m_blueCtrl : *m_redCtrl;
}

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "SOS Game", wxDefaultPosition, wxSize(900, 700)),
    m_aiTimer(this),
    m_replayTimer(this)
{
    BuildUI();

    Bind(wxEVT_TIMER, &MainFrame::ReplayTimer, this, m_replayTimer.GetId());

    StartNewGame();
}

void MainFrame::BuildUI()
{
    m_root = new wxPanel(this);
    CreateRootLayout(m_root);
}

void MainFrame::CreateRootLayout(wxPanel* root)
{
    auto* rootSizer = new wxBoxSizer(wxVERTICAL);

    // top panel with controls
    m_top = new wxPanel(root);
    CreateTopPanel(m_top);
    rootSizer->Add(m_top, 0, wxEXPAND | wxALL, 6);

    // board panel
    CreateBoardPanel(root);
    rootSizer->Add(m_boardPanel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);

    root->SetSizer(rootSizer);

    BindEvents();
}

void MainFrame::CreateTopPanel(wxPanel* parent)
{
    auto* topSizer = new wxBoxSizer(wxHORIZONTAL);

    // Size
    topSizer->Add(new wxStaticText(parent, wxID_ANY, "Size:"), 0,
        wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_sizeSpin = new wxSpinCtrl(parent, wxID_ANY, "5", wxDefaultPosition, wxSize(60, -1));
    m_sizeSpin->SetRange(3, 10);
    m_sizeSpin->SetValue(5);
    topSizer->Add(m_sizeSpin, 0, wxRIGHT, 12);

    // Mode
    wxString modes[] = { "Simple", "General" };
    m_modeRadio = new wxRadioBox(parent, wxID_ANY, "Game Mode", wxDefaultPosition,
        wxDefaultSize, WXSIZEOF(modes), modes, 1, wxRA_SPECIFY_ROWS);
    m_modeRadio->SetSelection(0);
    topSizer->Add(m_modeRadio, 0, wxRIGHT, 12);

    // Letter for human player
    wxString letters[] = { "S", "O" };
    m_letterRadio = new wxRadioBox(parent, wxID_ANY, "Letter", wxDefaultPosition,
        wxDefaultSize, WXSIZEOF(letters), letters, 1,
        wxRA_SPECIFY_ROWS);
    m_letterRadio->SetSelection(0);
    topSizer->Add(m_letterRadio, 0, wxRIGHT, 12);

    // New Game button
    m_newGameBtn = new wxButton(parent, wxID_ANY, "New Game");
    topSizer->Add(m_newGameBtn, 0, wxRIGHT, 12);

    // Player type checkboxes
    m_blueCPU = new wxCheckBox(parent, wxID_ANY, "Blue CPU");
    m_redCPU = new wxCheckBox(parent, wxID_ANY, "Red CPU");
    topSizer->Add(m_blueCPU, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    topSizer->Add(m_redCPU, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    // Record + Replay
    m_recordCheck = new wxCheckBox(parent, wxID_ANY, "Record game");
    topSizer->Add(m_recordCheck, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    m_replayBtn = new wxButton(parent, wxID_ANY, "Replay");
    topSizer->Add(m_replayBtn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    // Turn + scores
    m_turnLabel = new wxStaticText(parent, wxID_ANY, "Current turn: Blue");
    topSizer->Add(m_turnLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    m_scoreBlue = new wxStaticText(parent, wxID_ANY, "Blue: 0");
    m_scoreRed = new wxStaticText(parent, wxID_ANY, "Red: 0");
    topSizer->Add(m_scoreBlue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    topSizer->Add(m_scoreRed, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    parent->SetSizer(topSizer);
}

void MainFrame::CreateBoardPanel(wxPanel* parent)
{
    m_boardPanel = new BoardPanel(parent, this);
}

void MainFrame::BindEvents()
{
    m_newGameBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { StartNewGame(); });
    m_modeRadio->Bind(wxEVT_RADIOBOX, [this](wxCommandEvent&) { StartNewGame(); });
    m_sizeSpin->Bind(wxEVT_SPINCTRL, [this](wxCommandEvent&) { StartNewGame(); });

    if (m_replayBtn) m_replayBtn->Bind(wxEVT_BUTTON, &MainFrame::ReplayFromFile, this);
}

void MainFrame::StartRecording()
{
    m_recordedMoves.clear();

    if (m_recordCheck && m_recordCheck->IsChecked())
        m_isRecording = true;
    else
        m_isRecording = false;
}

void MainFrame::StartNewGame() {
    m_replayTimer.Stop(); // Stop replay
    m_isReplaying = false;

    const int size = std::clamp(m_sizeSpin->GetValue(), 3, 10);

    GameConfig cfg;
    cfg.size = size;
    cfg.mode = (m_modeRadio->GetSelection() == 0) ? GameMode::Simple
        : GameMode::General;

    if (m_blueCPU && m_blueCPU->IsChecked())
        m_blueCtrl = std::make_unique<ComputerPlayer>();
    else
        m_blueCtrl = std::make_unique<HumanPlayer>();

    if (m_redCPU && m_redCPU->IsChecked())
        m_redCtrl = std::make_unique<ComputerPlayer>();
    else
        m_redCtrl = std::make_unique<HumanPlayer>();

    m_game.reset(cfg);
    m_boardPanel->SetBoardSize(size);

    StartRecording();

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

void MainFrame::ApplyMove(int row, int col, Cell letter, bool fromReplay)
{
    if (m_game.result() != GameResult::Ongoing)
        return;

    Player current = m_game.currentPlayer();

    auto out = m_game.placeLetter(row, col, letter);
    if (!out.placed)
        return;

    if (m_isRecording && !fromReplay) {
        m_recordedMoves.push_back(RecordedMove{ row, col, letter, current });
    }

    RefreshScore();
    RefreshTurnLabel();
    m_boardPanel->Refresh();
    m_boardPanel->Update();
    if (wxTheApp) wxTheApp->Yield(true);

    if (m_game.result() != GameResult::Ongoing) {
        wxString msg = "Draw!";
        if (m_game.result() == GameResult::BlueWins) msg = "Blue wins!";
        else if (m_game.result() == GameResult::RedWins) msg = "Red wins!";

        wxMessageBox(
            msg,
            fromReplay ? "Replay finished" : "Game Over",
            wxOK | wxICON_INFORMATION,
            this);

        if (m_isRecording && !fromReplay) {
            SaveRecordingToFile();
        }
    }
}

void MainFrame::OnCellClicked(int row, int col) {
    // Ignore clicks while replaying
    if (m_isReplaying)
        return;

    if (m_game.result() != GameResult::Ongoing)
        return;

    const bool blueCPU = (m_blueCPU && m_blueCPU->IsChecked());
    const bool redCPU = (m_redCPU && m_redCPU->IsChecked());
    Player current = m_game.currentPlayer();
    if ((current == Player::Blue && blueCPU) ||
        (current == Player::Red && redCPU))
    {
        return;
    }

    const Cell letter =
        (m_letterRadio->GetSelection() == 0) ? Cell::S : Cell::O;

    ApplyMove(row, col, letter, /*fromReplay*/ false);

    AITimer();
}

void MainFrame::AITimer() {
    if (m_game.result() != GameResult::Ongoing) {
        m_aiTimer.Stop();
        return;
    }

    const bool blueTurn = (m_game.currentPlayer() == Player::Blue);
    const bool cpuTurn =
        (blueTurn && m_blueCPU && m_blueCPU->IsChecked()) ||
        (!blueTurn && m_redCPU && m_redCPU->IsChecked());

    if (cpuTurn) {
        if (!m_aiTimer.IsRunning())
            m_aiTimer.Start(1100); // delay so we can see AI moves
    }
    else {
        m_aiTimer.Stop();
    }
}

void MainFrame::OnAITick(wxTimerEvent& evt) {
    if (evt.GetId() != m_aiTimer.GetId()) {
        evt.Skip();
        return;
    }

    if (m_game.result() != GameResult::Ongoing) {
        m_aiTimer.Stop();
        return;
    }

    // ComputerPlayer::chooseMove returns std::optional<std::tuple<int,int,Cell>>
    auto moveOpt = m_computer.chooseMove(m_game.board());
    if (!moveOpt) {
        m_aiTimer.Stop();
        return;
    }

    // structured binding from tuple
    auto [row, col, letter] = *moveOpt;

    ApplyMove(row, col, letter, false);

    AITimer();
}

void MainFrame::SaveRecordingToFile()
{
    if (m_recordedMoves.empty())
        return;

    wxFileDialog saveDlg(
        this,
        "Save game recording",
        "",
        "",
        "Text files (*.txt)|*.txt|All files (*.*)|*.*",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveDlg.ShowModal() != wxID_OK)
        return;

    std::ofstream out(saveDlg.GetPath().ToStdString());
    if (!out)
        return;

    out << "SOSREC v1\n";

    const int   size = m_game.board().size();
    const char* modeStr =
        (m_game.mode() == GameMode::Simple) ? "Simple" : "General";
    out << size << " " << modeStr << "\n";

    for (const auto& mv : m_recordedMoves) {
        char L = (mv.letter == Cell::S) ? 'S' : 'O';
        char P = (mv.player == Player::Blue) ? 'B' : 'R';
        out << mv.row << " " << mv.col << " " << L << " " << P << "\n";
    }
}

void MainFrame::ReplayFromFile(wxCommandEvent& evt)
{
    (void)evt;
    m_aiTimer.Stop();
    m_replayTimer.Stop();
    m_isReplaying = false;

    wxFileDialog openDlg(
        this,
        "Open game recording",
        "",
        "",
        "Text files (*.txt)|*.txt|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openDlg.ShowModal() != wxID_OK)
        return;

    std::ifstream in(openDlg.GetPath().ToStdString());
    if (!in)
        return;

    std::string header;
    std::getline(in, header);
    if (header.rfind("SOSREC", 0) != 0) {
        wxMessageBox("Invalid recording file.", "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    int size = 3;
    std::string modeStr;
    in >> size >> modeStr;

    GameConfig cfg;
    cfg.size = size;
    cfg.mode = (modeStr == "General") ? GameMode::General : GameMode::Simple;

    m_game.reset(cfg);
    m_boardPanel->SetBoardSize(size);

    m_sizeSpin->SetValue(size);
    m_modeRadio->SetSelection((cfg.mode == GameMode::Simple) ? 0 : 1);

    m_isReplaying = true;
    m_isRecording = false;
    m_recordedMoves.clear();

    RefreshTurnLabel();
    RefreshScore();
    m_boardPanel->Refresh();

    m_replayMoves.clear();
    int  r, c;
    char L, P;
    while (in >> r >> c >> L >> P) {
        Cell   letter = (L == 'O') ? Cell::O : Cell::S;
        Player player = (P == 'R') ? Player::Red : Player::Blue;
        m_replayMoves.push_back(RecordedMove{ r, c, letter, player });
    }

    if (m_replayMoves.empty()) {
        m_isReplaying = false;
        wxMessageBox("No moves found in recording.", "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    m_replayIndex = 0;
    m_replayTimer.Start(600);
}

void MainFrame::ReplayTimer(wxTimerEvent& evt)
{
    if (evt.GetId() != m_replayTimer.GetId()) {
        evt.Skip();
        return;
    }

    if (!m_isReplaying || m_replayIndex >= m_replayMoves.size()) {
        m_replayTimer.Stop();
        m_isReplaying = false;
        return;
    }

    const auto& mv = m_replayMoves[m_replayIndex];
    ApplyMove(mv.row, mv.col, mv.letter, true);
    ++m_replayIndex;

    if (m_game.result() != GameResult::Ongoing ||
        m_replayIndex >= m_replayMoves.size()) {
        m_replayTimer.Stop();
        m_isReplaying = false;
    }
}