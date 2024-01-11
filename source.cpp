#include <SFML/Graphics.hpp>
#include <time.h>
#include <vector>

using namespace sf;
using namespace std;

// 2 paddle과 공의 충돌을 좀 더 명확히 만들고 싶음
       // 벽돌이 처음에는 빨간 색 -> 한번 맞추면 초록색 -> 
int main()
{
    srand(time(nullptr));

    RenderWindow app(VideoMode(1280, 848), "Arkanoid!");
    app.setFramerateLimit(60);

    Texture t0, t1, t2, t3, t4;
    t0.loadFromFile("images/Arkanoid/block02.png");
    t1.loadFromFile("images/Arkanoid/block01.png"); // 42 X 20
    t2.loadFromFile("images/Arkanoid/whitewall.jpg"); // 1280 X 848
    t3.loadFromFile("images/Arkanoid/ball.png"); // 24 X 24
    t4.loadFromFile("images/Arkanoid/wall.png"); // 24 X 24

    Sprite sBackground(t2), sBall(t3), sPaddle(t4);
    sPaddle.setScale(6.0f, 1.0f);
    sPaddle.setPosition(620, 848 - 24);

    sf::FloatRect ballRect = sBall.getLocalBounds();
    sBall.setOrigin(ballRect.left + ballRect.width / 2.0f,
        ballRect.top + ballRect.height / 2.0f);
    sBall.setPosition(600, 600);


    const int default_block_count = 28 * 20;
    vector<Sprite> blocks(default_block_count);

    auto block_size = t1.getSize();

    for (int i = 0; i < 27; ++i) {
        for (int j = 0; j < 20; ++j) {
            blocks[i * 20 + j].setTexture(t1);
            blocks[i * 20 + j].setPosition((i + 1) * block_size.x + 24, (j + 1) * block_size.y + 24);
        }
    }

    Clock clock;

    float paddle_speed = 0.0f;
    float paddle_acceleration = 5.0f; // 가속도
    float paddle_max_speed = 20.0f;
    float Moving_default_speed = 5.0f;

    float rotate_val = 0.05f;

    sf::Clock rightClock; // 오른쪽 키 입력에 대한 Clock 객체
    sf::Clock leftClock; // 왼쪽 키 입력에 대한 Clock 객체

    float dx = 7, dy = -7;

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        auto ball_pos = sBall.getPosition();

        if (dx > 0) {
            sBall.rotate(-rotate_val);
        }
        else {
            sBall.rotate(rotate_val);
        }

        ball_pos.x += dx;

        for (int i = 0; i < blocks.size(); ++i) {
            if (dx > 0) {
                sBall.rotate(-rotate_val);
            }
            else {
                sBall.rotate(rotate_val);
            }
            if (FloatRect(ball_pos.x - 6, ball_pos.y - 6, 12, 12).intersects(blocks[i].getGlobalBounds()))
            {
                blocks.erase(blocks.begin() + i);
                dy = -dy;
            }
        }

        ball_pos.y += dy;

        for (int i = 0; i < blocks.size(); ++i) {
            if (FloatRect(ball_pos.x - 6, ball_pos.y - 6, 12, 12).intersects(blocks[i].getGlobalBounds()))
            {
                blocks.erase(blocks.begin() + i);
                dy = -dy;
            }
        }
        if (ball_pos.x < 0 || ball_pos.x> 1280)   dx = -dx; // ball : 24 X 24 픽셀
        if (ball_pos.y < 0 || ball_pos.y> 848 - 12) {
            dy = -dy;
            // 게임 패배
        }


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

        if (FloatRect(ball_pos.x - 12, ball_pos.y - 12, 24, 24).intersects(sPaddle.getGlobalBounds())) {
            if (dx > 0) {
                sBall.rotate(-rotate_val);
            }
            else {
                sBall.rotate(rotate_val);
            }
            dy = -dy;
        }

        sBall.setPosition(ball_pos.x, ball_pos.y);

        app.clear();
        app.draw(sBackground);
        app.draw(sBall);
        app.draw(sPaddle);

        for (auto& obj : blocks) {
            app.draw(obj);
        }

        app.display();
    }
    return 0;
}