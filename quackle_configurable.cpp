/*
 *  Quackle Configurable Engine - Motore con livelli di potenza configurabili
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
#include "quackle_configs.h"

using namespace Quackle;
using namespace QuackleConfigs;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class QuackleConfigurableEngine {
private:
    Game m_game;
    PowerLevel m_powerLevel;
    EngineConfig m_config;
    bool m_initialized;
    bool m_verbose;
    
public:
    QuackleConfigurableEngine(PowerLevel level = MAXIMUM_POWER, bool verbose = true) 
        : m_powerLevel(level)
        , m_config(getConfig(level))
        , m_initialized(false)
        , m_verbose(verbose)
    {
    }
    
    // Inizializzazione con livello di potenza specifico
    bool initialize() {
        if (m_verbose) {
            cout << "=== QUACKLE CONFIGURABLE ENGINE ===" << endl;
            cout << "Power Level: " << m_powerLevel << endl;
            printConfigInfo(m_powerLevel);
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
        if (!loadLexicon()) {
            if (m_verbose) {
                cout << "❌ Failed to load lexicon: " << m_config.lexicon << endl;
            }
            return false;
        }
        
        // 4. Inizializzazione strategie
        if (!initializeStrategies()) {
            if (m_verbose) {
                cout << "❌ Failed to initialize strategies" << endl;
            }
            return false;
        }
        
        // 5. Configurazione gioco
        setupGame();
        
        m_initialized = true;
        
        if (m_verbose) {
            cout << "✅ Quackle Configurable Engine initialized successfully!" << endl;
        }
        
        return true;
    }
    
    // Generazione mosse con configurazione specifica
    vector<Move> generateMoves(const string& rackStr) {
        if (!m_initialized) {
            cout << "❌ Engine not initialized!" << endl;
            return vector<Move>();
        }
        
        if (m_verbose) {
            cout << "\n=== GENERATING MOVES ===" << endl;
            cout << "Rack: " << rackStr << endl;
            cout << "Power Level: " << m_powerLevel << endl;
            cout << "Max Moves: " << m_config.maxMoves << endl;
        }
        
        // 1. Setup posizione
        setupPosition(rackStr);
        
        // 2. Generazione mosse
        m_game.currentPosition().kibitz(m_config.maxMoves * 2);
        MoveList moves = m_game.currentPosition().moves();
        
        if (moves.empty()) {
            if (m_verbose) {
                cout << "❌ No valid moves found!" << endl;
            }
            return vector<Move>();
        }
        
        // 3. Valutazione (se abilitata)
        if (m_config.useAdvancedEvaluation) {
            evaluateMoves(moves);
        }
        
        // 4. Ordinamento
        sortMoves(moves);
        
        // 5. Limita al numero richiesto
        if (moves.size() > static_cast<size_t>(m_config.maxMoves)) {
            moves.resize(m_config.maxMoves);
        }
        
        // Converti in vector
        vector<Move> result;
        for (const auto& move : moves) {
            result.push_back(move);
        }
        
        if (m_verbose) {
            cout << "✅ Generated " << result.size() << " moves" << endl;
        }
        
        return result;
    }
    
    // Stampa mosse generate
    void printMoves(const vector<Move>& moves, int maxToShow = -1) {
        if (maxToShow == -1) maxToShow = m_config.maxMoves;
        
        cout << "\n=== GENERATED MOVES ===" << endl;
        cout << "Power Level: " << m_powerLevel << " - " << m_config.description << endl;
        
        int count = 0;
        for (const auto& move : moves) {
            if (count >= maxToShow) break;
            
            cout << "Move " << (count + 1) << ": ";
            if (move.action == Move::Place) {
                cout << "PLACE " << QUACKLE_ALPHABET_PARAMETERS->userVisible(move.tiles()) 
                     << " at (" << move.startrow << "," << move.startcol << ") ";
                cout << (move.horizontal ? "horizontal" : "vertical");
                cout << " - Score: " << move.score;
                
                if (m_config.useAdvancedEvaluation && move.equity != 0) {
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
    
    // Ottieni la mossa migliore
    Move getBestMove(const string& rackStr) {
        vector<Move> moves = generateMoves(rackStr);
        if (moves.empty()) {
            return Move::createPassMove();
        }
        return moves[0];
    }
    
    // Cambia livello di potenza
    void setPowerLevel(PowerLevel level) {
        m_powerLevel = level;
        m_config = getConfig(level);
        m_initialized = false; // Richiede reinizializzazione
    }
    
    // Verifica se il motore è inizializzato
    bool isInitialized() const {
        return m_initialized;
    }
    
    // Accesso al gioco per operazioni avanzate
    Game& getGame() {
        return m_game;
    }
    
private:
    bool loadLexicon() {
        // Prova prima GADDAG
        string gaddagFile = LexiconParameters::findDictionaryFile(m_config.lexicon + ".gaddag");
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
        string dawgFile = LexiconParameters::findDictionaryFile(m_config.lexicon + ".dawg");
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
    
    bool initializeStrategies() {
        try {
            QUACKLE_STRATEGY_PARAMETERS->initialize(m_config.strategySet);
        } catch (...) {
            if (m_verbose) {
                cout << "Some strategies not available, continuing with available ones..." << endl;
            }
        }
        
        // Verifica strategie attive
        const StrategyInfo& strategies = getStrategyInfo(m_powerLevel);
        bool allRequiredLoaded = true;
        
        if (strategies.syn2 && !QUACKLE_STRATEGY_PARAMETERS->hasSyn2()) allRequiredLoaded = false;
        if (strategies.worths && !QUACKLE_STRATEGY_PARAMETERS->hasWorths()) allRequiredLoaded = false;
        if (strategies.vcplace && !QUACKLE_STRATEGY_PARAMETERS->hasVcPlace()) allRequiredLoaded = false;
        if (strategies.bogowin && !QUACKLE_STRATEGY_PARAMETERS->hasBogowin()) allRequiredLoaded = false;
        if (strategies.superleaves && !QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves()) allRequiredLoaded = false;
        
        if (m_verbose) {
            cout << "Strategy loading status:" << endl;
            cout << "  - Syn2: " << (QUACKLE_STRATEGY_PARAMETERS->hasSyn2() ? "YES" : "NO") << endl;
            cout << "  - Worths: " << (QUACKLE_STRATEGY_PARAMETERS->hasWorths() ? "YES" : "NO") << endl;
            cout << "  - VcPlace: " << (QUACKLE_STRATEGY_PARAMETERS->hasVcPlace() ? "YES" : "NO") << endl;
            cout << "  - Bogowin: " << (QUACKLE_STRATEGY_PARAMETERS->hasBogowin() ? "YES" : "NO") << endl;
            cout << "  - Superleaves: " << (QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves() ? "YES" : "NO") << endl;
        }
        
        return allRequiredLoaded;
    }
    
    void setupGame() {
        PlayerList players;
        players.push_back(Player(MARK_UV("Player1"), Player::HumanPlayerType, 110));
        players.push_back(Player(MARK_UV("Player2"), Player::HumanPlayerType, 110));
        
        m_game.setPlayers(players);
        m_game.addPosition();
        m_game.currentPosition().setEmptyBoard();
    }
    
    void setupPosition(const string& rackStr) {
        Rack rack(QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(rackStr)));
        m_game.currentPosition().setCurrentPlayerRack(rack);
    }
    
    void evaluateMoves(MoveList& moves) {
        if (m_config.useAdvancedEvaluation) {
            CatchallEvaluator evaluator;
            for (auto& move : moves) {
                move.equity = evaluator.equity(m_game.currentPosition(), move);
            }
        }
    }
    
    void sortMoves(MoveList& moves) {
        if (m_config.useAdvancedEvaluation) {
            std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
                return a.equity > b.equity;
            });
        } else {
            std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
                return a.score > b.score;
            });
        }
    }
};

// Funzioni di utilità
void printUsage(const char* programName) {
    cout << "Usage: " << programName << " <power_level> <rack> [board_letter] [row] [col]" << endl;
    cout << "Power Levels:" << endl;
    cout << "  maximum, max  - Massima potenza (tutte le strategie)" << endl;
    cout << "  high          - Alta potenza (strategie principali)" << endl;
    cout << "  medium        - Potenza media (strategie base)" << endl;
    cout << "  basic         - Potenza base (solo generazione)" << endl;
    cout << "  speed         - Velocità massima (minimo)" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  " << programName << " maximum FALREI?" << endl;
    cout << "  " << programName << " high AEIRSTZ" << endl;
    cout << "  " << programName << " medium HELLO?? A 7 7" << endl;
    cout << "  " << programName << " speed QUACKLE" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    string powerLevelStr = argv[1];
    string rack = argv[2];
    
    PowerLevel level = getPowerLevelFromString(powerLevelStr);
    
    QuackleConfigurableEngine engine(level, true);
    if (!engine.initialize()) {
        cout << "❌ Failed to initialize engine" << endl;
        return 1;
    }
    
    // Setup board se specificato
    if (argc >= 6) {
        string boardLetter = argv[3];
        int row = std::stoi(argv[4]);
        int col = std::stoi(argv[5]);
        
        cout << "Placing " << boardLetter << " at (" << row << "," << col << ")" << endl;
        Move move = Move::createPlaceMove(row, col, true, QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(boardLetter)));
        engine.getGame().currentPosition().underlyingBoardReference().makeMove(move);
    }
    
    engine.printBoard();
    vector<Move> moves = engine.generateMoves(rack);
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
    
    return 0;
}
