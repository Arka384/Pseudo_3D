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
//vector for track segments
std::vector<std::pair<float, float>> TrackVector;	//curvature, length

void init(float &);
void update(sf::Color grassCol, sf::Color borderCol, float middle, float roadSize, float roadBorderLeft, float roadBorderRight, float roadBorderSize, int i);
void updateCarSprite(float PlayerCurvature, float fCurvature, sf::Sprite &car, sf::Texture carTex);

int main()
{
	sf::Clock clk;
	sf::Time time;
	sf::RenderWindow window(sf::VideoMode(WindowSize.x, WindowSize.y), "Pseudo_3D", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	float dt = 0.f;
	float distance = 0.f, speed = 0.f;
	float segmentDistance = 0.f, TotalDistance = 0.f;
	int segmentIndex = 0;
	float CurrentCurvature = 0.f, CurvatureDiff = 0.f, TrackCurvature = 0.f;
	float PlayerCurvature = 0.f, fCurvature = 0.f;	//this is curvatrue of a single particle

	init(TotalDistance);

	//car
	sf::Sprite car;
	sf::Texture carTexture;
	carTexture.loadFromFile("Car_Sprite_Sheet.png");
	car.setTexture(carTexture);
	car.setTextureRect(sf::IntRect(200, 175, 40, 25));
	car.setScale(4, 4);


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
			default:
				break;
			}
		}

		//////////////////////////
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))	speed += 2 * dt;
		else	speed -= 1 * dt;
		//clamping of speed
		if (speed > 1)speed = 1;
		if (speed < 0)speed = 0;
		distance += speed * 100 * dt;

		//player curvature handling
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			PlayerCurvature -= 0.7*dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			PlayerCurvature += 0.7*dt;
		if (fabs(PlayerCurvature - fCurvature) >= 0.7)
			speed -= 4 * dt;
		updateCarSprite(PlayerCurvature, fCurvature, car, carTexture);

		//Curvature handling
		segmentDistance = 0.f;
		segmentIndex = 0;
		for (auto k = TrackVector.begin(); k != TrackVector.end(); k++) {
			if (distance >= segmentDistance) {
				segmentDistance += TrackVector[segmentIndex].second;
				segmentIndex++;
			}
		}
		if (distance >= TotalDistance)	//in a loop
			distance -= TotalDistance;

		TrackCurvature = TrackVector[segmentIndex - 1].first;
		CurvatureDiff = (TrackCurvature - CurrentCurvature)*dt*speed;
		CurrentCurvature += CurvatureDiff;

		fCurvature += CurrentCurvature * dt * speed;
		float posX = PlayerCurvature - fCurvature;
		car.setPosition(((WindowSize.x / 2) + (posX*WindowSize.x) / 2) - 70, WindowSize.y - 150);


		for (int i = 0; i < nSize; i++) {

			float perspective = (float)i / (float)(nSize / 2);
			float middle = 0.5 + CurrentCurvature * pow((1.f - perspective), 3); //will be changed according to curvature and perspective
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
		window.draw(car);

		window.display();

	}

}

void update(sf::Color grassCol, sf::Color borderCol, float middle, float roadSize, float roadBorderLeft, float roadBorderRight, float roadBorderSize, int i)
{
	//all my clumsy update things 
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

void updateCarSprite(float PlayerCurvature, float fCurvature, sf::Sprite &car, sf::Texture carTex)
{
	float diff = PlayerCurvature - fCurvature;
	if (diff < 0) {
		if(fabs(diff) < 0.1)
			car.setTextureRect(sf::IntRect(200, 175, 40, 25));
		else if (fabs(diff) < 0.2)
			car.setTextureRect(sf::IntRect(121, 175, 38, 25));
		else if (fabs(diff) < 0.3)
			car.setTextureRect(sf::IntRect(80, 175, 40, 25));
		else
			car.setTextureRect(sf::IntRect(35, 175, 45, 25));
	}
	else if (diff > 0) {
		if (fabs(diff) < 0.1)
			car.setTextureRect(sf::IntRect(200, 175, 40, 25));
		else if (fabs(diff) < 0.2)
			car.setTextureRect(sf::IntRect(280, 175, 38, 25));
		else if (fabs(diff) < 0.3)
			car.setTextureRect(sf::IntRect(320, 175, 40, 25));
		else
			car.setTextureRect(sf::IntRect(360, 175, 45, 25));
	}
}

void init(float &TotalDistance)
{
	sf::RectangleShape temp;
	temp.setFillColor(sf::Color(128, 128, 128, 255));
	for (int i = 0; i < nSize; i++) {
		road.push_back(temp);
		borderLeft.push_back(temp);
		borderRight.push_back(temp);
	}

	temp.setSize(sf::Vector2f(WindowSize.x, cellHeight));
	for (int i = 0; i < nSize; i++)
		grass.push_back(temp);

	//Track segemnts
	TrackVector.push_back(std::make_pair(0.f, 400.f));
	TrackVector.push_back(std::make_pair(-0.2, 100.f));
	TrackVector.push_back(std::make_pair(-0.5, 200.f));
	TrackVector.push_back(std::make_pair(-0.8, 200.f));
	TrackVector.push_back(std::make_pair(0.f, 300.f));
	TrackVector.push_back(std::make_pair(-1.f, 100.f));
	TrackVector.push_back(std::make_pair(0.f, 300.f));
	TrackVector.push_back(std::make_pair(0.2, 200.f));
	TrackVector.push_back(std::make_pair(0.5, 200.f));
	TrackVector.push_back(std::make_pair(0.2, 200.f));
	TrackVector.push_back(std::make_pair(1.f, 100.f));
	TrackVector.push_back(std::make_pair(0.f, 100.f));

	for (auto i : TrackVector)
		TotalDistance += i.second;
}