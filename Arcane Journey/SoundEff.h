#pragma once
#include<SFML/Audio.hpp>
class SoundEff
{
private:
	sf::SoundBuffer sbf;
	sf::Sound arr_sound;

public:
	SoundEff();
	~SoundEff();
	void Openintro(bool,bool);

};


