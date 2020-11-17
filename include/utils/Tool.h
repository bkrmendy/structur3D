//
//  Header.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 06..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Tool_h
#define Tool_h

#include <optional>
#include <functional>

namespace S3D {

template<typename T>
class Tool {
    std::optional<T> state;
public:
    Tool() : state{std::nullopt} {}

    void start(T initialState) {
        this->state = std::optional{initialState};
    }

    void step(std::function<T(T)> handler) {
        if (this->state.has_value()) {
            this->state = handler(this->state.value());
        }
    }

    void finish() {
        this->state = std::nullopt;
    }
};

}

#endif /* Header_h */
