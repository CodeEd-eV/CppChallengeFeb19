//
// Created by Oliver on 11-Jan-19.
//

#ifndef GAME_H
#define GAME_H


#include <array>
#include <optional>
#include <string>
#include <exception>
#include <memory>

enum class FIELD_STATE : std::uint8_t {

    UNSET = 0,
    YELLOW = 1,
    RED = 2,

};

class Column {
public:

    template<typename I = std::uint32_t>
    static constexpr auto HEIGHT = I(7);

    using ColumnType = std::array<FIELD_STATE, HEIGHT<>>;

    Column(ColumnType& column);

    bool insert(FIELD_STATE coin);

    bool isFull() const;

    //0 is at the very top of the column, 6 at the bottom
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    FIELD_STATE operator[](T ind) const;

private:

    ColumnType& columnRef;

};

class ConnectFour {
public:

    template<typename I = std::uint32_t>
    static constexpr auto WIDTH = I(7);

    template<typename I = std::uint32_t>
    static constexpr auto HEIGHT = Column::HEIGHT<I>;

    using FieldType = std::array<Column::ColumnType, WIDTH<>>;

    ConnectFour() noexcept;

    //The field is saved in column major order!
    //{(0, 0), (1, 0), (2, 0), ....., (6, 0),
    // (0, 1), (1, 1), (2, 1), ....., (6, 1),
    // .....
    // .....
    // (0, 6), ...., ......,          (6, 6)}
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    Column operator[](T ind);

    template<typename T, typename = std::enable_if_t <std::is_integral_v<T>>>
    std::optional<FIELD_STATE> at(const std::tuple<T, T>& ind) const;

private:

    FieldType gameField;

};



enum class GAME_RESULT : std::uint8_t {

    YELLOW,
    RED,
    DRAW,
    CONTINUE

};

enum class RESULT_REASON : std::uint8_t {

    REGULAR, //Regular game over, either it is a draw or the winner team connected four
    TIMEOUT, //The looser team did not send a move command within 5 seconds
    IRREGULAR_MOVE, //The looser team send an invalid command

};


struct alignas(1) LLGameInfo {

    struct alignas(1) PlayerInfo {

        char Name[15];
        FIELD_STATE Color;

    }Player[2];

    GAME_RESULT Result;

    /*
     * The field is stored in column major order!
     * [0][0], [1][0], [2][0], .... , [6][0] <-- Top Row
     * [0][1], [1][1], .....        , [6][1]
     * [0][2], [1][2], .....
     * .....
     * .....
     * [0[6], [1][6], .....         , [6][6] <-- Bottom Row
     */
    FIELD_STATE Field[7][7];

    //Ignore, when Result is CONTINUE!
    RESULT_REASON Reason;

};



//checks if the game is over and who has won it
GAME_RESULT CheckConnectFour(const ConnectFour& ccf);

RESULT_REASON Insert(ConnectFour& ccf, FIELD_STATE fs, std::uint32_t col);



#include "ConnectFourImpl.inl"


#endif
