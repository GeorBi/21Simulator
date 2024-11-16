#include "game_core.h"

CompactGameState::CompactGameState(int8_t initialDistance) :
    mainDeck(0x07FF),
    player1Deck(0),
    player2Deck(0),
    player1HiddenDeck(0),
    player2HiddenDeck(0),
    player1CardCount(0),
    player2CardCount(0),
    player1Tally(0),
    player2Tally(0),
    goal(21),
    playerDistance(initialDistance),
    currentRound(1),
    currentBet(1),
    trumpHistoryCount(0),
    player1LastTrump(255),
    player2LastTrump(255),
    player1HasBless(false),
    player2HasBless(false),
    betModifier(0x0000),
    flags(PLAYER1_TURN),
    winState(WinState::IN_PROGRESS) {
    player1TrumpCards.fill(0);
    player2TrumpCards.fill(0);
}

CompactGameState::DistanceTracker::DistanceTracker() : distance(START_DISTANCE) {
    std::cout << "DistanceTracker initialized with distance: " << static_cast<int>(distance) << "\n";
}

bool CompactGameState::DistanceTracker::adjustDistance(int adjustment) {
    std::cout << "DistanceTracker adjusting distance from " << static_cast<int>(distance);
    int newDistance = distance + adjustment;
    if (newDistance <= MIN_DISTANCE) {
        distance = MIN_DISTANCE;
        std::cout << " to " << static_cast<int>(distance) << " (MIN)\n";
        return true;
    } else if (newDistance >= MAX_DISTANCE) {
        distance = MAX_DISTANCE;
        std::cout << " to " << static_cast<int>(distance) << " (MAX)\n";
        return true;
    }
    distance = newDistance;
    std::cout << " to " << static_cast<int>(distance) << "\n";
    return false;
}

int8_t CompactGameState::DistanceTracker::getDistance() const { 
    return distance; 
}

void CompactGameState::DistanceTracker::setDistance(int8_t newDistance) {
    std::cout << "DistanceTracker setting distance from " << static_cast<int>(distance) 
              << " to " << static_cast<int>(newDistance) << "\n";
    distance = newDistance;
}


uint16_t CompactGameState::getMainDeck() const { return mainDeck; }
uint16_t CompactGameState::getPlayer1Deck() const { return player1Deck; }
uint16_t CompactGameState::getPlayer2Deck() const { return player2Deck; }
uint8_t CompactGameState::getPlayer1Tally() const { return player1Tally; }
uint8_t CompactGameState::getPlayer2Tally() const { return player2Tally; }
uint8_t CompactGameState::getGoal() const { return goal; }
int8_t CompactGameState::getPlayerDistance() const { return playerDistance; }
uint8_t CompactGameState::getCurrentRound() const { return currentRound; }
uint8_t CompactGameState::getCurrentBet() const { return currentBet; }
uint16_t CompactGameState::getPlayer1HiddenDeck() const { return player1HiddenDeck; }
uint16_t CompactGameState::getPlayer2HiddenDeck() const { return player2HiddenDeck; }
uint8_t CompactGameState::getPlayer1CardCount() const { return player1CardCount; }
uint8_t CompactGameState::getPlayer2CardCount() const { return player2CardCount; }
CompactGameState::WinState CompactGameState::getWinState() const { return winState; }


void CompactGameState::setMainDeck(uint16_t deck) { mainDeck = deck; }
void CompactGameState::setPlayer1Deck(uint16_t deck) { player1Deck = deck; }
void CompactGameState::setPlayer2Deck(uint16_t deck) { player2Deck = deck; }
void CompactGameState::setPlayer1Tally(uint8_t tally) { player1Tally = tally; }
void CompactGameState::setPlayer2Tally(uint8_t tally) { player2Tally = tally; }
void CompactGameState::setGoal(uint8_t newGoal) { goal = newGoal; }
void CompactGameState::setPlayerDistance(int8_t distance) { playerDistance = distance; }
void CompactGameState::setCurrentRound(uint8_t round) { currentRound = round; }
void CompactGameState::setCurrentBet(uint8_t bet) { currentBet = bet; }
void CompactGameState::setPlayer1HiddenDeck(uint16_t deck) { player1HiddenDeck = deck; }
void CompactGameState::setPlayer2HiddenDeck(uint16_t deck) { player2HiddenDeck = deck; }
void CompactGameState::setPlayer1CardCount(uint8_t count) { player1CardCount = count; }
void CompactGameState::setPlayer2CardCount(uint8_t count) { player2CardCount = count; }


bool CompactGameState::isPlayer1Turn() const { return flags & PLAYER1_TURN; }
bool CompactGameState::isPlayer1Standing() const { return flags & PLAYER1_STANDING; }
bool CompactGameState::isPlayer2Standing() const { return flags & PLAYER2_STANDING; }
bool CompactGameState::isPlayer1Bust() const { return flags & PLAYER1_BUST; }
bool CompactGameState::isPlayer2Bust() const { return flags & PLAYER2_BUST; }
bool CompactGameState::isGameOver() const { return flags & GAME_OVER; }
bool CompactGameState::isMatchOver() const { return flags & MATCH_OVER; }
bool CompactGameState::hasActiveBless(bool isPlayer1) const { return isPlayer1 ? player1HasBless : player2HasBless; }
bool CompactGameState::canAdjustBetModifier(int16_t adjustment) const { return (betModifier + adjustment) >= 0; }


void CompactGameState::setPlayer1Standing(bool value) {
    if (value) flags |= PLAYER1_STANDING;
    else flags &= ~PLAYER1_STANDING;
}

void CompactGameState::setPlayer2Standing(bool value) {
    if (value) flags |= PLAYER2_STANDING;
    else flags &= ~PLAYER2_STANDING;
}

void CompactGameState::setPlayer1Bust(bool value) {
    if (value) flags |= PLAYER1_BUST;
    else flags &= ~PLAYER1_BUST;
}

void CompactGameState::setPlayer2Bust(bool value) {
    if (value) flags |= PLAYER2_BUST;
    else flags &= ~PLAYER2_BUST;
}

void CompactGameState::setGameOver(bool value) {
    if (value) flags |= GAME_OVER;
    else flags &= ~GAME_OVER;
}

void CompactGameState::setMatchOver(bool value) {
    if (value) flags |= MATCH_OVER;
    else flags &= ~MATCH_OVER;
}

void CompactGameState::setActiveBless(bool isPlayer1, bool value) {
    if (isPlayer1) player1HasBless = value;
    else player2HasBless = value;
}


uint8_t CompactGameState::getPlayer1TrumpCardCount(uint8_t cardId) const {
    return player1TrumpCards[cardId];
}

uint8_t CompactGameState::getPlayer2TrumpCardCount(uint8_t cardId) const {
    return player2TrumpCards[cardId];
}

void CompactGameState::addPlayer1TrumpCard(uint8_t cardId) {
    if (cardId < 25) player1TrumpCards[cardId]++;
}

void CompactGameState::addPlayer2TrumpCard(uint8_t cardId) {
    if (cardId < 25) player2TrumpCards[cardId]++;
}

void CompactGameState::decrementPlayer1TrumpCard(uint8_t cardId) {
    if (cardId < 25 && player1TrumpCards[cardId] > 0)
        player1TrumpCards[cardId]--;
}

void CompactGameState::decrementPlayer2TrumpCard(uint8_t cardId) {
    if (cardId < 25 && player2TrumpCards[cardId] > 0)
        player2TrumpCards[cardId]--;
}

void CompactGameState::toggleTurn() {
    flags ^= PLAYER1_TURN;
}

void CompactGameState::processRoundResult() {
    int p1Score = getPlayer1Tally();
    int p2Score = getPlayer2Tally();
    bool p1Bust = isPlayer1Bust();
    bool p2Bust = isPlayer2Bust();

    int distanceAdjustment = 0;

    if (p1Bust && p2Bust) {
        distanceAdjustment = (p1Score < p2Score) ? -1 : 1;
    } else if (p1Bust) {
        distanceAdjustment = 1;
    } else if (p2Bust) {
        distanceAdjustment = -1;
    } else if (p1Score > p2Score && p1Score <= goal) {
        distanceAdjustment = -1;
    } else if (p2Score > p1Score && p2Score <= goal) {
        distanceAdjustment = 1;
    }

    int totalBet = getCurrentBet() + getBetModifier();
    distanceAdjustment *= totalBet;

    playerDistance = std::clamp(playerDistance + distanceAdjustment, 0, 14);
    distanceTracker.setDistance(playerDistance);

    if (playerDistance <= 0) {
        winState = WinState::PLAYER1_WIN;
        setMatchOver(true);
    } else if (playerDistance >= 14) {
        winState = WinState::PLAYER2_WIN;
        setMatchOver(true);
    }
}

void CompactGameState::resetWinState() {
    winState = WinState::IN_PROGRESS;
}

void CompactGameState::adjustBetModifier(int16_t adjustment) {
    betModifier = std::max(int16_t(0), static_cast<int16_t>((betModifier) + adjustment));
}

void CompactGameState::resetBetModifier() {
    betModifier = 0;
}


void CompactGameState::recordTrumpPlay(uint8_t cardId, bool isPlayer1) {
    if (trumpHistoryCount < MAX_TRUMP_HISTORY) {
        trumpHistory[trumpHistoryCount++] = {cardId, isPlayer1};
        if (isPlayer1) {
            player1LastTrump = cardId;
        } else {
            player2LastTrump = cardId;
        }
    }
}

CompactGameState::TrumpCardPlay CompactGameState::getLastTrumpPlay() const {
    return trumpHistoryCount > 0 ? trumpHistory[trumpHistoryCount - 1] : TrumpCardPlay{255, false};
}

void CompactGameState::clearTrumpHistory() {
    trumpHistoryCount = 0;
    player1LastTrump = 255;
    player2LastTrump = 255;
    player1HasBless = false;
    player2HasBless = false;
    betModifier = 0;
}

uint16_t CompactGameState::getPerceivedPlayer1Deck() const {
    return player1Deck & ~player1HiddenDeck;
}

uint16_t CompactGameState::getPerceivedPlayer2Deck() const {
    return player2Deck & ~player2HiddenDeck;
}

int16_t CompactGameState::getBetModifier() const {
    return betModifier;
}


GameOperations::GameOperations() : rng(rd()) {
    resetGame();
}

std::string GameOperations::getTrumpCardName(int cardId) {
    static const std::array<std::string, 25> TRUMP_NAMES = {
        "Card 2", "Card 3", "Card 4", "Card 5", "Card 6", "Card 7",
        "Bless", "Bloodshed", "Destroy", "Disservice", "Exchange",
        "Friendship", "Go For 17", "Go For 24", "Go For 27", "Hush",
        "Perfect Draw", "Refresh", "Reincarnation", "Remove", "Return",
        "Shield", "Shield+", "Two-up", "One-up"
    };
    return TRUMP_NAMES[cardId];
}

void GameOperations::displayRoundTransitionDetails() const {
    std::cout << "\n=== Round Transition Details ===\n";
    std::cout << "Current Round: " << static_cast<int>(gameState.getCurrentRound()) << "\n";
    std::cout << "Base Bet: " << static_cast<int>(gameState.getCurrentBet()) << "\n";
    std::cout << "Current Distance: " << static_cast<int>(gameState.getPlayerDistance()) << "\n";
    std::cout << "Distance to Win (P1): " << static_cast<int>(gameState.getPlayerDistance()) << "\n";
    std::cout << "Distance to Win (P2): " << (14 - static_cast<int>(gameState.getPlayerDistance())) << "\n";
    std::cout << "============================\n";
}

void GameOperations::startNewRound() {
    if (!gameState.isMatchOver()) {
        std::cout << "\n=== Starting New Round ===\n";
        int currentDistance = gameState.getPlayerDistance();
        int nextRound = gameState.getCurrentRound() + 1;
        gameState = CompactGameState(currentDistance);
        gameState.setCurrentRound(nextRound);
        gameState.setCurrentBet(nextRound);
        dealInitialCards();
        dealInitialTrumpCards();
        std::cout << "=== New Round Started ===\n";
        std::cout << "Base bet: " << gameState.getCurrentBet() << "\n";
        std::cout << "Distance: " << gameState.getPlayerDistance() << "\n\n";
    }
}

bool GameOperations::isMatchOver() const {
    return gameState.isMatchOver();
}

bool GameOperations::isPlayer1Turn() const {
    return gameState.isPlayer1Turn();
}

void GameOperations::displayGameStatus() const {
    std::cout << "Round: " << static_cast<int>(gameState.getCurrentRound()) << "\n";
    std::cout << "Distance to death: " << static_cast<int>(gameState.getPlayerDistance()) << "\n";
    std::cout << "Goal: " << static_cast<int>(gameState.getGoal()) << "\n";
    std::cout << "Player 1 Tally: " << static_cast<int>(gameState.getPlayer1Tally()) 
              << (gameState.isPlayer1Bust() ? " (BUST)" : "") << "\n";
    std::cout << "Player 2 Tally: " << static_cast<int>(gameState.getPlayer2Tally()) 
              << (gameState.isPlayer2Bust() ? " (BUST)" : "") << "\n";
    std::cout << "Current Bet: " << (static_cast<int>(gameState.getCurrentBet()) + 
              gameState.getBetModifier()) << "\n";
}

void GameOperations::displayVisibleCards() const {
    std::cout << "Player 1 visible cards: ";
    displayDeck(gameState.getPerceivedPlayer1Deck());
    std::cout << "\nPlayer 2 visible cards: ";
    displayDeck(gameState.getPerceivedPlayer2Deck());
    std::cout << "\n";
}

void GameOperations::displayDeck(uint16_t deck) const {
    for (int i = 0; i < 11; ++i) {
        if (deck & (1 << i)) {
            std::cout << (i + 1) << " ";
        }
    }
}

void GameOperations::displayTrumpCards() const {
    bool isP1Turn = gameState.isPlayer1Turn();
    std::cout << "Available Trump Cards:\n";
    for (int i = 0; i < TRUMP_CARD_TYPES; ++i) {
        int count = isP1Turn ? 
            gameState.getPlayer1TrumpCardCount(i) : 
            gameState.getPlayer2TrumpCardCount(i);
        if (count > 0) {
            std::cout << i << ": " << getTrumpCardName(i) << " (x" << count << ")\n";
        }
    }
}

void GameOperations::dealInitialTrumpCards() {
    for (int i = 0; i < 2; i++) {
        drawTrumpCard(1);
        drawTrumpCard(2);
    }
}

void GameOperations::drawTrumpCard(int player) {
    std::discrete_distribution<> dist(TRUMP_CARD_WEIGHTS.begin(), TRUMP_CARD_WEIGHTS.end());
    int selectedCard = dist(rng);
    
    if (player == 1) {
        gameState.addPlayer1TrumpCard(selectedCard);
    } else {
        gameState.addPlayer2TrumpCard(selectedCard);
    }
}

void GameOperations::dealInitialCards() {
    drawCardForPlayer1(true);
    drawCardForPlayer2(true);
    drawCardForPlayer1();
    drawCardForPlayer2();
}

void GameOperations::drawCardForPlayer1(bool isHidden) {
    uint16_t availableCards = gameState.getMainDeck();
    if (availableCards == 0) return;

    std::vector<int> available;
    for (int i = 0; i < 11; ++i) {
        if (availableCards & (1 << i)) {
            available.push_back(i + 1);
        }
    }

    if (!available.empty()) {
        std::uniform_int_distribution<> dist(0, available.size() - 1);
        int selectedIdx = dist(rng);
        int cardValue = available[selectedIdx];
        uint16_t cardBit = 1 << (cardValue - 1);
        
        gameState.setMainDeck(availableCards & ~cardBit);
        gameState.setPlayer1Deck(gameState.getPlayer1Deck() | cardBit);
        if (isHidden) {
            gameState.setPlayer1HiddenDeck(gameState.getPlayer1HiddenDeck() | cardBit);
        }
        gameState.setPlayer1CardCount(gameState.getPlayer1CardCount() + 1);
        gameState.setPlayer1Tally(gameState.getPlayer1Tally() + cardValue);
        
        checkPlayerStatus(1);
    }
}

void GameOperations::drawCardForPlayer2(bool isHidden) {
    uint16_t availableCards = gameState.getMainDeck();
    if (availableCards == 0) return;

    std::vector<int> available;
    for (int i = 0; i < 11; ++i) {
        if (availableCards & (1 << i)) {
            available.push_back(i + 1);
        }
    }

    if (!available.empty()) {
        std::uniform_int_distribution<> dist(0, available.size() - 1);
        int selectedIdx = dist(rng);
        int cardValue = available[selectedIdx];
        uint16_t cardBit = 1 << (cardValue - 1);
        
        gameState.setMainDeck(availableCards & ~cardBit);
        gameState.setPlayer2Deck(gameState.getPlayer2Deck() | cardBit);
        if (isHidden) {
            gameState.setPlayer2HiddenDeck(gameState.getPlayer2HiddenDeck() | cardBit);
        }
        gameState.setPlayer2CardCount(gameState.getPlayer2CardCount() + 1);
        gameState.setPlayer2Tally(gameState.getPlayer2Tally() + cardValue);
        
        checkPlayerStatus(2);
    }
}

int GameOperations::findBestCard(int targetSum) {
    uint16_t availableCards = gameState.getMainDeck();
    int bestCard = -1;
    int currentTally = gameState.isPlayer1Turn() ? 
        gameState.getPlayer1Tally() : gameState.getPlayer2Tally();
    int bestDistance = INT_MAX;

    for (int i = 1; i <= 11; ++i) {
        if (availableCards & (1 << (i - 1))) {
            int newSum = currentTally + i;
            int distance = targetSum - newSum;
            if (distance >= 0 && distance < bestDistance) {
                bestDistance = distance;
                bestCard = i;
            }
        }
    }
    return bestCard;
}

void GameOperations::checkPlayerStatus(int player) {
    if (player == 1) {
        if (gameState.getPlayer1Tally() > gameState.getGoal() && 
            !gameState.hasActiveBless(true)) {
            gameState.setPlayer1Bust(true);
        }
    } else {
        if (gameState.getPlayer2Tally() > gameState.getGoal() && 
            !gameState.hasActiveBless(false)) {
            gameState.setPlayer2Bust(true);
        }
    }

    if (gameState.isPlayer1Standing() && gameState.isPlayer2Standing()) {
        determineWinner();
    }
}

void GameOperations::determineWinner() {
    int p1Tally = gameState.getPlayer1Tally();
    int p2Tally = gameState.getPlayer2Tally();
    bool p1Bust = gameState.isPlayer1Bust();
    bool p2Bust = gameState.isPlayer2Bust();

    if (p1Bust && p2Bust) {
        if (p1Tally < p2Tally) {
            std::cout << "Both players bust! Player 1 wins with lower score!\n";
        } else if (p2Tally < p1Tally) {
            std::cout << "Both players bust! Player 2 wins with lower score!\n";
        } else {
            std::cout << "Both players bust with equal scores! It's a tie!\n";
        }
    } else if (p1Bust) {
        std::cout << "Player 2 wins! Player 1 bust!\n";
    } else if (p2Bust) {
        std::cout << "Player 1 wins! Player 2 bust!\n";
    } else if (p1Tally > p2Tally) {
        std::cout << "Player 1 wins with score " << p1Tally << "!\n";
    } else if (p2Tally > p1Tally) {
        std::cout << "Player 2 wins with score " << p2Tally << "!\n";
    } else {
        std::cout << "It's a tie with score " << p1Tally << "!\n";
    }

    gameState.processRoundResult();
    std::cout << "\nNew distance to death: " << gameState.getPlayerDistance() << "\n";
    gameState.setGameOver(true);
}

void GameOperations::playerDrawCard() {
    drawCard();
}

void GameOperations::stand() {
    if (gameState.isPlayer1Turn()) {
        gameState.setPlayer1Standing(true);
    } else {
        gameState.setPlayer2Standing(true);
    }

    if (gameState.isPlayer1Standing() && gameState.isPlayer2Standing()) {
        determineWinner();
    } else {
        gameState.toggleTurn();
    }
}

void GameOperations::resetGame() {
    gameState = CompactGameState();
    gameState.setMainDeck(2047);
    dealInitialCards();
    dealInitialTrumpCards();
}

bool GameOperations::isGameOver() const {
    return gameState.isGameOver();
}

void GameOperations::endGame() {
    gameState.setGameOver(true);
}

const CompactGameState& GameOperations::getGameState() const {
    return gameState;
}

bool GameOperations::drawCard() {
    if (gameState.isGameOver()) return false;

    if (gameState.isPlayer1Turn() && gameState.isPlayer1Bust()) {
        std::cout << "Cannot draw when bust! Must stand or use trump cards.\n";
        return false;
    }
    if (!gameState.isPlayer1Turn() && gameState.isPlayer2Bust()) {
        std::cout << "Cannot draw when bust! Must stand or use trump cards.\n";
        return false;
    }

    if (gameState.isPlayer1Turn()) {
        drawCardForPlayer1();
    } else {
        drawCardForPlayer2();
    }

    gameState.toggleTurn();
    return true;
}

bool GameOperations::executeNumberedCard(int number) {
    uint16_t mainDeck = gameState.getMainDeck();
    uint16_t cardBit = 1 << (number - 1);
    
    if (!(mainDeck & cardBit)) {
        std::cout << "Card " << number << " is not available in the deck.\n";
        return true;
    }
    
    if (gameState.isPlayer1Turn()) {
        drawCardForPlayer1();
    } else {
        drawCardForPlayer2();
    }
    return true;
}

bool GameOperations::executeBless(bool isPlayer1) {
    gameState.setActiveBless(isPlayer1, true);
    std::cout << "Player " << (isPlayer1 ? "1" : "2") << " is now blessed!\n";
    return true;
}

bool GameOperations::executeBloodshed(bool isPlayer1) {
    drawTrumpCard(isPlayer1 ? 1 : 2);
    gameState.adjustBetModifier(1);
    std::cout << "Bet increased by 1 and drew a trump card!\n";
    return true;
}

bool GameOperations::executeDestroy(bool isPlayer1) {
    auto lastTrump = gameState.getLastTrumpPlay();
    if (lastTrump.cardId == 255 || lastTrump.isPlayer1 == isPlayer1) {
        std::cout << "No valid trump card to destroy!\n";
        return true;
    }
    
    undoTrumpEffect(lastTrump.cardId);
    return true;
}

bool GameOperations::executeDisservice(bool isPlayer1) {
    uint16_t opponentDeck = isPlayer1 ? gameState.getPlayer2Deck() : gameState.getPlayer1Deck();
    int opponentTally = isPlayer1 ? gameState.getPlayer2Tally() : gameState.getPlayer1Tally();
    
    for (int i = 10; i >= 0; --i) {
        if (opponentDeck & (1 << i)) {
            opponentDeck &= ~(1 << i);
            opponentTally -= (i + 1);
            
            if (isPlayer1) {
                gameState.setPlayer2Deck(opponentDeck);
                gameState.setPlayer2Tally(opponentTally);
            } else {
                gameState.setPlayer1Deck(opponentDeck);
                gameState.setPlayer1Tally(opponentTally);
            }
            
            std::cout << "Removed opponent's last card!\n";
            return true;
        }
    }
    return true;
}

bool GameOperations::executeExchange(bool isPlayer1) {
    uint16_t playerDeck = isPlayer1 ? gameState.getPlayer1Deck() : gameState.getPlayer2Deck();
    uint16_t opponentDeck = isPlayer1 ? gameState.getPlayer2Deck() : gameState.getPlayer1Deck();
    int playerTally = isPlayer1 ? gameState.getPlayer1Tally() : gameState.getPlayer2Tally();
    int opponentTally = isPlayer1 ? gameState.getPlayer2Tally() : gameState.getPlayer1Tally();
    
    int playerLastCard = -1, opponentLastCard = -1;
    int playerLastBit = -1, opponentLastBit = -1;

    for (int i = 10; i >= 0; --i) {
        if (playerLastCard == -1 && (playerDeck & (1 << i))) {
            playerLastCard = i + 1;
            playerLastBit = i;
        }
        if (opponentLastCard == -1 && (opponentDeck & (1 << i))) {
            opponentLastCard = i + 1;
            opponentLastBit = i;
        }
        if (playerLastCard != -1 && opponentLastCard != -1) break;
    }

    if (playerLastCard != -1 && opponentLastCard != -1) {
        playerDeck &= ~(1 << playerLastBit);
        opponentDeck &= ~(1 << opponentLastBit);
        playerDeck |= (1 << opponentLastBit);
        opponentDeck |= (1 << playerLastBit);

        playerTally = playerTally - playerLastCard + opponentLastCard;
        opponentTally = opponentTally - opponentLastCard + playerLastCard;

        if (isPlayer1) {
            gameState.setPlayer1Deck(playerDeck);
            gameState.setPlayer2Deck(opponentDeck);
            gameState.setPlayer1Tally(playerTally);
            gameState.setPlayer2Tally(opponentTally);
        } else {
            gameState.setPlayer2Deck(playerDeck);
            gameState.setPlayer1Deck(opponentDeck);
            gameState.setPlayer2Tally(playerTally);
            gameState.setPlayer1Tally(opponentTally);
        }
        std::cout << "Exchanged last cards!\n";
    }
    return true;
}

bool GameOperations::executeFriendship(bool isPlayer1) {
    drawTrumpCard(1);
    drawTrumpCard(1);
    drawTrumpCard(2);
    drawTrumpCard(2);
    return true;
}

bool GameOperations::executeGoFor(int newGoal) {
    gameState.setGoal(newGoal);
    std::cout << "Goal changed to " << newGoal << "!\n";
    return true;
}

bool GameOperations::executeHush(bool isPlayer1) {
    if (isPlayer1) {
        drawCardForPlayer1(true);
    } else {
        drawCardForPlayer2(true);
    }
    return true;
}

bool GameOperations::executePerfectDraw(bool isPlayer1) {
    int currentTally = isPlayer1 ? gameState.getPlayer1Tally() : gameState.getPlayer2Tally();
    int targetGoal = gameState.getGoal();
    int bestCard = findBestCard(targetGoal);

    if (bestCard != -1) {
        uint16_t cardBit = 1 << (bestCard - 1);
        if (isPlayer1) {
            gameState.setMainDeck(gameState.getMainDeck() & ~cardBit);
            gameState.setPlayer1Deck(gameState.getPlayer1Deck() | cardBit);
            gameState.setPlayer1Tally(gameState.getPlayer1Tally() + bestCard);
        } else {
            gameState.setMainDeck(gameState.getMainDeck() & ~cardBit);
            gameState.setPlayer2Deck(gameState.getPlayer2Deck() | cardBit);
            gameState.setPlayer2Tally(gameState.getPlayer2Tally() + bestCard);
        }
        std::cout << "Drew the perfect card: " << bestCard << "\n";
        return true;
    }
    
    std::cout << "No perfect card available!\n";
    return false;
}

bool GameOperations::executeRefresh(bool isPlayer1) {
    if (isPlayer1) {
        gameState.setMainDeck(gameState.getMainDeck() | gameState.getPlayer1Deck());
        gameState.setPlayer1Deck(0);
        gameState.setPlayer1Tally(0);
        drawCardForPlayer1();
        drawCardForPlayer1();
    } else {
        gameState.setMainDeck(gameState.getMainDeck() | gameState.getPlayer2Deck());
        gameState.setPlayer2Deck(0);
        gameState.setPlayer2Tally(0);
        drawCardForPlayer2();
        drawCardForPlayer2();
    }
    return true;
}

bool GameOperations::executeReincarnation(bool isPlayer1) {
    executeDestroy(isPlayer1);
    drawTrumpCard(isPlayer1 ? 1 : 2);
    return true;
}

bool GameOperations::executeReturn(bool isPlayer1) {
    if (isPlayer1) {
        uint16_t playerDeck = gameState.getPlayer1Deck();
        int playerTally = gameState.getPlayer1Tally();
        
        for (int i = 10; i >= 0; --i) {
            if (playerDeck & (1 << i)) {
                playerDeck &= ~(1 << i);
                gameState.setMainDeck(gameState.getMainDeck() | (1 << i));
                playerTally -= (i + 1);
                gameState.setPlayer1Deck(playerDeck);
                gameState.setPlayer1Tally(playerTally);
                break;
            }
        }
    } else {
        uint16_t playerDeck = gameState.getPlayer2Deck();
        int playerTally = gameState.getPlayer2Tally();
        
        for (int i = 10; i >= 0; --i) {
            if (playerDeck & (1 << i)) {
                playerDeck &= ~(1 << i);
                gameState.setMainDeck(gameState.getMainDeck() | (1 << i));
                playerTally -= (i + 1);
                gameState.setPlayer2Deck(playerDeck);
                gameState.setPlayer2Tally(playerTally);
                break;
            }
        }
    }
    return true;
}

bool GameOperations::executeShield(int amount) {
    if (!gameState.canAdjustBetModifier(-amount)) {
        std::cout << "Cannot decrease bet below 0!\n";
        return true;
    }
    gameState.adjustBetModifier(-amount);
    std::cout << "Bet decreased by " << amount << "!\n";
    return true;
}

bool GameOperations::executeUpBet(int amount) {
    gameState.adjustBetModifier(amount);
    std::cout << "Bet increased by " << amount << "!\n";
    return true;
}

void GameOperations::undoTrumpEffect(uint8_t cardId) {
    switch (cardId) {
        case 7: // Bloodshed
            gameState.adjustBetModifier(-1);
            break;
        case 12: // Go For 17
        case 13: // Go For 24
        case 14: // Go For 27
            gameState.setGoal(21);
            break;
        case 21: // Shield
            if (gameState.canAdjustBetModifier(1)) {
                gameState.adjustBetModifier(1);
            }
            break;
        case 22: // Shield+
            if (gameState.canAdjustBetModifier(2)) {
                gameState.adjustBetModifier(2);
            }
            break;
        case 23: // Two-up
            gameState.adjustBetModifier(-2);
            break;
        case 24: // One-up
            gameState.adjustBetModifier(-1);
            break;
    }
}

bool GameOperations::useTrumpCard(int cardId) {
    if (gameState.isGameOver()) return false;

    bool isPlayer1 = gameState.isPlayer1Turn();
    uint8_t cardCount = isPlayer1 ? 
        gameState.getPlayer1TrumpCardCount(cardId) : 
        gameState.getPlayer2TrumpCardCount(cardId);

    if (cardCount == 0) return false;

    gameState.recordTrumpPlay(cardId, isPlayer1);

    if (isPlayer1) {
        gameState.decrementPlayer1TrumpCard(cardId);
    } else {
        gameState.decrementPlayer2TrumpCard(cardId);
    }

    bool result = false;
    switch (cardId) {
        case 0: case 1: case 2: case 3: case 4: case 5:
            result = executeNumberedCard(cardId + 2);
            break;
        case 6: result = executeBless(isPlayer1); break;
        case 7: result = executeBloodshed(isPlayer1); break;
        case 8: result = executeDestroy(isPlayer1); break;
        case 9: result = executeDisservice(isPlayer1); break;
        case 10: result = executeExchange(isPlayer1); break;
        case 11: result = executeFriendship(isPlayer1); break;
        case 12: result = executeGoFor(17); break;
        case 13: result = executeGoFor(24); break;
        case 14: result = executeGoFor(27); break;
        case 15: result = executeHush(isPlayer1); break;
        case 16: result = executePerfectDraw(isPlayer1); break;
        case 17: result = executeRefresh(isPlayer1); break;
        case 18: result = executeReincarnation(isPlayer1); break;
        case 19: result = executeDisservice(isPlayer1); break;
        case 20: result = executeReturn(isPlayer1); break;
        case 21: result = executeShield(1); break;
        case 22: result = executeShield(2); break;
        case 23: result = executeUpBet(2); break;
        case 24: result = executeUpBet(1); break;
    }

    return result;
}
