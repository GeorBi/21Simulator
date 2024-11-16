// main.cpp
#include "game_core.h"
#include "card_game_ai.h"
#include <iostream>
#include <string>
#include <limits>

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void displayMenu() {
    std::cout << "\nAvailable actions:\n";
    std::cout << "1. Play game\n";
    std::cout << "2. Train AI\n";
    std::cout << "3. Watch AI play\n";
    std::cout << "4. Exit\n";
    std::cout << "Choice: ";
}

void playGame() {
  
}

void trainAI() {
    CardGameAI ai;
    
    std::cout << "Starting AI training...\n";
    ai.collectTrainingData(10000); 
    
    std::cout << "Evaluating model...\n";
    int wins = 0;
    const int TEST_GAMES = 1000;
    
    for (int i = 0; i < TEST_GAMES; i++) {
        GameOperations game;
        
        while (!game.isMatchOver()) {
            if (game.isPlayer1Turn()) {
  
                int action = ai.getAction(game.getGameState(), 0.0f);
                
                if (action < 11) {
                    game.drawCard();
                } else {
                    game.useTrumpCard(action - 11);
                }
            } else {

                if (game.getGameState().getPlayer2Tally() < 17) {
                    game.drawCard();
                } else {
                    game.stand();
                }
            }
        }
        
        if (game.getGameState().getWinState() == CompactGameState::WinState::PLAYER1_WIN) {
            wins++;
        }
    }
    
    std::cout << "Win rate: " << (static_cast<float>(wins) / TEST_GAMES * 100) << "%\n";
}

void watchAIPlay() {
    CardGameAI ai;
    GameOperations game;
    
    while (!game.isMatchOver()) {
        std::cout << "\n=== Current Game State ===\n";
        game.displayGameStatus();
        game.displayVisibleCards();
        
        if (game.isPlayer1Turn()) {
            std::cout << "\nAI's turn...\n";
            int action = ai.getAction(game.getGameState(), 0.0f);
            
            if (action < 11) {
                std::cout << "AI draws a card\n";
                game.drawCard();
            } else {
                std::cout << "AI uses trump card " << (action - 11) << "\n";
                game.useTrumpCard(action - 11);
            }
        } else {
            if (game.getGameState().getPlayer2Tally() < 17) {
                std::cout << "Player 2 draws a card\n";
                game.drawCard();
            } else {
                std::cout << "Player 2 stands\n";
                game.stand();
            }
        }
        
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }
}
int main() {
    while (true) {
        displayMenu();
        
        int choice;
        std::cin >> choice;
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                playGame();
                break;
            case 2:
                trainAI();
                break;
            case 3:
                watchAIPlay();
                break;
            case 4:
                std::cout << "Thanks for playing!\n";
                return 0;
            default:
                std::cout << "Invalid choice!\n";
                break;
        }
    }
    
    return 0;
}
