
#include <utility>

template<typename T, typename>
Player::Player(T&& p, FIELD_STATE col, std::uint32_t pnr) :
playerPtr(static_cast<Player::Concept*>(new Player::PlayerModel{std::forward<T>(p), col, pnr}) ){

}

template<typename P>
Player::PlayerModel<P>::PlayerModel(P &&p, FIELD_STATE col, std::uint32_t pnr) :
player(std::forward<P>(p)), color(col), playerNr(pnr) {

}

template<typename P>
const std::string& Player::PlayerModel<P>::getName() const {

    return player.getName();

}

template<typename P>
std::uint32_t Player::PlayerModel<P>::chooseMove(ConnectFour &cf) {

    return player.chooseMove(cf);

}

template<typename P>
void Player::PlayerModel<P>::tellGameState(ConnectFour &cf, const Player &other, GAME_RESULT result, RESULT_REASON rr, bool playAgain) {

    if (playerNr == 0) {

        LLGameInfo gi = MakeGameInfo(cf, getName(), getColor(), other.getName(), other.getColor(), result, rr, playAgain);
        player.tellGameState(gi);

    } else if (playerNr == 1) {

        LLGameInfo gi = MakeGameInfo(cf, other.getName(), other.getColor(), getName(), getColor(), result, rr, playAgain);
        player.tellGameState(gi);

    }

}


template<typename P>
FIELD_STATE  Player::PlayerModel<P>::getColor() const {

    return color;

}

template<typename P>
void Player::PlayerModel<P>::setColor(FIELD_STATE col) {

    color = col;

}

