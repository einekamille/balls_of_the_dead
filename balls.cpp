#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#define resolution_x 500
#define resolution_y 500
#define num_of_enemy 70
#define gegner_radius 10
#define player_radius 10
#define path_to_music "./resources/music.ogg"


//Базовый класс для персонажей {
class Character
{
public:
	bool life;
    int health;
    double x_pos;
    double y_pos;
    double x_dir;
    double y_dir;
    int radius;
    
	Character(int _health, int _radius, double _xpos, double _ypos, double _xdir, double _ydir)
	{
		life = true;
		health = _health;
		radius = _radius;
		x_pos = _xpos;
		y_pos = _ypos;
		x_dir = _xdir;
		y_dir = _ydir;
	}
	
	void moving(float time)
    {
        if (x_pos + radius >= resolution_x)
        {
            x_pos = x_pos - radius / 2;
            x_dir = 0 - x_dir;
        }
        if (x_pos - radius <= 0)
        {
            x_pos = x_pos + radius / 2;
            x_dir = 0 - x_dir;
        }
        if (y_pos + radius >= resolution_y - 25)
        {
            y_pos = y_pos - radius / 2;
            y_dir = 0 - y_dir;
        }
        if (y_pos - radius <= 0)
        {
            y_pos = y_pos + radius / 2;
			y_dir = 0 - y_dir;
        }
        x_pos = x_pos + x_dir * 0.001 * time;
        y_pos = y_pos + y_dir * 0.001 * time;
    }
};

//Класс "игрок"
class Player : public Character
{
private:

public:
	int score;
    Player(int _health, int _radius, double _xpos, double _ypos, double _xdir, double _ydir) : Character(_health, _radius, _xpos, _ypos, _xdir, _ydir)
    {
		score = 0;
    }
};

//Класс "противник"
class Gegner : public Character
{
private:
public:
    Gegner(int _health, int _radius, double _xpos, double _ypos, double _xdir, double _ydir) : Character(_health, _radius, _xpos, _ypos, _xdir, _ydir)
    {
    }
    void movedit(double xpos, double ypos, double xdir, double ydir)
    {
        x_pos = xpos;
        y_pos = ypos;
        x_dir = xdir;
        y_dir = ydir;
    }
};

//Обработка cтолкновения двух персонажей
void gegner_coll(Gegner &enemy1, Gegner &enemy2)
{
    enemy2.x_dir = (enemy2.x_pos - enemy1.x_pos);
    enemy2.y_dir = (enemy2.y_pos - enemy1.y_pos);
    enemy1.x_dir = (enemy1.x_pos - enemy2.x_pos);
    enemy1.y_dir = (enemy1.y_pos - enemy2.y_pos);
}
//Главная функция
int main()
{	
	//Создание персонажей
    Player player(100, player_radius, 10, 10, 0, 0);	//Один игрок
    Gegner **enemy = new Gegner* [num_of_enemy];  //И куча врагов
    if (enemy == 0)
    {
		printf("Ошибка выделения памяти для enemy\n");
		return -1;
	}
    for (int i = 0; i < num_of_enemy; i++)
    {
		enemy[i] = new Gegner(100, gegner_radius, rand()%(resolution_x/2)+gegner_radius + resolution_x/4,
								rand()%(resolution_y/2)+gegner_radius + resolution_y/4,
								rand()%200-100, rand()%200-100);
		if (enemy == 0)
		{
			printf("Ошибка выделения памяти для enemy\n");
			for (int j = 0; j < i; j++) delete [] enemy[j];
			delete [] enemy;
			return -1;
		}
    }
	
	//Инициализируем и создаём объекты SFML
    sf::RenderWindow window(sf::VideoMode(resolution_x, resolution_y), "0001");  //Создаётся окно
    sf::ContextSettings settings;  //Инициализируются настроечки
    settings.antialiasingLevel = 8;  //Устанавливается уровень АА

    sf::Clock clock;	//Создаём таймер для регулировки КвС

	
	//Зафигачим музыкальное сопровождение
	sf::Music game_music;	
	if(!game_music.openFromFile(path_to_music)) return -1;
	game_music.setLoop(true);
	game_music.play();

	//Кружок для врага 
    sf::CircleShape shape(gegner_radius);
    shape.setFillColor(sf::Color::Red);
	
	//Кружок для врага, которо нужно достать
    sf::CircleShape shape_c(gegner_radius);
    shape_c.setFillColor(sf::Color::Cyan);

	//Кружок для убитого врага
    sf::CircleShape shape_d(gegner_radius);
    shape_d.setFillColor(sf::Color::Yellow);

	//Кружок для игрока
    sf::CircleShape shp(player_radius);
    shp.setFillColor(sf::Color::Green);

	//Прямоугольик для панели информации
    sf::RectangleShape bar(sf::Vector2f(resolution_x, 25));
    bar.setFillColor(sf::Color::Cyan);
    bar.setPosition(0, resolution_y - 25);

    //Далее текстовой информации
    //Загрузка шрифта
    sf::Font font;
    font.loadFromFile("resources/sansation.ttf");

	//Информация о количестве набранных очков
    sf::Text score_text("", font, 20);
    score_text.setColor(sf::Color::Black);
    score_text.setPosition(10, resolution_y - 23);
    char sc[50]; //сюда будет помещён сам текст

	//Оставшееся здоровье игрока 
    sf::Text health_text("", font, 20);
    health_text.setColor(sf::Color::Black);
    health_text.setPosition(200, resolution_y - 23);
    char hp[50]; //сюда будет помещён сам текст

    //Надпись "Game Over"
    sf::Text endspiel("GAME OVER", font, 70);
    endspiel.setColor(sf::Color::White);
    endspiel.setStyle(sf::Text::Bold);
    endspiel.setPosition(resolution_x/2 - 200, resolution_y/2 - 50);
	

    int current = num_of_enemy - 1;

    

    while (window.isOpen())   //main cycle
    {
        float time = clock.getElapsedTime().asMicroseconds();   //for fps-reg
        clock.restart();
        time = time / 800;
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
		player.x_dir = 0;
		player.y_dir = 0;
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
        {
            player.x_dir = -100;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        {
            player.x_dir = 100;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        {
            player.y_dir = -100;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
        {
            player.y_dir = 100;
        }
        window.clear();

        for (int i = 0; i < num_of_enemy; i++)   //calculate
        {
            enemy[i]->moving(time);
            if (i == current)
            {
                shape_c.setPosition(enemy[i]->x_pos - enemy[i]->radius, enemy[i]->y_pos - enemy[i]->radius);
                window.draw(shape_c);
            }
            else
            {
                if (enemy[i]->life)
                {
                    shape.setPosition(enemy[i]->x_pos - enemy[i]->radius, enemy[i]->y_pos - enemy[i]->radius);
                    window.draw(shape);
                }
                if (enemy[i]->life == false)
                {
                    shape_d.setPosition(enemy[i]->x_pos - enemy[i]->radius, enemy[i]->y_pos - enemy[i]->radius);
                    window.draw(shape_d);
                }
            }

            for (int j = i+1; j < num_of_enemy; j++)
            {
                if (sqrt((enemy[j]->x_pos - enemy[i]->x_pos)*(enemy[j]->x_pos - enemy[i]->x_pos) + (enemy[j]->y_pos - enemy[i]->y_pos)*(enemy[j]->y_pos - enemy[i]->y_pos)) < enemy[i]->radius + enemy[j]->radius)
                {

                    gegner_coll(*enemy[i], *enemy[j]);
                }
            }
            if (sqrt((player.x_pos - enemy[i]->x_pos)*(player.x_pos - enemy[i]->x_pos) + (player.y_pos - enemy[i]->y_pos)*(player.y_pos - enemy[i]->y_pos)) < enemy[i]->radius + player.radius)
            {
                enemy[i]->x_dir = (enemy[i]->x_pos - player.x_pos);
                enemy[i]->y_dir = (enemy[i]->y_pos - player.y_pos);
                enemy[i]->x_pos = enemy[i]->x_pos + enemy[i]->x_dir/10;
                enemy[i]->y_pos = enemy[i]->y_pos + enemy[i]->y_dir/10;

                if ((i == current) && (enemy[i]->life))
                {
                player.score += 10;
                enemy[i]->life = false;
                current--;
                } else if (enemy[i]->life) player.health--;
            }
        }

        shp.setPosition(player.x_pos - player.radius, player.y_pos - player.radius);
        window.draw(shp);
        if (player.health <= 0)
        {
            player.life = false;
            window.draw(endspiel);
        }
        if (player.life) player.moving(time);

        sprintf(hp, "Health: %d ", player.health);
        sprintf(sc, "Score: %d ", player.score);
        score_text.setString(sc);
        health_text.setString(hp);
        window.draw(bar);
        window.draw(health_text);
        window.draw(score_text);
        window.display();
    }
    for (int i = 0; i < num_of_enemy; i++) delete [] enemy[i];
    delete [] enemy;
    return 0;
}
