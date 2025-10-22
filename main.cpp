#include "main.h"

// Tell wxWidgets to create the application
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "SOS Game", wxPoint(50, 50), wxSize(500, 500)) {

    // --- Create Controls ---
    wxString gameModes[] = { "Simple game", "General game" };
    gameModeRadio = new wxRadioBox(this, wxID_ANY, "SOS", wxDefaultPosition, wxDefaultSize, 2, gameModes, 1, wxRA_SPECIFY_COLS);

    boardSizeText = new wxTextCtrl(this, wxID_ANY, "8", wxDefaultPosition, wxSize(40, -1));

    wxString choices[] = { "S", "O" };
    bluePlayerRadio = new wxRadioBox(this, wxID_ANY, "Blue player", wxDefaultPosition, wxDefaultSize, 2, choices, 1, wxRA_SPECIFY_COLS);
    redPlayerRadio = new wxRadioBox(this, wxID_ANY, "Red player", wxDefaultPosition, wxDefaultSize, 2, choices, 1, wxRA_SPECIFY_COLS);

    SetBackgroundColour(wxColour(600, 580, 80));

    // Panel to hold the grid
    boardPanel = new wxPanel(this);

    statusText = new wxStaticText(this, wxID_ANY, "Current turn: ");
    newGameButton = new wxButton(this, wxID_ANY, "New Game");

    // --- Layout ---
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Left side (controls)
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
    leftSizer->Add(gameModeRadio, 0, wxALL, 5);

    wxBoxSizer* sizeSizer = new wxBoxSizer(wxHORIZONTAL);
    sizeSizer->Add(new wxStaticText(this, wxID_ANY, "Board size:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    sizeSizer->Add(boardSizeText, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    leftSizer->Add(sizeSizer, 0, wxALL, 5);

    leftSizer->Add(statusText, 0, wxALL, 5);
    leftSizer->Add(newGameButton, 0, wxALL, 5);

    mainSizer->Add(leftSizer, 0, wxALL, 10);

    // Right side (game)
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    rightSizer->Add(bluePlayerRadio, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    // Create the sizer for the grid
    boardSizer = new wxGridSizer(game.boardSize, game.boardSize, 0, 0);
    boardPanel->SetSizer(boardSizer);
    rightSizer->Add(boardPanel, 1, wxEXPAND | wxALL, 10);

    rightSizer->Add(redPlayerRadio, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    mainSizer->Add(rightSizer, 1, wxEXPAND | wxALL, 10);

    SetSizer(mainSizer);

    // --- Bind Events ---
    newGameButton->Bind(wxEVT_BUTTON, &MyFrame::OnNewGame, this);
    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);

    // Create the first board
    CreateBoard();
}

void MyFrame::CreateBoard() {
    // THIS IS INEFFICIENT (amateur style)
    // Destroy all old buttons
    boardPanel->DestroyChildren();

    int size = game.boardSize;
    boardSizer->SetRows(size);
    boardSizer->SetCols(size);

    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            // Give button an ID we can use to find its row/col
            int buttonId = 1000 + (r * 10) + c; // Buggy if size > 10, but ok for now
            wxButton* btn = new wxButton(boardPanel, buttonId, "", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
            btn->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
            boardSizer->Add(btn, 1, wxEXPAND | wxALL);

            // Bind this new button
            btn->Bind(wxEVT_BUTTON, &MyFrame::OnBoardClick, this);
        }
    }

    // Update status text and enable/disable radio boxes
    statusText->SetLabel("Current turn: blue");
    bluePlayerRadio->Enable(true);
    redPlayerRadio->Enable(false);

    boardPanel->Layout(); // Refresh the panel's layout
}

void MyFrame::OnNewGame(wxCommandEvent& event) {
    long size;
    boardSizeText->GetValue().ToLong(&size);

    if (size < 3 || size > MAX_BOARD_SIZE) {
        wxMessageBox("Board size must be between 3 and 10.", "Error");
        return;
    }

    int mode = gameModeRadio->GetSelection();
    game.newGame((int)size, mode);

    // Re-create the whole board
    CreateBoard();
}

void MyFrame::OnBoardClick(wxCommandEvent& event) {
    int id = event.GetId() - 1000;
    int r = id / 10; // Relies on max size 10
    int c = id % 10;

    int letter; // 1 for S, 2 for O
    if (game.getTurn() == 0) { // Blue's turn
        letter = (bluePlayerRadio->GetSelection() == 0) ? 1 : 2;
    }
    else { // Red's turn
        letter = (redPlayerRadio->GetSelection() == 0) ? 1 : 2;
    }

    if (game.makeMove(r, c, letter)) {
        // Move was successful, update the button
        wxButton* btn = (wxButton*)event.GetEventObject();
        btn->SetLabel(letter == 1 ? "S" : "O");
        btn->Enable(false); // Disable the button

        // Update status text and enable/disable radio boxes
        if (game.getTurn() == 0) { // Now Blue's turn
            statusText->SetLabel("Current turn: blue");
            bluePlayerRadio->Enable(true);
            redPlayerRadio->Enable(false);
        }
        else { // Now Red's turn
            statusText->SetLabel("Current turn: red");
            bluePlayerRadio->Enable(false);
            redPlayerRadio->Enable(true);
        }
    }
}

void MyFrame::OnClose(wxCloseEvent& event) {
    Destroy();
}