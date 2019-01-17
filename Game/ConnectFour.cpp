//
// Created by Oliver on 11-Jan-19.
//

#include "ConnectFour.h"



ConnectFour::ConnectFour() noexcept {

    for (auto& c : gameField) {
        for(auto& e: c) {
            e = FIELD_STATE::UNSET;
        }
    }

}

Column::Column(Column::ColumnType &column) : columnRef(column) {

}

bool Column::isFull() const {

    return columnRef[0] != FIELD_STATE::UNSET;

}

bool Column::insert(FIELD_STATE coin) {

    if (isFull()) {
        return false;
    }

    for (auto i = Column::HEIGHT<int> - 1; i >= 0; i--) {

        if (columnRef[i] == FIELD_STATE::UNSET) {
            columnRef[i] = coin;
            break;
        }

    }

    return true;

}






InvalidMove::InvalidMove(const char *err) : msg(err) {

}

const char* InvalidMove::what() const noexcept {

    return msg;

}

GAME_RESULT CheckConnectFour(const ConnectFour& ccf) {

    bool unsetFieldFound = false;
    for(int i = 0; i < ConnectFour::WIDTH<int>; i++) {
        for (int j = 0; j < ConnectFour::HEIGHT<int>; j++) {

            if (*ccf.at(std::tuple(i, j)) == FIELD_STATE::UNSET) {
                unsetFieldFound = true;
            }

            //Test for four in one color in a column
            auto c1 = ccf.at(std::tuple(i, j));
            auto c2 = ccf.at(std::tuple(i, j + 1));
            auto c3 = ccf.at(std::tuple(i, j + 2));
            auto c4 = ccf.at(std::tuple(i, j + 3));

            if (c1 && c1 != FIELD_STATE::UNSET && c1 == c2 && c1 == c3 && c1 == c4) {
                return c1.value() == FIELD_STATE::YELLOW ? GAME_RESULT::YELLOW : GAME_RESULT::RED;
            }

            //Test for four in one color in a row
            auto r1 = ccf.at(std::tuple(i, j));
            auto r2 = ccf.at(std::tuple(i + 1, j));
            auto r3 = ccf.at(std::tuple(i + 2, j));
            auto r4 = ccf.at(std::tuple(i + 3, j));

            if (r1 && r1 != FIELD_STATE::UNSET && r1 == r2 && r1 == r3 && r1 == r4) {
                return r1.value() == FIELD_STATE::YELLOW ? GAME_RESULT::YELLOW : GAME_RESULT::RED;
            }

            //Test for four in one color in an upper right pointing diagonal
            auto ur1 = ccf.at(std::tuple(i, j));
            auto ur2 = ccf.at(std::tuple(i + 1, j - 1));
            auto ur3 = ccf.at(std::tuple(i + 2, j - 2));
            auto ur4 = ccf.at(std::tuple(i + 3, j - 3));

            if (ur1 && ur1 != FIELD_STATE::UNSET && ur1 == ur2 && ur1 == ur3 && ur1 == ur4) {
                return ur1.value() == FIELD_STATE::YELLOW ? GAME_RESULT::YELLOW : GAME_RESULT::RED;
            }

            //Test for four in on e color in an upper left pointing diagonal;
            auto ul1 = ccf.at(std::tuple(i, j));
            auto ul2 = ccf.at(std::tuple(i - 1, j - 1));
            auto ul3 = ccf.at(std::tuple(i - 2, j - 2));
            auto ul4 = ccf.at(std::tuple(i - 3, j - 3));

            if (ul1 && ul1 != FIELD_STATE::UNSET && ul1 == ul2 && ul1 == ul3 && ul1 == ul4) {
                return ul1.value() == FIELD_STATE::YELLOW ? GAME_RESULT::YELLOW : GAME_RESULT::RED;
            }

        }
    }

    return unsetFieldFound ? GAME_RESULT::CONTINUE : GAME_RESULT::DRAW;

}

void Insert(ConnectFour& ccf, FIELD_STATE fs, std::uint32_t col) {

    if (col < 0 || ccf.WIDTH<decltype(col)> <= col) {
        throw InvalidMove("Column does not exist");
    }

    auto column = ccf[col];

    if(!column.insert(fs)) {
        throw InvalidMove("Insert failed: Column is full!");
    }

}






