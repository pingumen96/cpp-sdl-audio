#pragma once

#include "InputEnabledScene.h"
#include "../math/math.h"
#include <SDL2/SDL.h>

namespace game {

    /**
     * @brief Pong game scene implementation
     *
     * Classic Pong game with two paddles and a ball.
     * - Player 1 (left): W/S keys
     * - Player 2 (right): Up/Down arrows
     * - Ball bounces between paddles and top/bottom walls
     * - Score tracking for both players
     *
     * Now uses ECS-based input system (Phase 1: reads InputState in old methods)
     */
    class PongScene : public InputEnabledScene {
    private:
        // Game entities
        ecs::Entity leftPaddleEntity = 0;
        ecs::Entity rightPaddleEntity = 0;
        ecs::Entity ballEntity = 0;
        ecs::Entity backgroundEntity = 0;

        // Score entities for UI
        ecs::Entity leftScoreEntity = 0;
        ecs::Entity rightScoreEntity = 0;
        ecs::Entity centerLineEntity = 0;

        // Game state
        int leftScore = 0;
        int rightScore = 0;
        bool gameRunning = true;

        // Game constants
        static constexpr float PADDLE_WIDTH = 20.0f;
        static constexpr float PADDLE_HEIGHT = 100.0f;
        static constexpr float PADDLE_SPEED = 300.0f;
        static constexpr float BALL_SIZE = 15.0f;
        static constexpr float BALL_INITIAL_SPEED = 250.0f;
        static constexpr float BALL_SPEED_INCREASE = 1.05f;
        static constexpr int WINNING_SCORE = 5;

        // Screen dimensions (assuming 800x600)
        static constexpr float SCREEN_WIDTH = 800.0f;
        static constexpr float SCREEN_HEIGHT = 600.0f;

        // Input state
        bool leftPaddleUp = false;
        bool leftPaddleDown = false;
        bool rightPaddleUp = false;
        bool rightPaddleDown = false;

    public:
        PongScene() : InputEnabledScene("PongScene") {
            setPausesUnderlying(true);
        }

        void onAttach(scene::SceneManager& manager) override;
        void onActivate() override;
        void onDeactivate() override;
        void onDetach(scene::SceneManager& manager) override;
        void update(float deltaTime) override;

        /**
         * @brief Handle Pong game input
         */
    void handlePongInput(const SDL_Event& event);    private:
        void createGameEntities();
        void updatePaddles(float deltaTime);
        void updateBall(float deltaTime);
        void checkCollisions();
        void resetBall();
        void updateScore();
        void checkWinCondition();

        // Utility functions
        math::Vec2f getBallDirection() const;
        void setBallDirection(const math::Vec2f& direction);
        float getBallSpeed() const;
        void setBallSpeed(float speed);

        // Collision detection helpers
        bool checkPaddleBallCollision(ecs::Entity paddleEntity) const;
        bool isPointInRect(const math::Vec2f& point, const math::Vec2f& rectPos, const math::Vec2f& rectSize) const;
        math::Vec2f calculateBallBounce(const math::Vec2f& ballPos, const math::Vec2f& paddlePos, const math::Vec2f& paddleSize) const;
    };

} // namespace game
