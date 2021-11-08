#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <vector>

sf::Vector2i WindowSize = sf::Vector2i(800, 592);
int cellHeight = 8;
int nSize = WindowSize.y / cellHeight;
std::vector<sf::RectangleShape> grass;
std::vector<sf::RectangleShape> road;
std::vector<sf::RectangleShape> borderLeft;
std::vector<sf::RectangleShape> borderRight;

void init(void);
void update(sf::Color grassCol, sf::Color borderCol, float middle, float roadSize, float roadBorderLeft, float roadBorderRight, float roadBorderSize, int i);

int main()
{
	sf::Clock clk;
	sf::Time time;
	sf::RenderWindow window(sf::VideoMode(WindowSize.x, WindowSize.y), "Test", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	float dt = 0.f;
	float distance = 0.f;

	init();

	while (window.isOpen())
	{
		time = clk.restart();
		dt = time.asSeconds();

		sf::Event e;
		while (window.pollEvent(e)) {
			switch (e.type)
			{
			case sf::Event::Closed:
				window.close();
			case sf::Event::KeyPressed:
				if (e.key.code == sf::Keyboard::W)
					distance += 100.f*dt;
			default:
				break;
			}
		}


		for (int i = 0; i < nSize; i++) {

			float perspective = (float)i / (float)(nSize / 2);
			float middle = 0.5;
			float roadSize = 0.2 + perspective * 0.9;
			float roadBorderSize = roadSize * 0.15;
			roadSize = roadSize / 2;	//already made it half

			int roadBorderLeft = (middle - roadSize)*WindowSize.x;
			int roadBorderRight = (middle + roadSize)*WindowSize.x;

			sf::Color grassCol = sin(20 * pow(1 - perspective, 3) + distance * 0.1) > 0 ? sf::Color(0, 102, 0, 255) : sf::Color::Green;
			sf::Color roadBorderCol = sin(80 * pow(1 - perspective, 2) + distance) > 0 ? sf::Color::Red : sf::Color::White;

			update(grassCol, roadBorderCol, middle, roadSize, roadBorderLeft, roadBorderRight, roadBorderSize, i);

		}


		window.clear();
		for (auto i = grass.begin(); i != grass.end(); i++)
			window.draw(*i);
		for (auto i = road.begin(); i != road.end(); i++)
			window.draw(*i);
		for (auto i = borderLeft.begin(); i != borderLeft.end(); i++)
			window.draw(*i);
		for (auto i = borderRight.begin(); i != borderRight.end(); i++)
			window.draw(*i);
		window.display();

	}

}

void update(sf::Color grassCol, sf::Color borderCol, float middle, float roadSize, float roadBorderLeft, float roadBorderRight, float roadBorderSize, int i)
{
	int y = (WindowSize.y / 2) + (i * cellHeight);
	//grass
	grass[i].setPosition(0, y);
	grass[i].setFillColor(grassCol);
	//road
	int roadX = (middle - roadSize)*WindowSize.x;
	roadSize = roadSize * WindowSize.x * 2;
	road[i].setSize(sf::Vector2f(roadSize, cellHeight));
	road[i].setPosition(roadX, y);
	//road border
	roadBorderSize = roadSize * 0.15;
	borderLeft[i].setFillColor(borderCol);
	borderLeft[i].setSize(sf::Vector2f(roadBorderSize, cellHeight));
	borderLeft[i].setPosition(roadBorderLeft, y);
	borderRight[i].setFillColor(borderCol);
	borderRight[i].setSize(sf::Vector2f(roadBorderSize, cellHeight));
	borderRight[i].setPosition(roadBorderRight - roadBorderSize, y);
}



void init()
{
	sf::RectangleShape temp;
	for (int i = 0; i < nSize; i++) {
		road.push_back(temp);
		borderLeft.push_back(temp);
		borderRight.push_back(temp);
	}

	temp.setSize(sf::Vector2f(WindowSize.x, cellHeight));
	for (int i = 0; i < nSize; i++)
		grass.push_back(temp);
}