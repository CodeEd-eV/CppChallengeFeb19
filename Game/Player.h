//
// Created by Oliver on 17-Jan-19.
//

#ifndef CONNECTFOURSERVER_PLAYER_H
#define CONNECTFOURSERVER_PLAYER_H

#include "ConnectFour.h"

#include <memory>
#include <string>
#include <limits>
#include <exception>
#undef max


class PlayerError : public std::exception {
public:

    explicit constexpr PlayerError(const char* cc = nullptr) noexcept;

    const char* what() const override;

private:

    const char* error;

};


//Type erasure pattern
class Player {
public:

    static constexpr auto TIMEOUT_MOVE = std::numeric_limits<std::uint32_t>::max();

    Player() = default;

    template<typename T, typename = std::enable_if_t<!std::is_same_v<T, Player>>>
    explicit Player(T&& p, FIELD_STATE col, std::uint32_t pnr);

    Player(Player&&) noexcept = default;
    Player& operator=(Player&&) noexcept = default;

    std::uint32_t chooseMove(ConnectFour& cf);

    void tellGameState(ConnectFour& cf, const Player& other, GAME_RESULT result, RESULT_REASON rr);

    const std::string& getName() const noexcept;

    FIELD_STATE getColor() const noexcept;

    void setColor(FIELD_STATE col) noexcept;

private:

    class Concept {
    public:

        virtual std::uint32_t chooseMove(ConnectFour& cf) = 0;

        virtual void tellGameState(ConnectFour& cf, const Player& other, GAME_RESULT result, RESULT_REASON rr) = 0;

        virtual const std::string& getName() const noexcept = 0;

        virtual FIELD_STATE getColor() const noexcept = 0;

        virtual void setColor(FIELD_STATE col) noexcept = 0;

        virtual ~Concept() = default;

    };

    template<typename P>
    class PlayerModel : public Concept {
    public:

        PlayerModel(P&& p, FIELD_STATE col, std::uint32_t pnr);

        std::uint32_t chooseMove(ConnectFour& cf) override;

        void tellGameState(ConnectFour& cf, const Player& other, GAME_RESULT result, RESULT_REASON rr) override;

        const std::string& getName() const noexcept override;

        FIELD_STATE  getColor() const noexcept override;

        void setColor(FIELD_STATE col) noexcept override;

    private:

        std::uint32_t playerNr;
        FIELD_STATE color;
        P player;

    };

    std::unique_ptr<Concept> playerPtr;

};


LLGameInfo MakeGameInfo(const ConnectFour& cf, const std::string& plname1,
                        FIELD_STATE col1, const std::string& plname2, FIELD_STATE col2,
                        GAME_RESULT result, RESULT_REASON rr);

Player MakeRandomAIPlayer(FIELD_STATE color, std::uint32_t plnr);


#include "PlayerImpl.inl"


#endif //CONNECTFOURSERVER_PLAYER_H
