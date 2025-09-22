/* Generate move from specific rack and board */
#include <iostream>
#include <string>
#include <vector>

#include "datamanager.h"
#include "lexiconparameters.h"
#include "boardparameters.h"
#include "game.h"

using namespace Quackle;
using std::cout;
using std::endl;
using std::string;
using std::vector;

static bool ensureLexicon()
{
    // Try to load enable1.dawg (which is actually twl06.dawg)
    std::string dawg = LexiconParameters::findDictionaryFile("enable1.dawg");
    cout << "Looking for enable1.dawg: " << dawg << endl;
    if (dawg.empty()) {
        dawg = LexiconParameters::findDictionaryFile("twl06.dawg");
        cout << "Fallback to twl06.dawg: " << dawg << endl;
    }
    if (dawg.empty()) return false;
    
    QUACKLE_LEXICON_PARAMETERS->loadDawg(dawg);
    cout << "DAWG loaded: " << (QUACKLE_LEXICON_PARAMETERS->hasDawg() ? "YES" : "NO") << endl;
    return QUACKLE_LEXICON_PARAMETERS->hasDawg();
}

void printBoard(const Board &board) {
    cout << "Board state:" << endl;
    for (int r = 0; r < board.height(); ++r) {
        for (int c = 0; c < board.width(); ++c) {
            Letter letter = board.letter(r, c);
            if (letter == QUACKLE_NULL_MARK) {
                cout << ".";
            } else {
                cout << QUACKLE_ALPHABET_PARAMETERS->userVisible(letter);
            }
        }
        cout << endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <rack> [board_config]" << endl;
        cout << "  rack: letters in rack (e.g., AEIRSTZ)" << endl;
        cout << "  board_config: optional board setup (e.g., A7,8H for A at row 7 col 8, H at row 8 col 9)" << endl;
        cout << "  Example: " << argv[0] << " AEIRSTZ" << endl;
        cout << "  Example: " << argv[0] << " HELLO?? A7,8H" << endl;
        return 1;
    }

    DataManager dm;
    dm.setAppDataDirectory("data");
    dm.setBoardParameters(new EnglishBoard());

    cout << "Alphabet: " << QUACKLE_ALPHABET_PARAMETERS->alphabetName() << endl;
    cout << "Alphabet size: " << QUACKLE_ALPHABET_PARAMETERS->length() << endl;

    if (!ensureLexicon()) {
        std::cerr << "No DAWG lexicon found under data/lexica.\n";
        return 2;
    }

    // Create a simple 2-player game
    Game game;
    PlayerList players;
    players.push_back(Player(MARK_UV("P1"), Player::HumanPlayerType, 110));
    players.push_back(Player(MARK_UV("P2"), Player::HumanPlayerType, 110));
    game.setPlayers(players);
    game.addPosition();

    // Set up board
    game.currentPosition().setEmptyBoard();
    
    // Parse board configuration if provided
    if (argc > 2) {
        string boardConfig = argv[2];
        cout << "Setting up board with: " << boardConfig << endl;
        
        // Simple parsing: letter followed by row,col
        // Format: A7,8H9,10 (A at 7,8 and H at 9,10)
        for (size_t i = 0; i < boardConfig.length(); ) {
            if (i + 3 < boardConfig.length() && 
                boardConfig[i+1] >= '0' && boardConfig[i+1] <= '9' &&
                boardConfig[i+2] == ',' &&
                boardConfig[i+3] >= '0' && boardConfig[i+3] <= '9') {
                
                char letter = boardConfig[i];
                int row = boardConfig[i+1] - '0';
                int col = boardConfig[i+2] - '0';
                
                // Handle double digits
                if (i + 4 < boardConfig.length() && boardConfig[i+4] >= '0' && boardConfig[i+4] <= '9') {
                    row = row * 10 + (boardConfig[i+4] - '0');
                    i += 5;
                } else {
                    i += 4;
                }
                
                cout << "Placing " << letter << " at (" << row << "," << col << ")" << endl;
                Move move = Move::createPlaceMove(row, col, true, QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(string(1, letter))));
                game.currentPosition().underlyingBoardReference().makeMove(move);
            } else {
                i++;
            }
        }
    }

    // Set rack
    string rackStr = argv[1];
    cout << "Setting rack to: " << rackStr << endl;
    game.currentPosition().setCurrentPlayerRack(Rack(QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(rackStr))));
    
    printBoard(game.currentPosition().board());

    // Generate moves
    cout << "\nGenerating moves..." << endl;
    game.currentPosition().kibitz(10);
    const MoveList &moves = game.currentPosition().moves();
    
    cout << "\nFound " << moves.size() << " moves:" << endl;
    for (size_t i = 0; i < moves.size() && i < 10; ++i) {
        const Move &move = moves[i];
        cout << "Move " << (i+1) << ": ";
        if (move.action == Move::Place) {
            cout << "PLACE " << QUACKLE_ALPHABET_PARAMETERS->userVisible(move.tiles()) << " at (" << move.startrow << "," << move.startcol << ") ";
            cout << (move.horizontal ? "horizontal" : "vertical");
            cout << " - Score: " << move.score;
        } else if (move.action == Move::Exchange) {
            cout << "EXCHANGE " << QUACKLE_ALPHABET_PARAMETERS->userVisible(move.tiles()) << " - Score: " << move.score;
        } else if (move.action == Move::Pass) {
            cout << "PASS - Score: " << move.score;
        }
        cout << endl;
    }
    
    if (moves.empty()) {
        cout << "No valid moves found!" << endl;
        return 3;
    }
    
    // Show best move
    const Move &bestMove = moves[0];
    cout << "\n=== BEST MOVE ===" << endl;
    if (bestMove.action == Move::Place) {
        cout << "Place " << QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles()) << " at (" << bestMove.startrow << "," << bestMove.startcol << ") ";
        cout << (bestMove.horizontal ? "horizontally" : "vertically");
        cout << " for " << bestMove.score << " points" << endl;
    } else if (bestMove.action == Move::Exchange) {
        cout << "Exchange " << QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles()) << " for " << bestMove.score << " points" << endl;
    } else if (bestMove.action == Move::Pass) {
        cout << "Pass for " << bestMove.score << " points" << endl;
    }

    return 0;
}
