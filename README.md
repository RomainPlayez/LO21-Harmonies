# Harmonies — Implémentation C++/Qt

> Projet universitaire réalisé dans le cadre du cours **LO21 — Programmation Orientée Objet** à l'[Université de Technologie de Compiègne (UTC)](https://www.utc.fr/), semestre P26 (février – juin 2026).

## Présentation

Adaptation complète du jeu de société **Harmonies** (Libellud, 2023) en C++ avec interface graphique Qt. Le jeu se joue en mode solo ou multijoueur (1 à 4 joueurs) et propose deux modes d'affichage : une interface graphique Qt et un mode console texte.

Le joueur choisit des groupes de jetons colorés sur un plateau central et les place sur son plateau hexagonal personnel (5×5) afin de compléter des cartes animaux — chacune rapportant des points selon la configuration de jetons et de cubes formée autour de l'animal.

## Fonctionnalités

- **Mode multijoueur** (1 à 4 joueurs) et **mode solo** avec règles adaptées
- **Interface graphique Qt** (plateaux hexagonaux interactifs, cartes, animations)
- **Mode console** entièrement jouable
- **Extension Esprits** (cartes esprit avec règles spéciales)
- **Système de sauvegarde/restauration** de partie (patron Memento)
- Calcul automatique des scores multicritères en fin de partie
- Détection de motifs paysages (arbre, montagne, bâtiment, champ, eau)

## Architecture & Design Patterns

Le projet suit une architecture **MVC** :

| Couche | Rôle |
|--------|------|
| `Jeu`, `Joueur`, `PlateauCentral`, `Sac` | Modèle — logique de jeu |
| `VueConsole`, widgets Qt (`JeuWidget`, `PlateauJoueurWidget`…) | Vue |
| `ControleurPartie` | Contrôleur |

Patrons de conception implémentés :

- **Memento** — sauvegarde et restauration complète de l'état de jeu (`EtatPartie` / `ControleurPartie` / `Jeu`)
- **Strategy** — règles de score polymorphes (`RegleDeScore` et sous-classes)
- **Singleton** — gestionnaire de ressources Qt

## Stack technique

- **Langage** : C++17
- **Framework GUI** : Qt 6.11 (Widgets, Signals/Slots, QGraphicsScene)
- **Build** : CMake 3.30 + MinGW 13.1 (Windows) / Clang (macOS)
- **Versionning** : Git / GitLab (rendu) + GitHub (archive)
- **Modélisation** : UML (PlantUML)

## Structure du projet

```
harmonies/
├── include/
│   ├── core/          # Headers — logique de jeu
│   └── gui/           # Headers — widgets Qt
├── src/
│   ├── core/          # Implémentation — logique de jeu
│   └── gui/           # Implémentation — interface graphique
├── assets/            # Images (cartes animaux, jetons, plateaux…)
├── main.cpp           # Point d'entrée (choix GUI / console)
├── mainwindow.cpp/h   # Fenêtre principale Qt
├── CMakeLists.txt
└── ressources.qrc
```

## Compilation & Lancement

### Prérequis
- Qt 6.x (avec le module Widgets)
- CMake ≥ 3.16
- Compilateur C++17 (MinGW 64-bit sous Windows, Clang sous macOS)

### Avec Qt Creator
1. Ouvrir `CMakeLists.txt` dans Qt Creator
2. Sélectionner le kit **Desktop Qt 6.x MinGW 64-bit**
3. Compiler (Ctrl+B) puis exécuter (Ctrl+R)
4. Activer **"Exécuter dans un terminal"** (Projets > Exécution) pour accéder au choix console/GUI

### En ligne de commande
```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j4
./Harmonies.exe
```

## Équipe

Projet réalisé en équipe de 5 étudiants GI01 — UTC P26 :
**Bastien · Naomi · Romain · Tom · Wael**

## Licence

Projet académique — usage éducatif uniquement.  
Le jeu de société Harmonies est une propriété de [Libellud](https://www.libellud.com/).
