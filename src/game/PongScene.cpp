#include "PongScene.h"
#include "../ecs/components/CommonComponents.h"
#include "../ecs/components/Renderable2D.h"
#include "../scene/rendering/Renderer2D.h"
#include <iostream>
#include <random>
#include <cmath>

namespace game {

    void PongScene::onAttach(scene::SceneManager& manager) {
        std::cout << "[PongScene] Pong scene attached" << std::endl;

        // Initialize 2D rendering system
        initialize2D(manager);

        // Create game entities
        createGameEntities();
    }

    void PongScene::onActivate() {
        std::cout << "[PongScene] Pong activated" << std::endl;
        gameRunning = true;
    }

    void PongScene::onDeactivate() {
        std::cout << "[PongScene] Pong deactivated" << std::endl;
    }

    void PongScene::onDetach(scene::SceneManager& manager) {
        std::cout << "[PongScene] Pong scene detached" << std::endl;
        scene::Scene2D::onDetach(manager);
    }

    void PongScene::update(float deltaTime) {
        if (!gameRunning) return;

        // Update base scene systems
        scene::Scene2D::update(deltaTime);

        // Update game-specific logic
        updatePaddles(deltaTime);
        updateBall(deltaTime);
        checkCollisions();
        updateScore();
        checkWinCondition();
    }

    void PongScene::handlePongInput(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                // Player 1 (left paddle) - W/S
            case SDLK_w:
                leftPaddleUp = true;
                break;
            case SDLK_s:
                leftPaddleDown = true;
                break;
                // Player 2 (right paddle) - Up/Down arrows
            case SDLK_UP:
                rightPaddleUp = true;
                break;
            case SDLK_DOWN:
                rightPaddleDown = true;
                break;
                // Reset game
            case SDLK_r:
                leftScore = 0;
                rightScore = 0;
                resetBall();
                gameRunning = true;
                break;
                // Exit to menu
            case SDLK_ESCAPE:
                // This should be handled by the game state manager
                gameRunning = false;
                break;
            }
        } else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            case SDLK_w:
                leftPaddleUp = false;
                break;
            case SDLK_s:
                leftPaddleDown = false;
                break;
            case SDLK_UP:
                rightPaddleUp = false;
                break;
            case SDLK_DOWN:
                rightPaddleDown = false;
                break;
            }
        }
    }

    void PongScene::createGameEntities() {
        std::cout << "[PongScene] Creating game entities..." << std::endl;

        // Convert from absolute coordinates (0-800, 0-600) to centered coordinates (-400 to +400, -300 to +300)
        // Center = (0, 0), Left edge = -400, Right edge = +400, Top edge = +300, Bottom edge = -300

        // Create center line - make it more visible
        centerLineEntity = createSprite(
            math::Vec2f(0.0f, 0.0f), // Center of screen in centered coordinates
            math::Vec2f(4.0f, SCREEN_HEIGHT * 0.8f), // Thicker and shorter line
            "", // No texture
            scene::Color::White,
            1 // Line layer
        );
        std::cout << "[PongScene] Center line entity created: " << centerLineEntity << std::endl;

        // Create left paddle - make it bigger and more visible
        leftPaddleEntity = createSprite(
            math::Vec2f(-350.0f, 0.0f), // Left side in centered coordinates (50px from left edge)
            math::Vec2f(PADDLE_WIDTH * 2, PADDLE_HEIGHT * 1.5f), // Make it bigger
            "", // No texture
            scene::Color::Red, // Red color for visibility
            2 // Paddle layer
        );
        std::cout << "[PongScene] Left paddle entity created: " << leftPaddleEntity << std::endl;

        // Create right paddle - make it bigger and more visible
        rightPaddleEntity = createSprite(
            math::Vec2f(350.0f, 0.0f), // Right side in centered coordinates (50px from right edge)
            math::Vec2f(PADDLE_WIDTH * 2, PADDLE_HEIGHT * 1.5f), // Make it bigger
            "", // No texture
            scene::Color::Blue, // Blue color for visibility
            2 // Paddle layer
        );
        std::cout << "[PongScene] Right paddle entity created: " << rightPaddleEntity << std::endl;

        // Create ball - make it bigger
        ballEntity = createSprite(
            math::Vec2f(0.0f, 0.0f), // Center of screen in centered coordinates
            math::Vec2f(BALL_SIZE * 3, BALL_SIZE * 3), // Make it 3x bigger
            "", // No texture
            scene::Color::Green, // Green color for visibility
            3 // Ball layer
        );
        std::cout << "[PongScene] Ball entity created: " << ballEntity << std::endl;

        // Add velocity component to ball for movement
        auto* coordinator = getCoordinator();
        if (coordinator && ballEntity != 0) {
            math::Vec2f initialDir = getBallDirection();
            ecs::components::Velocity velocity;
            velocity.linear = math::Vec3f(initialDir.x() * BALL_INITIAL_SPEED, initialDir.y() * BALL_INITIAL_SPEED, 0.0f);
            velocity.angular = math::Vec3f(0.0f, 0.0f, 0.0f);
            coordinator->addComponent(ballEntity, velocity);
            std::cout << "[PongScene] Ball velocity set: (" << velocity.linear.x() << ", " << velocity.linear.y() << ")" << std::endl;
        }

        std::cout << "[PongScene] Game entities created successfully" << std::endl;
        std::cout << "[PongScene] Screen dimensions: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << std::endl;

        // Debug: verificare se il renderer2D è disponibile
        auto* renderer = getRenderer2D();
        if (renderer) {
            std::cout << "[PongScene] Renderer2D is available" << std::endl;
        } else {
            std::cout << "[PongScene] WARNING: Renderer2D is NULL!" << std::endl;
        }
    }    void PongScene::updatePaddles(float deltaTime) {
        auto* coordinator = getCoordinator();
        if (!coordinator) return;

        // Update left paddle position references
        if (leftPaddleEntity != 0) {
            auto& leftTransform = coordinator->getComponent<ecs::components::Transform>(leftPaddleEntity);

            if (leftPaddleUp) {
                leftTransform.position.y() -= PADDLE_SPEED * deltaTime;
            }
            if (leftPaddleDown) {
                leftTransform.position.y() += PADDLE_SPEED * deltaTime;
            }

            // Keep paddle within screen bounds (centered coordinates: -300 to +300 for Y)
            float paddleHalfHeight = (PADDLE_HEIGHT * 1.5f) / 2; // Account for bigger paddle
            leftTransform.position.y() = std::max(-300.0f + paddleHalfHeight,
                std::min(300.0f - paddleHalfHeight, leftTransform.position.y()));
        }

        // Update right paddle
        if (rightPaddleEntity != 0) {
            auto& rightTransform = coordinator->getComponent<ecs::components::Transform>(rightPaddleEntity);

            if (rightPaddleUp) {
                rightTransform.position.y() -= PADDLE_SPEED * deltaTime;
            }
            if (rightPaddleDown) {
                rightTransform.position.y() += PADDLE_SPEED * deltaTime;
            }

            // Keep paddle within screen bounds (centered coordinates: -300 to +300 for Y)
            float paddleHalfHeight = (PADDLE_HEIGHT * 1.5f) / 2; // Account for bigger paddle
            rightTransform.position.y() = std::max(-300.0f + paddleHalfHeight,
                std::min(300.0f - paddleHalfHeight, rightTransform.position.y()));
        }
    }

    void PongScene::updateBall(float deltaTime) {
        auto* coordinator = getCoordinator();
        if (!coordinator || ballEntity == 0) return;

        auto& ballTransform = coordinator->getComponent<ecs::components::Transform>(ballEntity);
        auto& ballVelocity = coordinator->getComponent<ecs::components::Velocity>(ballEntity);

        // Update ball position based on velocity
        ballTransform.position.x() += ballVelocity.linear.x() * deltaTime;
        ballTransform.position.y() += ballVelocity.linear.y() * deltaTime;

        // Check collision with top and bottom walls (centered coordinates: -300 to +300 for Y)
        float ballHalfSize = (BALL_SIZE * 3) / 2; // Account for bigger ball
        if (ballTransform.position.y() <= -300.0f + ballHalfSize) {
            ballTransform.position.y() = -300.0f + ballHalfSize;
            ballVelocity.linear.y() = -ballVelocity.linear.y();
        } else if (ballTransform.position.y() >= 300.0f - ballHalfSize) {
            ballTransform.position.y() = 300.0f - ballHalfSize;
            ballVelocity.linear.y() = -ballVelocity.linear.y();
        }

        // Check if ball went off screen (scoring) - centered coordinates: -400 to +400 for X
        if (ballTransform.position.x() < -400.0f - ballHalfSize) {
            // Right player scores
            rightScore++;
            resetBall();
        } else if (ballTransform.position.x() > 400.0f + ballHalfSize) {
            // Left player scores
            leftScore++;
            resetBall();
        }
    }

    void PongScene::checkCollisions() {
        auto* coordinator = getCoordinator();
        if (!coordinator || ballEntity == 0) return;

        // Check collision with left paddle
        if (checkPaddleBallCollision(leftPaddleEntity)) {
            auto& ballTransform = coordinator->getComponent<ecs::components::Transform>(ballEntity);
            auto& ballVelocity = coordinator->getComponent<ecs::components::Velocity>(ballEntity);
            auto& paddleTransform = coordinator->getComponent<ecs::components::Transform>(leftPaddleEntity);

            // Calculate bounce direction
            math::Vec2f ballPos2D(ballTransform.position.x(), ballTransform.position.y());
            math::Vec2f paddlePos2D(paddleTransform.position.x(), paddleTransform.position.y());
            math::Vec2f bounceDir = calculateBallBounce(ballPos2D, paddlePos2D,
                math::Vec2f(PADDLE_WIDTH, PADDLE_HEIGHT));

            // Increase speed slightly and apply new direction
            float currentSpeed = std::sqrt(ballVelocity.linear.x() * ballVelocity.linear.x() +
                ballVelocity.linear.y() * ballVelocity.linear.y());
            ballVelocity.linear.x() = bounceDir.x() * (currentSpeed * BALL_SPEED_INCREASE);
            ballVelocity.linear.y() = bounceDir.y() * (currentSpeed * BALL_SPEED_INCREASE);

            // Ensure ball is not inside paddle
            ballTransform.position.x() = paddleTransform.position.x() + PADDLE_WIDTH / 2 + BALL_SIZE / 2;
        }

        // Check collision with right paddle
        if (checkPaddleBallCollision(rightPaddleEntity)) {
            auto& ballTransform = coordinator->getComponent<ecs::components::Transform>(ballEntity);
            auto& ballVelocity = coordinator->getComponent<ecs::components::Velocity>(ballEntity);
            auto& paddleTransform = coordinator->getComponent<ecs::components::Transform>(rightPaddleEntity);

            // Calculate bounce direction
            math::Vec2f ballPos2D(ballTransform.position.x(), ballTransform.position.y());
            math::Vec2f paddlePos2D(paddleTransform.position.x(), paddleTransform.position.y());
            math::Vec2f bounceDir = calculateBallBounce(ballPos2D, paddlePos2D,
                math::Vec2f(PADDLE_WIDTH, PADDLE_HEIGHT));

            // Increase speed slightly and apply new direction
            float currentSpeed = std::sqrt(ballVelocity.linear.x() * ballVelocity.linear.x() +
                ballVelocity.linear.y() * ballVelocity.linear.y());
            ballVelocity.linear.x() = bounceDir.x() * (currentSpeed * BALL_SPEED_INCREASE);
            ballVelocity.linear.y() = bounceDir.y() * (currentSpeed * BALL_SPEED_INCREASE);

            // Ensure ball is not inside paddle
            ballTransform.position.x() = paddleTransform.position.x() - PADDLE_WIDTH / 2 - BALL_SIZE / 2;
        }
    }

    void PongScene::resetBall() {
        auto* coordinator = getCoordinator();
        if (!coordinator || ballEntity == 0) return;

        auto& ballTransform = coordinator->getComponent<ecs::components::Transform>(ballEntity);
        auto& ballVelocity = coordinator->getComponent<ecs::components::Velocity>(ballEntity);

        // Reset position to center (centered coordinates)
        ballTransform.position = math::Vec3f(0.0f, 0.0f, 0.0f);

        // Reset velocity with random direction
        math::Vec2f initialDir = getBallDirection();
        ballVelocity.linear = math::Vec3f(initialDir.x() * BALL_INITIAL_SPEED, initialDir.y() * BALL_INITIAL_SPEED, 0.0f);
    }

    void PongScene::updateScore() {
        // This would update score display entities if we had text rendering
        // For now, we'll just print to console when score changes
        static int lastLeftScore = -1;
        static int lastRightScore = -1;

        if (leftScore != lastLeftScore || rightScore != lastRightScore) {
            std::cout << "[PongScene] Score: " << leftScore << " - " << rightScore << std::endl;
            lastLeftScore = leftScore;
            lastRightScore = rightScore;
        }
    }

    void PongScene::checkWinCondition() {
        if (leftScore >= WINNING_SCORE) {
            std::cout << "[PongScene] Left Player Wins! Final Score: " << leftScore << " - " << rightScore << std::endl;
            gameRunning = false;
        } else if (rightScore >= WINNING_SCORE) {
            std::cout << "[PongScene] Right Player Wins! Final Score: " << leftScore << " - " << rightScore << std::endl;
            gameRunning = false;
        }
    }

    math::Vec2f PongScene::getBallDirection() const {
        // Generate random direction
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * static_cast<float>(M_PI));

        // Generate angle that's not too vertical (avoid straight up/down)
        float angle;
        do {
            angle = angleDist(gen);
        } while (std::abs(std::sin(angle)) > 0.8f); // Avoid too vertical angles

        math::Vec2f direction(std::cos(angle), std::sin(angle));
        return direction.normalized();
    }

    void PongScene::setBallDirection(const math::Vec2f& direction) {
        auto* coordinator = getCoordinator();
        if (!coordinator || ballEntity == 0) return;

        auto& ballVelocity = coordinator->getComponent<ecs::components::Velocity>(ballEntity);
        float currentSpeed = std::sqrt(ballVelocity.linear.x() * ballVelocity.linear.x() +
            ballVelocity.linear.y() * ballVelocity.linear.y());
        math::Vec2f normalizedDir = direction.normalized();
        ballVelocity.linear.x() = normalizedDir.x() * currentSpeed;
        ballVelocity.linear.y() = normalizedDir.y() * currentSpeed;
    }

    float PongScene::getBallSpeed() const {
        auto* coordinator = getCoordinator();
        if (!coordinator || ballEntity == 0) return 0.0f;

        const auto& ballVelocity = coordinator->getComponent<ecs::components::Velocity>(ballEntity);
        return std::sqrt(ballVelocity.linear.x() * ballVelocity.linear.x() +
            ballVelocity.linear.y() * ballVelocity.linear.y());
    }

    void PongScene::setBallSpeed(float speed) {
        auto* coordinator = getCoordinator();
        if (!coordinator || ballEntity == 0) return;

        auto& ballVelocity = coordinator->getComponent<ecs::components::Velocity>(ballEntity);
        float currentSpeed = std::sqrt(ballVelocity.linear.x() * ballVelocity.linear.x() +
            ballVelocity.linear.y() * ballVelocity.linear.y());
        if (currentSpeed > 0.0f) {
            float scale = speed / currentSpeed;
            ballVelocity.linear.x() *= scale;
            ballVelocity.linear.y() *= scale;
        }
    }

    bool PongScene::checkPaddleBallCollision(ecs::Entity paddleEntity) const {
        auto* coordinator = getCoordinator();
        if (!coordinator || paddleEntity == 0 || ballEntity == 0) return false;

        const auto& ballTransform = coordinator->getComponent<ecs::components::Transform>(ballEntity);
        const auto& paddleTransform = coordinator->getComponent<ecs::components::Transform>(paddleEntity);

        // Simple AABB collision detection
        float ballLeft = ballTransform.position.x() - BALL_SIZE / 2;
        float ballRight = ballTransform.position.x() + BALL_SIZE / 2;
        float ballTop = ballTransform.position.y() - BALL_SIZE / 2;
        float ballBottom = ballTransform.position.y() + BALL_SIZE / 2;

        float paddleLeft = paddleTransform.position.x() - PADDLE_WIDTH / 2;
        float paddleRight = paddleTransform.position.x() + PADDLE_WIDTH / 2;
        float paddleTop = paddleTransform.position.y() - PADDLE_HEIGHT / 2;
        float paddleBottom = paddleTransform.position.y() + PADDLE_HEIGHT / 2;

        return (ballLeft < paddleRight && ballRight > paddleLeft &&
            ballTop < paddleBottom && ballBottom > paddleTop);
    }

    bool PongScene::isPointInRect(const math::Vec2f& point, const math::Vec2f& rectPos, const math::Vec2f& rectSize) const {
        float left = rectPos.x() - rectSize.x() / 2;
        float right = rectPos.x() + rectSize.x() / 2;
        float top = rectPos.y() - rectSize.y() / 2;
        float bottom = rectPos.y() + rectSize.y() / 2;

        return (point.x() >= left && point.x() <= right &&
            point.y() >= top && point.y() <= bottom);
    }

    math::Vec2f PongScene::calculateBallBounce(const math::Vec2f& ballPos, const math::Vec2f& paddlePos, const math::Vec2f& paddleSize) const {
        // Calculate where on the paddle the ball hit (normalized from -1 to 1)
        float hitPos = (ballPos.y() - paddlePos.y()) / (paddleSize.y() / 2);
        hitPos = std::max(-1.0f, std::min(1.0f, hitPos)); // Clamp to [-1, 1]

        // Calculate bounce angle based on hit position
        float bounceAngle = hitPos * (static_cast<float>(M_PI) / 4); // Max 45 degrees

        // Determine horizontal direction based on which paddle (centered coordinates)
        float horizontalDir = (paddlePos.x() < 0.0f) ? 1.0f : -1.0f;

        math::Vec2f bounceDir(horizontalDir * std::cos(bounceAngle), std::sin(bounceAngle));
        return bounceDir.normalized();
    }

} // namespace game
