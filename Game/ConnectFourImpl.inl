


template<typename T, typename>
FIELD_STATE Column::operator[](T ind) const {

    return columnRef[ind];

}


template<typename T, typename>
Column ConnectFour::operator[](T ind) {

    return gameField[ind];

}


template<typename T, typename>
std::optional<FIELD_STATE>  ConnectFour::at(const std::tuple<T, T>& ind) const {

    auto w = std::get<0>(ind);
    auto h = std::get<1>(ind);

    if (w < 0 || w >= WIDTH<T> ||
        h < 0 || h >= HEIGHT<T>) {
        return std::nullopt;
    }

    return gameField.at(w).at(h);

}





