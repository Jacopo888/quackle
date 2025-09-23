# ⚡ Quackle Power Engine - Quick Start

## 🚀 Avvio Rapido

### 1. Compilazione (Una volta sola)
```bash
cd /home/jacopo/Progetti-github/quackle
qmake && make -j4
g++ -std=c++11 -I. -Iquackleio -o quackle_power quackle_power.cpp -Llib/release -Lquackleio/lib/release -lquackle -lquackleio
```

### 2. Uso Base
```bash
# Board vuota
./quackle_power FALREI?

# Board con lettera
./quackle_power HELLO?? A 7 7

# Test performance
./quackle_benchmark
```

## 🎯 Livelli di Potenza

### Massima Potenza (Raccomandato)
```bash
./quackle_power <rack>
# ✅ Tutte le strategie native attive
# ✅ Equity precisa e differenziata
# ✅ Mosse ottimali garantite
```

### Potenza Media
```bash
# Modifica quackle_power.cpp: "default_english" → "default"
# ✅ Strategie base attive
# ✅ Performance più veloce
# ⚠️ Precisione ridotta
```

### Velocità Massima
```bash
./simple_move <rack>
# ✅ Solo generazione base
# ✅ Velocità massima
# ❌ Nessuna valutazione strategica
```

## 📊 Interpretazione Risultati

### Output Tipico
```
=== BEST MOVE (with full strategy evaluation) ===
Place FRAgILE at (7,3) horizontally for 76 points (equity: 78.39)

=== STRATEGY STATUS ===
  - Syn2: YES      ← Sinergia lettere
  - Worths: YES    ← Valore lettere
  - VcPlace: YES   ← Posizionamento
  - Bogowin: YES   ← Probabilità vittoria
  - Superleaves: YES ← Leave evaluation
```

### Equity vs Score
- **Equity > Score**: Mossa strategica eccellente ✅
- **Equity = Score**: Mossa neutra ⚪
- **Equity < Score**: Mossa subottimale ⚠️

## 🔧 Troubleshooting Rapido

| Problema | Soluzione |
|----------|-----------|
| "Strategy candidate missing" | Usa "default_english" |
| "No DAWG lexicon found" | Verifica enable1.15.gaddag |
| Performance lente | Usa modalità potenza media |
| Mosse non ottimali | Verifica strategie attive |

## 🎮 Esempi Pratici

### Rack Facile
```bash
./quackle_power AEIRSTZ
# Risultato: ZAIRES (50 punti, equity: 50.00)
```

### Rack con Blank
```bash
./quackle_power FALREI?
# Risultato: FRAgILE (76 punti, equity: 78.39)
```

### Rack Difficile
```bash
./quackle_power QUACKLE
# Risultato: QUACK (60 punti, equity: 60.00)
```

### Board con Lettera
```bash
./quackle_power HELLO?? A 7 7
# Risultato: HOLL. (12 punti, equity: 71.00)
```

## ⚡ Comandi Rapidi

```bash
# Test completo
./quackle_benchmark

# Solo mossa migliore
./quackle_power <rack> | grep "BEST MOVE"

# Verifica strategie
./quackle_power <rack> | grep "STRATEGY STATUS"

# Performance test
time ./quackle_power FALREI?
```

## 🏆 Risultati Attesi

### Performance
- **Velocità**: 2739+ mosse/secondo
- **Precisione**: Equity differenziata
- **Accuratezza**: Mosse ottimali identificate

### Strategie Attive (Massima Potenza)
- ✅ Syn2: Sinergia lettere
- ✅ Worths: Valore lettere
- ✅ VcPlace: Posizionamento
- ✅ Bogowin: Probabilità vittoria
- ✅ Superleaves: Leave evaluation

**Pronto per l'uso!** 🚀
