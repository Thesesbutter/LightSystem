#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <GL/gl.h>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>
#include <LightSystem/LightSystem.hpp>
#include <LightSystem/SpotLight.hpp>

#define WIDTH   640
#define HEIGHT  480

//font taken from http://www.fontspace.com/melifonts/sweet-cheeks
int main(int argc, char** argv) {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "LightSystem test");

    bool debugLightMapOnly = false;
    bool aabb = false;
    bool debugUseShader = true;

    bool update = false;
    //bg
    sf::Texture bg;
    if(!bg.loadFromFile("data/map.png")) exit(-1);
    //if(!bg.loadFromFile("data/map2.jpg")) exit(-1);
    //bg.setRepeated(true);
    sf::Sprite bgSpr(bg,sf::IntRect(0,0,WIDTH,HEIGHT));
    bgSpr.setOrigin(sf::Vector2f(WIDTH/2,HEIGHT/2));

    int fps = 0;
    int elapsedFrames = 0;
    sf::Clock clock, pclock;

    sf::Font font;

    if(!font.loadFromFile("data/Sweet Cheeks.ttf")) exit(-1); //because yes

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setPosition(580,10);
    text.setString("0");
    text.setColor(sf::Color::White);

    sf::View view;
    view.setSize(sf::Vector2f(WIDTH,HEIGHT));

    sf::RectangleShape p(sf::Vector2f(10,10));
    p.setFillColor(sf::Color::Blue);
    p.setPosition(sf::Vector2f(1680,2090));
    p.setOrigin(5,5);

    int speed = 5;

    DMGDVT::LS::LightSystem ls;
    ls.setAmbiantLight(sf::Color(15,0,60));
    ls.setView(view);
    //1679,1583                                                                      radius              DA         SA         I    B    LF
    DMGDVT::LS::SpotLight* spot =  new DMGDVT::LS::SpotLight(sf::Vector2f(1678,1582),200,sf::Color::Red, 0.0f       ,M_PIf*2.0f,1.0f,0.5f,1.0f);
    DMGDVT::LS::SpotLight* spot2 = new DMGDVT::LS::SpotLight(sf::Vector2f(1778,1417),200,sf::Color::Blue,0.0f       ,M_PIf*2.0f,1.0f,0.5f,1.0f);
    DMGDVT::LS::SpotLight* spot3 = new DMGDVT::LS::SpotLight(sf::Vector2f(1878,1582),200,sf::Color::Green,0.0f      ,M_PIf*2.0f,1.0f,0.5f,1.0f);

    DMGDVT::LS::SpotLight* spot4 = new DMGDVT::LS::SpotLight(sf::Vector2f(1520,1871),300,sf::Color::White,-M_PIf/4.0f ,M_PIf/5.0f,0.5f,1.0f,1.5f);
    DMGDVT::LS::SpotLight* spot5 = new DMGDVT::LS::SpotLight(sf::Vector2f(1840,1871),300,sf::Color::White,M_PIf/4.0f ,M_PIf/5.0f,0.5f,1.0f,1.5f);

    /*template add example*/ls.addLight<DMGDVT::LS::SpotLight>(sf::Vector2f(1679,2200),800,sf::Color(250,95,20),M_PIf ,M_PIf/3.0f,1.0f,0.0f,2.0f);
    //1679,1583                                                                      radius              DA              SA  I    B    LF
    DMGDVT::LS::SpotLight* playerLight = new DMGDVT::LS::SpotLight(p.getPosition(),200,sf::Color::White);

    playerLight->setLinearity(2.0f);
    playerLight->setBleed(0.0f);
    playerLight->setSpreadAngle(M_PIf/3.0f);
    playerLight->setDirectionAngle(M_PIf);

    ls.addLight(spot);
    ls.addLight(spot2);
    ls.addLight(spot3);
    ls.addLight(spot4);
    ls.addLight(spot5);
    ls.addLight(playerLight);

    sf::Vector2i mouseInt = sf::Mouse::getPosition(window);
    sf::Vector2f mouse(window.mapPixelToCoords(mouseInt));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::KeyPressed){
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                    {
                        window.close();
                    } break;
                    case sf::Keyboard::Up :
                    {
                        p.move(0,-speed);
                    } break;
                    case sf::Keyboard::Down :
                    {
                        p.move(0,speed);
                    } break;
                    case sf::Keyboard::Left :
                    {
                        p.move(-speed,0);
                    } break;
                    case sf::Keyboard::Right :
                    {
                        p.move(speed,0);
                    } break;
                    case sf::Keyboard::F1 :
                    {
                        aabb = !aabb;
                    } break;
                    case sf::Keyboard::F2 :
                    {
                        debugLightMapOnly = !debugLightMapOnly;
                    } break;
                    case sf::Keyboard::F3 :
                    {
                        debugUseShader = !debugUseShader;
                    } break;
                    default: break;
                }
            }
        }

        mouseInt = sf::Mouse::getPosition(window);
        mouse = window.mapPixelToCoords(mouseInt);

        playerLight->setDirectionAngle(DMUtils::sfml::getAngleBetweenVectors(sf::Vector2f(0.0f,1.0f),mouse-sf::Vector2f(WIDTH/2.0f,HEIGHT/2.0f)));

        playerLight->setPosition(p.getPosition());
        if(update) ls.update(playerLight);
        update = false;

        int x = p.getPosition().x-WIDTH/2;
        int y = p.getPosition().y-HEIGHT/2;

        bgSpr.setTextureRect(sf::IntRect(x,y,WIDTH,HEIGHT));
        bgSpr.setPosition(p.getPosition());

        window.clear();

        sf::View baseView = window.getView();

            view.setCenter(p.getPosition());
            window.setView(view);
            window.draw(bgSpr);
            window.draw(p);

            int flags = 0;
            if(debugLightMapOnly) flags |= DMGDVT::LS::LightSystem::DEBUG_FLAGS::LIGHTMAP_ONLY;
            if(!debugUseShader) flags |= DMGDVT::LS::LightSystem::DEBUG_FLAGS::SHADER_OFF;
            ls.debugRender(view,window,flags);
            //use ls.render if not using debug
            ls.draw(view,window);
            if(aabb) ls.drawAABB(view,window);

        window.setView(baseView);

        window.draw(text);
        window.display();

        //sf::sleep(sf::milliseconds(16));
        ++elapsedFrames;
        if(clock.getElapsedTime().asMilliseconds() > 500) {
            fps = elapsedFrames;
            elapsedFrames = 0;
            clock.restart();
            //for some reason my compiler doesn't find to_string so..
            std::ostringstream str;
            str << (fps*2);
            text.setString(str.str());
        }
    }

    return 0;
}
