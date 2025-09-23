/*
 *  Quackle Power Engine - Configurazioni
 *  Diversi livelli di potenza e strategie
 *  Copyright (C) 2024 - Basato su Quackle
 */

#ifndef QUACKLE_CONFIGS_H
#define QUACKLE_CONFIGS_H

#include <string>

namespace QuackleConfigs {

// Livelli di potenza disponibili
enum PowerLevel {
    MAXIMUM_POWER = 0,    // Tutte le strategie native
    HIGH_POWER = 1,       // Strategie principali
    MEDIUM_POWER = 2,     // Strategie base
    BASIC_POWER = 3,      // Solo generazione
    SPEED_POWER = 4       // Velocità massima
};

// Configurazione per ogni livello
struct EngineConfig {
    std::string strategySet;
    std::string lexicon;
    int maxMoves;
    bool useAdvancedEvaluation;
    bool useSimulation;
    std::string description;
};

// Configurazioni predefinite
const EngineConfig CONFIGS[] = {
    // MAXIMUM_POWER
    {
        "default_english",    // Tutte le strategie native
        "enable1.15",         // GADDAG ottimizzato
        20,                   // 20 mosse
        true,                 // Valutazione avanzata
        true,                 // Simulazione
        "Massima potenza - Tutte le strategie native attive"
    },
    
    // HIGH_POWER
    {
        "default",            // Strategie principali
        "enable1.15",         // GADDAG ottimizzato
        15,                   // 15 mosse
        true,                 // Valutazione avanzata
        false,                // No simulazione
        "Alta potenza - Strategie principali, no simulazione"
    },
    
    // MEDIUM_POWER
    {
        "default",            // Strategie base
        "twl06",              // DAWG standard
        10,                   // 10 mosse
        true,                 // Valutazione base
        false,                // No simulazione
        "Potenza media - Strategie base, performance bilanciata"
    },
    
    // BASIC_POWER
    {
        "default",            // Strategie minime
        "twl06",              // DAWG standard
        5,                    // 5 mosse
        false,                // Valutazione semplice
        false,                // No simulazione
        "Potenza base - Solo generazione essenziale"
    },
    
    // SPEED_POWER
    {
        "default",            // Strategie minime
        "twl06",              // DAWG standard
        3,                    // 3 mosse
        false,                // Valutazione minima
        false,                // No simulazione
        "Velocità massima - Generazione rapida"
    }
};

// Funzioni di utilità
inline const EngineConfig& getConfig(PowerLevel level) {
    return CONFIGS[level];
}

inline std::string getConfigDescription(PowerLevel level) {
    return CONFIGS[level].description;
}

inline PowerLevel getPowerLevelFromString(const std::string& level) {
    if (level == "maximum" || level == "max") return MAXIMUM_POWER;
    if (level == "high") return HIGH_POWER;
    if (level == "medium") return MEDIUM_POWER;
    if (level == "basic") return BASIC_POWER;
    if (level == "speed") return SPEED_POWER;
    return MAXIMUM_POWER; // Default
}

// Informazioni sulle strategie per ogni livello
struct StrategyInfo {
    bool syn2;
    bool worths;
    bool vcplace;
    bool bogowin;
    bool superleaves;
    bool catchall;
};

const StrategyInfo STRATEGY_INFO[] = {
    // MAXIMUM_POWER
    { true, true, true, true, true, true },
    
    // HIGH_POWER
    { false, true, false, true, false, true },
    
    // MEDIUM_POWER
    { false, true, false, true, false, false },
    
    // BASIC_POWER
    { false, true, false, false, false, false },
    
    // SPEED_POWER
    { false, false, false, false, false, false }
};

inline const StrategyInfo& getStrategyInfo(PowerLevel level) {
    return STRATEGY_INFO[level];
}

// Stampa informazioni configurazione
inline void printConfigInfo(PowerLevel level) {
    const EngineConfig& config = getConfig(level);
    const StrategyInfo& strategies = getStrategyInfo(level);
    
    std::cout << "=== CONFIGURATION INFO ===" << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Description: " << config.description << std::endl;
    std::cout << "Strategy Set: " << config.strategySet << std::endl;
    std::cout << "Lexicon: " << config.lexicon << std::endl;
    std::cout << "Max Moves: " << config.maxMoves << std::endl;
    std::cout << "Advanced Evaluation: " << (config.useAdvancedEvaluation ? "YES" : "NO") << std::endl;
    std::cout << "Simulation: " << (config.useSimulation ? "YES" : "NO") << std::endl;
    std::cout << "\nActive Strategies:" << std::endl;
    std::cout << "  - Syn2: " << (strategies.syn2 ? "YES" : "NO") << std::endl;
    std::cout << "  - Worths: " << (strategies.worths ? "YES" : "NO") << std::endl;
    std::cout << "  - VcPlace: " << (strategies.vcplace ? "YES" : "NO") << std::endl;
    std::cout << "  - Bogowin: " << (strategies.bogowin ? "YES" : "NO") << std::endl;
    std::cout << "  - Superleaves: " << (strategies.superleaves ? "YES" : "NO") << std::endl;
    std::cout << "  - Catchall: " << (strategies.catchall ? "YES" : "NO") << std::endl;
}

} // namespace QuackleConfigs

#endif // QUACKLE_CONFIGS_H
