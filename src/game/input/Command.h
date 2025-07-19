#pragma once



namespace game { class Avatar; }

namespace game::input {
    class Command {
    public:
        virtual ~Command() = default;
        virtual void execute(game::Avatar& avatar) = 0;
    };

}

