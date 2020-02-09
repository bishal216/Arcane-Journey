#include<SFML/Graphics.hpp>
class OpenAnim
{
private:
	sf::RenderWindow& m_window;
	int &stateOA;
	float xoff = -1920.0f, yoff = -1080.0f, alpha = 0;

public:
	//Constructor
	OpenAnim(sf::RenderWindow& window,int* state) :m_window(window),stateOA(*state){};
	void loadintro( float x);
	
};

