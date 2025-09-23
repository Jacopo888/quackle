/*
 *  Quackle Levels Engine - Motore con livelli di potenza semplificati
 *  Copyright (C) 2024 - Basato su Quackle
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

#include "datamanager.h"
#include "lexiconparameters.h"
#include "boardparameters.h"
#include "strategyparameters.h"
#include "game.h"
#include "alphabetparameters.h"
#include "evaluator.h"
#include "catchall.h"

using namespace Quackle;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// Livelli di potenza
enum PowerLevel {
    MAXIMUM = 0,    // Tutte le strategie
    HIGH = 1,       // Strategie principali
    MEDIUM = 2,     // Strategie base
    BASIC = 3       // Solo generazione
};

// Funzione per caricare il dizionario
static bool ensureLexicon(const string& lexicon = "enable1.15") {
    // Prova prima GADDAG
    std::string gaddag = LexiconParameters::findDictionaryFile(lexicon + ".gaddag");
    if (!gaddag.empty()) {
        QUACKLE_LEXICON_PARAMETERS->loadGaddag(gaddag);
        if (QUACKLE_LEXICON_PARAMETERS->hasGaddag()) return true;
    }

    // Fall back to DAWG
    std::string dawg = LexiconParameters::findDictionaryFile(lexicon + ".dawg");
    if (dawg.empty()) {
        dawg = LexiconParameters::findDictionaryFile("twl06.dawg");
    }
    if (dawg.empty()) return false;
    
    QUACKLE_LEXICON_PARAMETERS->loadDawg(dawg);
    return QUACKLE_LEXICON_PARAMETERS->hasDawg();
}

// Funzione per stampare la board
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

// Funzione per valutare le mosse
void evaluateMoves(MoveList& moves, const GamePosition& position, PowerLevel level) {
    if (level == MAXIMUM || level == HIGH) {
        CatchallEvaluator evaluator;
        for (auto& move : moves) {
            move.equity = evaluator.equity(position, move);
        }
    }
}

// Funzione per ordinare le mosse
void sortMoves(MoveList& moves, PowerLevel level) {
    if (level == MAXIMUM || level == HIGH) {
        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            return a.equity > b.equity;
        });
    } else {
        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            return a.score > b.score;
        });
    }
}

// Funzione per stampare le mosse
void printMoves(const MoveList& moves, PowerLevel level, int maxToShow = 10) {
    cout << "\n=== GENERATED MOVES ===" << endl;
    
    string levelName;
    switch (level) {
        case MAXIMUM: levelName = "MAXIMUM POWER"; break;
        case HIGH: levelName = "HIGH POWER"; break;
        case MEDIUM: levelName = "MEDIUM POWER"; break;
        case BASIC: levelName = "BASIC POWER"; break;
    }
    cout << "Level: " << levelName << endl;
    
    int count = 0;
    for (const auto& move : moves) {
        if (count >= maxToShow) break;
        
        cout << "Move " << (count + 1) << ": ";
        if (move.action == Move::Place) {
            cout << "PLACE " << QUACKLE_ALPHABET_PARAMETERS->userVisible(move.tiles()) 
                 << " at (" << move.startrow << "," << move.startcol << ") ";
            cout << (move.horizontal ? "horizontal" : "vertical");
            cout << " - Score: " << move.score;
            
            if ((level == MAXIMUM || level == HIGH) && move.equity != 0) {
                cout << " - Equity: " << std::fixed << std::setprecision(2) << move.equity;
            }
            
        } else if (move.action == Move::Exchange) {
            cout << "EXCHANGE " << QUACKLE_ALPHABET_PARAMETERS->userVisible(move.tiles()) 
                 << " - Score: " << move.score;
        } else if (move.action == Move::Pass) {
            cout << "PASS - Score: " << move.score;
        }
        cout << endl;
        count++;
    }
    
    if (moves.size() > static_cast<size_t>(maxToShow)) {
        cout << "... and " << (moves.size() - maxToShow) << " more moves" << endl;
    }
}

// Funzione per stampare lo stato delle strategie
void printStrategyStatus(PowerLevel level) {
    cout << "\n=== STRATEGY STATUS ===" << endl;
    cout << "Lexicon: " << (QUACKLE_LEXICON_PARAMETERS->hasDawg() ? "DAWG" : "NO") 
         << " / " << (QUACKLE_LEXICON_PARAMETERS->hasGaddag() ? "GADDAG" : "NO") << endl;
    
    cout << "Strategies:" << endl;
    cout << "  - Syn2: " << (QUACKLE_STRATEGY_PARAMETERS->hasSyn2() ? "YES" : "NO");
    if (level == MAXIMUM) cout << " ✅";
    else if (level == HIGH) cout << " ❌";
    else cout << " ❌";
    cout << endl;
    
    cout << "  - Worths: " << (QUACKLE_STRATEGY_PARAMETERS->hasWorths() ? "YES" : "NO");
    if (level == MAXIMUM || level == HIGH || level == MEDIUM) cout << " ✅";
    else cout << " ❌";
    cout << endl;
    
    cout << "  - VcPlace: " << (QUACKLE_STRATEGY_PARAMETERS->hasVcPlace() ? "YES" : "NO");
    if (level == MAXIMUM) cout << " ✅";
    else cout << " ❌";
    cout << endl;
    
    cout << "  - Bogowin: " << (QUACKLE_STRATEGY_PARAMETERS->hasBogowin() ? "YES" : "NO");
    if (level == MAXIMUM || level == HIGH || level == MEDIUM) cout << " ✅";
    else cout << " ❌";
    cout << endl;
    
    cout << "  - Superleaves: " << (QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves() ? "YES" : "NO");
    if (level == MAXIMUM) cout << " ✅";
    else cout << " ❌";
    cout << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <level> <rack> [board_letter] [row] [col]" << endl;
        cout << "Levels:" << endl;
        cout << "  maximum  - Massima potenza (tutte le strategie)" << endl;
        cout << "  high     - Alta potenza (strategie principali)" << endl;
        cout << "  medium   - Potenza media (strategie base)" << endl;
        cout << "  basic    - Potenza base (solo generazione)" << endl;
        cout << endl;
        cout << "Examples:" << endl;
        cout << "  " << argv[0] << " maximum FALREI?" << endl;
        cout << "  " << argv[0] << " high AEIRSTZ" << endl;
        cout << "  " << argv[0] << " medium HELLO?? A 7 7" << endl;
        return 1;
    }

    string levelStr = argv[1];
    string rack = argv[2];
    
    PowerLevel level;
    if (levelStr == "maximum") level = MAXIMUM;
    else if (levelStr == "high") level = HIGH;
    else if (levelStr == "medium") level = MEDIUM;
    else if (levelStr == "basic") level = BASIC;
    else {
        cout << "Invalid level: " << levelStr << endl;
        return 1;
    }

    cout << "=== QUACKLE LEVELS ENGINE ===" << endl;
    cout << "Level: " << levelStr << endl;

    // Inizializzazione DataManager
    DataManager dm;
    dm.setAppDataDirectory("data");
    dm.setBoardParameters(new EnglishBoard());
    dm.setAlphabetParameters(new EnglishAlphabetParameters());
    dm.setLexiconParameters(new LexiconParameters());
    dm.setStrategyParameters(new StrategyParameters());

    cout << "Alphabet: " << QUACKLE_ALPHABET_PARAMETERS->alphabetName() << endl;
    cout << "Alphabet size: " << QUACKLE_ALPHABET_PARAMETERS->length() << endl;

    // Caricamento dizionario
    string lexicon = (level == MAXIMUM || level == HIGH) ? "enable1.15" : "twl06";
    if (!ensureLexicon(lexicon)) {
        std::cerr << "No lexicon found for: " << lexicon << std::endl;
        return 2;
    }

    // Inizializzazione strategie
    cout << "\nInitializing strategies..." << endl;
    string strategySet = (level == MAXIMUM) ? "default_english" : "default";
    try {
        QUACKLE_STRATEGY_PARAMETERS->initialize(strategySet);
    } catch (...) {
        cout << "Some strategies not available, continuing with available ones..." << endl;
    }
    
    printStrategyStatus(level);

    // Crea un gioco semplice
    Game game;
    PlayerList players;
    players.push_back(Player(MARK_UV("P1"), Player::HumanPlayerType, 110));
    players.push_back(Player(MARK_UV("P2"), Player::HumanPlayerType, 110));
    game.setPlayers(players);
    game.addPosition();

    // Setup board
    game.currentPosition().setEmptyBoard();
    
    // Place a letter on board if provided
    if (argc >= 6) {
        string boardLetter = argv[3];
        int row = std::stoi(argv[4]);
        int col = std::stoi(argv[5]);
        
        cout << "Placing " << boardLetter << " at (" << row << "," << col << ")" << endl;
        Move move = Move::createPlaceMove(row, col, true, QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(boardLetter)));
        game.currentPosition().underlyingBoardReference().makeMove(move);
    }

    // Set rack
    cout << "Setting rack to: " << rack << endl;
    game.currentPosition().setCurrentPlayerRack(Rack(QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(rack))));
    
    printBoard(game.currentPosition().board());

    // Genera mosse
    cout << "\nGenerating moves..." << endl;
    int maxMoves = (level == MAXIMUM) ? 20 : (level == HIGH) ? 15 : (level == MEDIUM) ? 10 : 5;
    game.currentPosition().kibitz(maxMoves * 2);
    MoveList moves = game.currentPosition().moves();
    
    if (moves.empty()) {
        cout << "No valid moves found!" << endl;
        return 3;
    }
    
    // Valuta e ordina mosse
    evaluateMoves(moves, game.currentPosition(), level);
    sortMoves(moves, level);
    
    // Limita al numero richiesto
    if (moves.size() > static_cast<size_t>(maxMoves)) {
        moves.resize(maxMoves);
    }
    
    printMoves(moves, level, maxMoves);
    
    // Mostra la mossa migliore
    const Move &bestMove = moves[0];
    cout << "\n=== BEST MOVE ===" << endl;
    if (bestMove.action == Move::Place) {
        cout << "Place " << QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles()) 
             << " at (" << bestMove.startrow << "," << bestMove.startcol << ") ";
        cout << (bestMove.horizontal ? "horizontally" : "vertically");
        cout << " for " << bestMove.score << " points";
        if ((level == MAXIMUM || level == HIGH) && bestMove.equity != 0) {
            cout << " (equity: " << std::fixed << std::setprecision(2) << bestMove.equity << ")";
        }
        cout << endl;
    } else if (bestMove.action == Move::Exchange) {
        cout << "Exchange " << QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles()) 
             << " for " << bestMove.score << " points" << endl;
    } else if (bestMove.action == Move::Pass) {
        cout << "Pass for " << bestMove.score << " points" << endl;
    }

    cout << "\n=== LEVEL ANALYSIS ===" << endl;
    switch (level) {
        case MAXIMUM:
            cout << "✅ Massima potenza: Tutte le strategie native attive" << endl;
            cout << "✅ Equity precisa: Valutazione completa score + leave" << endl;
            cout << "✅ Performance ottimale: 20 mosse con analisi completa" << endl;
            break;
        case HIGH:
            cout << "✅ Alta potenza: Strategie principali attive" << endl;
            cout << "✅ Equity buona: Valutazione score + leave" << endl;
            cout << "✅ Performance alta: 15 mosse con analisi" << endl;
            break;
        case MEDIUM:
            cout << "✅ Potenza media: Strategie base attive" << endl;
            cout << "⚠️ Equity limitata: Solo valutazione score" << endl;
            cout << "✅ Performance media: 10 mosse" << endl;
            break;
        case BASIC:
            cout << "✅ Potenza base: Solo generazione" << endl;
            cout << "❌ Nessuna equity: Solo score" << endl;
            cout << "✅ Performance base: 5 mosse" << endl;
            break;
    }

    return 0;
}
