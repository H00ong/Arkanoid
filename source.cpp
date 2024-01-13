#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>

using namespace sf;
using namespace std;

class Block {
private:
    Texture texture;
    Sprite block;

    int Collision_count;
    bool block_life;
public:
    Block() : Collision_count(0), block_life(true) {
        block.setScale(1.5f, 1.5f);
        texture.loadFromFile("images/Arkanoid/block01.png");

        block.setTexture(texture);
    }

    void update() {
        ++Collision_count;
        if (Collision_count == 1) {
            texture.loadFromFile("images/Arkanoid/block02.png");
            block.setScale(1.5f, 1.5f);
            block.setTexture(texture);
        }
        else if(Collision_count == 2){
            block_life = false;
        }
    }

    bool life() {
        return block_life;
    }

    Sprite& ret_sprite() {
        return block;
    }
};

int main()
{
    RenderWindow app(VideoMode(1280, 848), "Arkanoid!");
    app.setFramerateLimit(60);

    Texture t2, t3, t4;
    t2.loadFromFile("images/Arkanoid/whitewall.jpg"); // 1280 X 848
    t3.loadFromFile("images/Arkanoid/ball.png"); // 24 X 24
    t4.loadFromFile("images/Arkanoid/wall.png"); // 24 X 24

    Sprite sBackground(t2), sBall(t3), sPaddle(t4);

    sPaddle.setScale(6.0f, 1.0f);
    sf::FloatRect paddleRect = sPaddle.getLocalBounds();
    sPaddle.setOrigin(paddleRect.left + paddleRect.width / 2.0f, paddleRect.top + paddleRect.height / 2.0f);
    sPaddle.setPosition(640, 848 - 12);

    sf::FloatRect ballRect = sBall.getLocalBounds();
    sBall.setOrigin(ballRect.left + ballRect.width / 2.0f, ballRect.top + ballRect.height / 2.0f);
    sBall.setPosition(600, 600);

    const int default_block_count = 16 * 10;
    vector<Block> blocks(default_block_count);

    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 10; ++j) {
            FloatRect block_Rect = blocks[i * 10 + j].ret_sprite().getGlobalBounds();
            blocks[i * 10 + j].ret_sprite().setPosition((i + 1) * ( block_Rect.width + 8) + 24, (j + 1) * (block_Rect.height + 8) + 24);
        }
    }

    Clock clock; // for rotate;    
    sf::Clock rightClock; // for rightkey
    sf::Clock leftClock; // for leftkey
    float rotate_val = 5.0f; // angle velocity

    // paddle velocity
    float paddle_speed = 0.0f;
    float paddle_acceleration = 5.0f;
    float paddle_max_speed = 20.0f;
    float Moving_default_speed = 10.0f;
    
    // ball velocity
    float dx = 7, dy = -7;
    const float ball_velocity = sqrt(dx * dx + dy * dy);
    // for vector erase
    int swith_block_count = 0;

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        auto ball_pos = sBall.getPosition();

        ball_pos.x += dx;
        ball_pos.y += dy;
        
        if (dx > 0) {
            sBall.rotate(-rotate_val);
        }
        else {
            sBall.rotate(rotate_val);
        }

        //공이 블록과 충돌하는 상황
        for (int i = 0; i < blocks.size(); ++i) {
            const float EPSILON = 0.00005f;

            FloatRect block_bounds = blocks[i].ret_sprite().getGlobalBounds();
            
            FloatRect Left_wall = FloatRect(block_bounds.left, block_bounds.top, EPSILON, block_bounds.height);
            FloatRect Right_wall = FloatRect(block_bounds.left + block_bounds.width, block_bounds.top, EPSILON, block_bounds.height);
            FloatRect under_wall = FloatRect(block_bounds.left, block_bounds.top + block_bounds.height, block_bounds.width, EPSILON);
            FloatRect up_wall = FloatRect(block_bounds.left, block_bounds.top, block_bounds.width, EPSILON);
            
            bool is_collision = false;
            // 공이 블록 왼쪽 변과 부딪히는 상황 or 공이 블록 오른쪽 변과 부딪히는 상황
            if (FloatRect(ball_pos.x - ballRect.width / 2.0f, ball_pos.y - ballRect.height / 2.0f, ballRect.width, ballRect.height).intersects(Left_wall) ||
                FloatRect(ball_pos.x - ballRect.width / 2.0f, ball_pos.y - ballRect.height / 2.0f, ballRect.width, ballRect.height).intersects(Right_wall)
                ) 
            {
                is_collision = true;
                dx = -dx;
            }
            
            // 공이 블록 아래쪽 변과 부딪히는 상황 or 공이 블록 위쪽 변과 부딪히는 상황
            if (FloatRect(ball_pos.x - ballRect.width / 4.0f, ball_pos.y - ballRect.height / 4.0f, ballRect.width/2.0f, ballRect.height/2.0f).intersects(under_wall) || 
                FloatRect(ball_pos.x - ballRect.width / 4.0f, ball_pos.y - ballRect.height / 4.0f, ballRect.width/2.0f, ballRect.height/2.0f).intersects(up_wall)) 
            {
                is_collision = true;
                dy = -dy;
            }

            if (is_collision) {
                blocks[i].update();
                if (!blocks[i].life()) {
                    ++swith_block_count;
                    blocks[i].ret_sprite().setPosition(-100, 0);
                }
            }
        }

        if (swith_block_count == default_block_count) {
            //게임 승리
            blocks.clear();
        }

        // 공이 벽에 부딪히는 상황
        if (ball_pos.x < ballRect.width / 2.0f || ball_pos.x > 1280 - ballRect.width / 2.0f) {
            dx = -dx;
        }

        if (ball_pos.y < ballRect.width / 2.0f) {
            dy = -dy;
        }

        if (ball_pos.y > 848 - ballRect.width / 2.0f) {
            app.close();
        }
        // 패들을 움직이는 상황
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            float time = rightClock.getElapsedTime().asSeconds();
            paddle_speed = std::min(paddle_max_speed, Moving_default_speed + paddle_acceleration * time);
            sPaddle.move(paddle_speed, 0);
            leftClock.restart();
        }
        else if (Keyboard::isKeyPressed(Keyboard::Left)) {
            float time = leftClock.getElapsedTime().asSeconds();
            paddle_speed = -(std::min(paddle_max_speed, Moving_default_speed + paddle_acceleration * time));
            sPaddle.move(paddle_speed, 0);
            rightClock.restart();
        }
        else {
            rightClock.restart();
            leftClock.restart();
        }

        auto paddle_pos = sPaddle.getPosition();

        const float CONSTANT = 0.12f; // 내가 정한 임의의 상수

        if (FloatRect(ball_pos.x - ballRect.width / 2.0f, ball_pos.y - 12, 24, 24).intersects(sPaddle.getGlobalBounds())) { // paddle의 길이 24 * 6 = 144
            dx = (ball_pos.x - paddle_pos.x) * CONSTANT; 
            dy = -sqrt(ball_velocity * ball_velocity - dx * dx);
        }

        sBall.setPosition(ball_pos.x, ball_pos.y);

        app.clear();
        app.draw(sBackground);
        app.draw(sBall);
        app.draw(sPaddle);

        for (auto& obj : blocks) {
            app.draw(obj.ret_sprite());
        }

        app.display();
    }
    return 0;
}