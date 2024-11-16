#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <bitset>
#include <algorithm>
#include <climits>
#include <array>

enum TrumpCard {
    CARD_2 = 0,
    CARD_3 = 1,
    CARD_4 = 2,
    CARD_5 = 3,
    CARD_6 = 4,
    CARD_7 = 5,
    BLESS = 6,
    BLOODSHED = 7,
    DESTROY = 8,
    DISSERVICE = 9,
    EXCHANGE = 10,
    FRIENDSHIP = 11,
    GO_FOR_17 = 12,
    GO_FOR_24 = 13,
    GO_FOR_27 = 14,
    HUSH = 15,
    PERFECT_DRAW = 16,
    REFRESH = 17,
    REINCARNATION = 18,
    REMOVE = 19,
    RETURN = 20,
    SHIELD = 21,
    SHIELD_PLUS = 22,
    TWO_UP = 23,
    ONE_UP = 24
};

class CompactGameState {
public:
    enum class WinState {
        IN_PROGRESS,
        PLAYER1_WIN,
        PLAYER2_WIN
    };

    CompactGameState(int8_t initialDistance = 7);

    class DistanceTracker {
    public:
        DistanceTracker();
        bool adjustDistance(int adjustment);
        int8_t getDistance() const;
        void setDistance(int8_t newDistance);

    private:
        int8_t distance;
        static constexpr int8_t MIN_DISTANCE = 0;
        static constexpr int8_t MAX_DISTANCE = 14;
        static constexpr int8_t START_DISTANCE = 7;
    };

    // Getters
    uint16_t getMainDeck() const;
    uint16_t getPlayer1Deck() const;
    uint16_t getPlayer2Deck() const;
    uint8_t getPlayer1Tally() const;
    uint8_t getPlayer2Tally() const;
    uint8_t getGoal() const;
    int8_t getPlayerDistance() const;
    uint8_t getCurrentRound() const;
    uint8_t getCurrentBet() const;
    uint16_t getPlayer1HiddenDeck() const;
    uint16_t getPlayer2HiddenDeck() const;
    uint8_t getPlayer1CardCount() const;
    uint8_t getPlayer2CardCount() const;
    WinState getWinState() const;
    uint16_t getPerceivedPlayer1Deck() const;
    uint16_t getPerceivedPlayer2Deck() const;
    int16_t getBetModifier() const;

    // Setters
    void setMainDeck(uint16_t deck);
    void setPlayer1Deck(uint16_t deck);
    void setPlayer2Deck(uint16_t deck);
    void setPlayer1Tally(uint8_t tally);
    void setPlayer2Tally(uint8_t tally);
    void setGoal(uint8_t newGoal);
    void setPlayerDistance(int8_t distance);
    void setCurrentRound(uint8_t round);
    void setCurrentBet(uint8_t bet);
    void setPlayer1HiddenDeck(uint16_t deck);
    void setPlayer2HiddenDeck(uint16_t deck);
    void setPlayer1CardCount(uint8_t count);
    void setPlayer2CardCount(uint8_t count);

    // State checks
    bool isPlayer1Turn() const;
    bool isPlayer1Standing() const;
    bool isPlayer2Standing() const;
    bool isPlayer1Bust() const;
    bool isPlayer2Bust() const;
    bool isGameOver() const;
    bool isMatchOver() const;
    bool hasActiveBless(bool isPlayer1) const;
    bool canAdjustBetModifier(int16_t adjustment) const;

    // State setters
    void setPlayer1Standing(bool value);
    void setPlayer2Standing(bool value);
    void setPlayer1Bust(bool value);
    void setPlayer2Bust(bool value);
    void setGameOver(bool value);
    void setMatchOver(bool value);
    void setActiveBless(bool isPlayer1, bool value);

    // Trump card operations
    uint8_t getPlayer1TrumpCardCount(uint8_t cardId) const;
    uint8_t getPlayer2TrumpCardCount(uint8_t cardId) const;
    void addPlayer1TrumpCard(uint8_t cardId);
    void addPlayer2TrumpCard(uint8_t cardId);
    void decrementPlayer1TrumpCard(uint8_t cardId);
    void decrementPlayer2TrumpCard(uint8_t cardId);

    // Game flow
    void toggleTurn();
    void processRoundResult();
    void resetWinState();
    void adjustBetModifier(int16_t adjustment);
    void resetBetModifier();

    // Trump history
    struct TrumpCardPlay {
        uint8_t cardId;
        bool isPlayer1;
    };
    void recordTrumpPlay(uint8_t cardId, bool isPlayer1);
    TrumpCardPlay getLastTrumpPlay() const;
    void clearTrumpHistory();

    DistanceTracker distanceTracker;

private:
    uint16_t mainDeck;
    uint16_t player1Deck;
    uint16_t player2Deck;
    uint8_t player1Tally;
    uint8_t player2Tally;
    uint8_t goal;
    int8_t playerDistance;
    uint8_t currentRound;
    uint8_t currentBet;
    std::array<uint8_t, 25> player1TrumpCards;
    std::array<uint8_t, 25> player2TrumpCards;
    uint16_t flags;
    uint16_t player1HiddenDeck;
    uint16_t player2HiddenDeck;
    uint8_t player1CardCount;
    uint8_t player2CardCount;
    WinState winState;

    static constexpr uint16_t PLAYER1_TURN = 0x0001;
    static constexpr uint16_t PLAYER1_STANDING = 0x0002;
    static constexpr uint16_t PLAYER2_STANDING = 0x0004;
    static constexpr uint16_t PLAYER1_BUST = 0x0008;
    static constexpr uint16_t PLAYER2_BUST = 0x0010;
    static constexpr uint16_t GAME_OVER = 0x0020;
    static constexpr uint16_t MATCH_OVER = 0x0040;

    static constexpr size_t MAX_TRUMP_HISTORY = 50;
    TrumpCardPlay trumpHistory[MAX_TRUMP_HISTORY];
    size_t trumpHistoryCount;

    uint8_t player1LastTrump;
    uint8_t player2LastTrump;
    bool player1HasBless;
    bool player2HasBless;
    int16_t betModifier;
};

class GameOperations {
public:
    GameOperations();
    void displayRoundTransitionDetails() const;
    void startNewRound();
    bool isMatchOver() const;
    bool isPlayer1Turn() const;
    void displayGameStatus() const;
    void displayVisibleCards() const;
    void displayTrumpCards() const;
    void playerDrawCard();
    void stand();
    void resetGame();
    bool isGameOver() const;
    void endGame();
    const CompactGameState& getGameState() const;
    static std::string getTrumpCardName(int cardId);
    bool drawCard();
    bool useTrumpCard(int cardId);

private:
    void displayDeck(uint16_t deck) const;
    void dealInitialTrumpCards();
    void drawTrumpCard(int player);
    void dealInitialCards();
    void drawCardForPlayer1(bool isHidden = false);
    void drawCardForPlayer2(bool isHidden = false);
    int findBestCard(int targetSum);
    void checkPlayerStatus(int player);
    void determineWinner();
    bool executeNumberedCard(int number);
    bool executeBless(bool isPlayer1);
    bool executeBloodshed(bool isPlayer1);
    bool executeDestroy(bool isPlayer1);
    bool executeDisservice(bool isPlayer1);
    bool executeExchange(bool isPlayer1);
    bool executeFriendship(bool isPlayer1);
    bool executeGoFor(int newGoal);
    bool executeHush(bool isPlayer1);
    bool executePerfectDraw(bool isPlayer1);
    bool executeRefresh(bool isPlayer1);
    bool executeReincarnation(bool isPlayer1);
    bool executeReturn(bool isPlayer1);
    bool executeShield(int amount);
    bool executeUpBet(int amount);
    void undoTrumpEffect(uint8_t cardId);

    CompactGameState gameState;
    std::random_device rd;
    std::mt19937 rng;

    static constexpr size_t TRUMP_CARD_TYPES = 25;
    static constexpr std::array<double, 25> TRUMP_CARD_WEIGHTS = {
        0.0196, 0.0196, 0.0196, 0.0196, 0.0196, 0.0196,
        0.0196, 0.049, 0.049, 0.0294, 0.049, 0.049,
        0.049, 0.049, 0.049, 0.049, 0.049, 0.049,
        0.049, 0.049, 0.049, 0.049, 0.049, 0.049
    };
};

#endif // GAME_CORE_H
