/*
 *  Quackle Advanced Engine -- Motore avanzato con strategie complete
 *  Utilizza tutte le strategie native di Quackle senza workaround
 *  Copyright (C) 2024 - Basato su Quackle
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
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

class QuackleAdvancedEngine {
private:
    Game m_game;
    bool m_initialized;
    bool m_verbose;
    
public:
    QuackleAdvancedEngine() 
        : m_initialized(false)
        , m_verbose(true)
    {
    }
    
    // Inizializzazione completa del motore
    bool initialize(const string& lexicon = "enable1.15", bool verbose = true) {
        m_verbose = verbose;
        
        if (m_verbose) {
            cout << "=== QUACKLE ADVANCED ENGINE INITIALIZATION ===" << endl;
        }
        
        // 1. Inizializzazione DataManager
        DataManager* dm = DataManager::self();
        dm->setAppDataDirectory("data");
        
        // 2. Configurazione parametri
        dm->setBoardParameters(new EnglishBoard());
        dm->setAlphabetParameters(new EnglishAlphabetParameters());
        dm->setLexiconParameters(new LexiconParameters());
        dm->setStrategyParameters(new StrategyParameters());
        
        // 3. Caricamento dizionario
        if (!loadLexicon(lexicon)) {
            if (m_verbose) {
                cout << "❌ Failed to load lexicon: " << lexicon << endl;
            }
            return false;
        }
        
        // 4. Inizializzazione strategie
        if (!initializeStrategies(lexicon)) {
            if (m_verbose) {
                cout << "❌ Failed to initialize strategies" << endl;
            }
            return false;
        }
        
        // 5. Configurazione gioco
        setupGame();
        
        m_initialized = true;
        
        if (m_verbose) {
            cout << "✅ Quackle Advanced Engine initialized successfully!" << endl;
            printEngineStatus();
        }
        
        return true;
    }
    
    // Generazione mosse con strategia completa
    MoveList generateMoves(const string& rackStr, int maxMoves = 20) {
        if (!m_initialized) {
            cout << "❌ Engine not initialized!" << endl;
            return MoveList();
        }
        
        if (m_verbose) {
            cout << "\n=== GENERATING MOVES ===" << endl;
            cout << "Rack: " << rackStr << endl;
            cout << "Max moves: " << maxMoves << endl;
        }
        
        // 1. Setup posizione
        setupPosition(rackStr);
        
        // 2. Generazione mosse base
        m_game.currentPosition().kibitz(maxMoves * 2);
        MoveList moves = m_game.currentPosition().moves();
        
        if (moves.empty()) {
            if (m_verbose) {
                cout << "❌ No valid moves found!" << endl;
            }
            return moves;
        }
        
        // 3. Valutazione statica avanzata
        evaluateMovesStatically(moves);
        
        // 4. Ordinamento finale
        sortMovesByValue(moves);
        
        // 5. Limita al numero richiesto
        if (moves.size() > static_cast<size_t>(maxMoves)) {
            moves.resize(maxMoves);
        }
        
        if (m_verbose) {
            cout << "✅ Generated " << moves.size() << " moves" << endl;
        }
        
        return moves;
    }
    
    // Stampa stato del motore
    void printEngineStatus() {
        cout << "\n=== ENGINE STATUS ===" << endl;
        cout << "Initialized: " << (m_initialized ? "YES" : "NO") << endl;
        cout << "Lexicon: " << (QUACKLE_LEXICON_PARAMETERS->hasDawg() ? "DAWG" : "NO") 
             << " / " << (QUACKLE_LEXICON_PARAMETERS->hasGaddag() ? "GADDAG" : "NO") << endl;
        cout << "Strategies:" << endl;
        cout << "  - Syn2: " << (QUACKLE_STRATEGY_PARAMETERS->hasSyn2() ? "YES" : "NO") << endl;
        cout << "  - Worths: " << (QUACKLE_STRATEGY_PARAMETERS->hasWorths() ? "YES" : "NO") << endl;
        cout << "  - VcPlace: " << (QUACKLE_STRATEGY_PARAMETERS->hasVcPlace() ? "YES" : "NO") << endl;
        cout << "  - Bogowin: " << (QUACKLE_STRATEGY_PARAMETERS->hasBogowin() ? "YES" : "NO") << endl;
        cout << "  - Superleaves: " << (QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves() ? "YES" : "NO") << endl;
        cout << "Alphabet: " << QUACKLE_ALPHABET_PARAMETERS->alphabetName() << endl;
        cout << "Board: English Board" << endl;
    }
    
    // Stampa mosse generate
    void printMoves(const MoveList& moves, int maxToShow = 10) {
        cout << "\n=== GENERATED MOVES ===" << endl;
        
        int count = 0;
        for (const auto& move : moves) {
            if (count >= maxToShow) break;
            
            cout << "Move " << (count + 1) << ": ";
            if (move.action == Move::Place) {
                cout << "PLACE " << QUACKLE_ALPHABET_PARAMETERS->userVisible(move.tiles()) 
                     << " at (" << move.startrow << "," << move.startcol << ") ";
                cout << (move.horizontal ? "horizontal" : "vertical");
                cout << " - Score: " << move.score;
                
                // Mostra valutazione se disponibile
                if (move.equity != 0) {
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
    
    // Stampa board
    void printBoard() {
        const Board& board = m_game.currentPosition().board();
        cout << "\n=== BOARD STATE ===" << endl;
        
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
    
    // Verifica se il motore è inizializzato
    bool isInitialized() const {
        return m_initialized;
    }
    
    // Verifica se il gioco è finito
    bool isGameOver() const {
        return m_game.currentPosition().gameOver();
    }
    
    // Ottieni la mossa migliore
    Move getBestMove() {
        if (!m_initialized) {
            return Move::createPassMove();
        }
        
        MoveList moves = generateMoves("", 1);
        if (moves.empty()) {
            return Move::createPassMove();
        }
        
        return moves[0];
    }
    
private:
    bool loadLexicon(const string& lexicon) {
        // Prova prima GADDAG
        string gaddagFile = LexiconParameters::findDictionaryFile(lexicon + ".gaddag");
        if (!gaddagFile.empty()) {
            QUACKLE_LEXICON_PARAMETERS->loadGaddag(gaddagFile);
            if (QUACKLE_LEXICON_PARAMETERS->hasGaddag()) {
                if (m_verbose) {
                    cout << "✅ GADDAG loaded: " << gaddagFile << endl;
                }
                return true;
            }
        }
        
        // Fallback su DAWG
        string dawgFile = LexiconParameters::findDictionaryFile(lexicon + ".dawg");
        if (dawgFile.empty()) {
            dawgFile = LexiconParameters::findDictionaryFile("twl06.dawg");
        }
        
        if (!dawgFile.empty()) {
            QUACKLE_LEXICON_PARAMETERS->loadDawg(dawgFile);
            if (QUACKLE_LEXICON_PARAMETERS->hasDawg()) {
                if (m_verbose) {
                    cout << "✅ DAWG loaded: " << dawgFile << endl;
                }
                return true;
            }
        }
        
        return false;
    }
    
    bool initializeStrategies(const string& lexicon) {
        QUACKLE_STRATEGY_PARAMETERS->initialize(lexicon);
        
        // Verifica che tutte le strategie siano caricate
        bool allLoaded = QUACKLE_STRATEGY_PARAMETERS->hasSyn2() && 
                        QUACKLE_STRATEGY_PARAMETERS->hasWorths() && 
                        QUACKLE_STRATEGY_PARAMETERS->hasVcPlace() && 
                        QUACKLE_STRATEGY_PARAMETERS->hasBogowin() && 
                        QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves();
        
        if (m_verbose) {
            cout << "Strategy loading status:" << endl;
            cout << "  - Syn2: " << (QUACKLE_STRATEGY_PARAMETERS->hasSyn2() ? "YES" : "NO") << endl;
            cout << "  - Worths: " << (QUACKLE_STRATEGY_PARAMETERS->hasWorths() ? "YES" : "NO") << endl;
            cout << "  - VcPlace: " << (QUACKLE_STRATEGY_PARAMETERS->hasVcPlace() ? "YES" : "NO") << endl;
            cout << "  - Bogowin: " << (QUACKLE_STRATEGY_PARAMETERS->hasBogowin() ? "YES" : "NO") << endl;
            cout << "  - Superleaves: " << (QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves() ? "YES" : "NO") << endl;
        }
        
        return allLoaded;
    }
    
    void setupGame() {
        // Crea giocatori
        PlayerList players;
        players.push_back(Player(MARK_UV("Player1"), Player::HumanPlayerType, 110));
        players.push_back(Player(MARK_UV("Player2"), Player::HumanPlayerType, 110));
        
        m_game.setPlayers(players);
        m_game.addPosition();
        m_game.currentPosition().setEmptyBoard();
    }
    
    void setupPosition(const string& rackStr) {
        // Imposta rack
        Rack rack(QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(rackStr)));
        m_game.currentPosition().setCurrentPlayerRack(rack);
    }
    
    void evaluateMovesStatically(MoveList& moves) {
        // Usa il valutatore avanzato CatchallEvaluator
        CatchallEvaluator evaluator;
        
        for (auto& move : moves) {
            // Valuta la mossa usando tutte le strategie native
            move.equity = evaluator.equity(m_game.currentPosition(), move);
        }
    }
    
    void sortMovesByValue(MoveList& moves) {
        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            return a.equity > b.equity;
        });
    }
};

// Funzioni di utilità
void printUsage(const char* programName) {
    cout << "Usage: " << programName << " <command> [options]" << endl;
    cout << "Commands:" << endl;
    cout << "  generate <rack> [max_moves]              # Generate moves" << endl;
    cout << "  play                                    # Interactive game" << endl;
    cout << "  test                                    # Run performance tests" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  " << programName << " generate AEIRSTZ 10" << endl;
    cout << "  " << programName << " generate FALREI? 15" << endl;
    cout << "  " << programName << " play" << endl;
    cout << "  " << programName << " test" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    string command = argv[1];
    
    if (command == "generate") {
        if (argc < 3) {
            cout << "Usage: " << argv[0] << " generate <rack> [max_moves]" << endl;
            return 1;
        }
        
        string rack = argv[2];
        int maxMoves = (argc > 3) ? std::stoi(argv[3]) : 10;
        
        QuackleAdvancedEngine engine;
        if (!engine.initialize()) {
            cout << "❌ Failed to initialize engine" << endl;
            return 1;
        }
        
        engine.printBoard();
        MoveList moves = engine.generateMoves(rack, maxMoves);
        engine.printMoves(moves);
        
        if (!moves.empty()) {
            cout << "\n=== BEST MOVE ===" << endl;
            const Move& best = moves[0];
            if (best.action == Move::Place) {
                cout << "Place " << QUACKLE_ALPHABET_PARAMETERS->userVisible(best.tiles()) 
                     << " at (" << best.startrow << "," << best.startcol << ") ";
                cout << (best.horizontal ? "horizontally" : "vertically");
                cout << " for " << best.score << " points";
                if (best.equity != 0) {
                    cout << " (equity: " << std::fixed << std::setprecision(2) << best.equity << ")";
                }
                cout << endl;
            }
        }
        
    } else if (command == "play") {
        cout << "Interactive play mode not yet implemented" << endl;
        return 1;
        
    } else if (command == "test") {
        cout << "Performance test mode not yet implemented" << endl;
        return 1;
        
    } else {
        cout << "Unknown command: " << command << endl;
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}
