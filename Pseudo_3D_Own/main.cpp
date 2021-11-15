#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <vector>

sf::Vector2i WindowSize = sf::Vector2i(800, 592);
int cellHeight = 4;
int nSize = WindowSize.y / cellHeight;
std::vector<sf::RectangleShape> grass;
std::vector<sf::RectangleShape> road;
std::vector<sf::RectangleShape> borderLeft;
std::vector<sf::RectangleShape> borderRight;
//vector for track segments
std::vector<std::pair<float, float>> TrackVector;	//curvature, length

void init(float &, sf::Font &, sf::Text *speedDetails, sf::Text *lapDetails);
void update(sf::Color grassCol, sf::Color borderCol, float middle, float roadSize, float roadBorderLeft, float roadBorderRight, float roadBorderSize, int i);

int main()
{
	sf::Clock clk;
	sf::Time time;
	sf::RenderWindow window(sf::VideoMode(WindowSize.x, WindowSize.y), "Pseudo_3D", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	
	float distance = 0.f, speed = 0.f;
	float segmentDistance = 0.f, TotalDistance = 0.f;
	int segmentIndex = 0;
	float CurrentCurvature = 0.f, CurvatureDiff = 0.f, TrackCurvature = 0.f;
	float PlayerCurvature = 0.f, fCurvature = 0.f;	//this is curvatrue of a single particle
	float lapTime = 0;
	float dt = 0.f, timer = 0;
	sf::Text speedDetails[3];//speedText, YourSpeed, unit;
	sf::Sound theme;
	sf::SoundBuffer tbuf;
	sf::Text lapDetails[2];
	sf::Font font;
	font.loadFromFile("res/font.otf");
	tbuf.loadFromFile("res/sound_game.ogg");
	theme.setBuffer(tbuf);
	theme.setLoop(true);
	theme.play();

	init(TotalDistance, font, speedDetails, lapDetails);

	//car
	sf::Sprite car, mountain;
	sf::Texture carTexture, back;
	carTexture.loadFromFile("res/Car_Sprite_Sheet.png");
	car.setTexture(carTexture);
	car.setTextureRect(sf::IntRect(200, 175, 40, 25));
	car.setScale(4, 4);
	back.loadFromFile("res/back.png");
	mountain.setTexture(back);
	

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
		if (speed > 1.8)speed = 1.8;
		if (speed < 0)speed = 0;
		distance += speed * 100 * dt;

		//player curvature handling
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			PlayerCurvature -= 0.7*dt;
			car.setTextureRect(sf::IntRect(80, 175, 40, 25));
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			PlayerCurvature += 0.7*dt;
			car.setTextureRect(sf::IntRect(320, 175, 40, 25));
		}
		else
			car.setTextureRect(sf::IntRect(200, 175, 40, 25));
			
		if (fabs(PlayerCurvature - fCurvature) >= 0.6)
			speed -= 4 * dt;

		//Curvature handling
		segmentDistance = 0.f;
		segmentIndex = 0;
		for (auto k = TrackVector.begin(); k != TrackVector.end(); k++) {
			if (distance >= segmentDistance) {
				segmentDistance += TrackVector[segmentIndex].second;
				segmentIndex++;
			}
		}
		if (distance >= TotalDistance) {	//in a loop
			std::stringstream ss;
			ss << lapTime;
			lapDetails[1].setString(ss.str());
			lapTime = 0;
			distance -= TotalDistance;
		}
		lapTime += dt;

		TrackCurvature = TrackVector[segmentIndex - 1].first;
		CurvatureDiff = (TrackCurvature - CurrentCurvature)*dt*speed;
		CurrentCurvature += CurvatureDiff;

		fCurvature += CurrentCurvature * dt * speed;
		float posX = PlayerCurvature - fCurvature;
		car.setPosition(((WindowSize.x / 2) + (posX*WindowSize.x) / 2) - 70, WindowSize.y - 150);
		mountain.setPosition(((WindowSize.x / 2) - (CurrentCurvature*WindowSize.x) / 2) - mountain.getGlobalBounds().width/2, -200);
		
		std::stringstream s;
		int currSpeed = int((speed/2)*10000*dt);
		if (currSpeed < 0)
			currSpeed = 0;
		s << currSpeed;
		speedDetails[0].setString(s.str());
		speedDetails[0].setPosition(WindowSize.x - speedDetails[0].getGlobalBounds().width - 80, WindowSize.y - 160);

		for (int i = 0; i < nSize; i++) {

			float perspective = (float)i / (float)(nSize / 2);
			float middle = 0.5 + CurrentCurvature * pow((1.f - perspective), 3); //will be changed according to curvature and perspective
			float roadSize = 0.1 + perspective * 0.8;
			float roadBorderSize = roadSize * 0.15;
			roadSize = roadSize / 2;	//already made it half

			int roadBorderLeft = (middle - roadSize)*WindowSize.x;
			int roadBorderRight = (middle + roadSize)*WindowSize.x;

			sf::Color grassCol = sin(20 * pow(1 - perspective, 3) + distance * 0.1) > 0 ? sf::Color(2, 130, 6, 255) : sf::Color(2, 77, 4, 255);
			sf::Color roadBorderCol = sin(30 * pow(1 - perspective, 2) + distance * 0.5) > 0 ? sf::Color::Red : sf::Color::White;

			update(grassCol, roadBorderCol, middle, roadSize, roadBorderLeft, roadBorderRight, roadBorderSize, i);

		}


		window.clear();
		window.draw(mountain);
		for (auto i = grass.begin(); i != grass.end(); i++)
			window.draw(*i);
		for (auto i = road.begin(); i != road.end(); i++)
			window.draw(*i);
		for (auto i = borderLeft.begin(); i != borderLeft.end(); i++)
			window.draw(*i);
		for (auto i = borderRight.begin(); i != borderRight.end(); i++)
			window.draw(*i);
		window.draw(car);
		for (int i = 0; i < 3; i++)
			window.draw(speedDetails[i]);
		for (int i = 0; i < 2; i++)
			window.draw(lapDetails[i]);


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
	roadBorderSize = roadSize * 0.12;
	borderLeft[i].setFillColor(borderCol);
	borderLeft[i].setSize(sf::Vector2f(roadBorderSize, cellHeight));
	borderLeft[i].setPosition(roadBorderLeft, y);
	borderRight[i].setFillColor(borderCol);
	borderRight[i].setSize(sf::Vector2f(roadBorderSize, cellHeight));
	borderRight[i].setPosition(roadBorderRight - roadBorderSize, y);
}

void init(float &TotalDistance, sf::Font &font, sf::Text *speedDetails, sf::Text *lapDetails)
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

	for (int i = 0; i < 3; i++)
		speedDetails[i].setFont(font);
	speedDetails[0].setCharacterSize(30.f);

	speedDetails[1].setFont(font);
	speedDetails[1].setCharacterSize(35.f);
	speedDetails[1].setString("speed");
	speedDetails[1].setPosition(WindowSize.x - 130, WindowSize.y - 200);

	speedDetails[2].setFont(font);
	speedDetails[2].setCharacterSize(20.f);
	speedDetails[2].setString("KMH");
	speedDetails[2].setPosition(WindowSize.x - 70, WindowSize.y - 150);


	for (int i = 0; i < 2; i++) {
		lapDetails[i].setFont(font);
		lapDetails[i].setFillColor(sf::Color::Black);
		lapDetails[i].setCharacterSize(15.f);
		lapDetails[i].setPosition(10, i * 30 + 10);
	}
	lapDetails[0].setCharacterSize(20.f);
	lapDetails[0].setString("LAST LAP TIME");

	//Track segemnts
	TrackVector.push_back(std::make_pair(0.f, 10.f));	//start and end 
	TrackVector.push_back(std::make_pair(0.f, 400.f));
	TrackVector.push_back(std::make_pair(-0.2f, 200.f));
	TrackVector.push_back(std::make_pair(-0.5f, 300.f));
	TrackVector.push_back(std::make_pair(0.f, 300.f));
	TrackVector.push_back(std::make_pair(-0.5f, 400.f));
	TrackVector.push_back(std::make_pair(0.5f, 400.f));
	
	TrackVector.push_back(std::make_pair(0.f, 300.f));
	TrackVector.push_back(std::make_pair(0.4f, 300.f));
	TrackVector.push_back(std::make_pair(0.8f, 300.f));
	TrackVector.push_back(std::make_pair(-0.2, 400.f));
	TrackVector.push_back(std::make_pair(-0.6f, 300.f));
	TrackVector.push_back(std::make_pair(0.5f, 400.f));

	TrackVector.push_back(std::make_pair(0.8f, 300.f));
	TrackVector.push_back(std::make_pair(0.f, 200.f));
	TrackVector.push_back(std::make_pair(-0.8f, 300.f));

	TrackVector.push_back(std::make_pair(0.3f, 200.f));
	TrackVector.push_back(std::make_pair(-0.3f, 300.f));
	TrackVector.push_back(std::make_pair(0.6f, 300.f));
	TrackVector.push_back(std::make_pair(-0.4f, 300.f));
	TrackVector.push_back(std::make_pair(0.6f, 300.f));

	for (auto i : TrackVector)
		TotalDistance += i.second;
}