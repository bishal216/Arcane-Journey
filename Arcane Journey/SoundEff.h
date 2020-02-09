#pragma once
#include<SFML/Audio.hpp>
class SoundEff
{
private:

public:
	SoundEff();
	~SoundEff();
	static void GameSound()
	{
		sf::SoundBuffer sbf;
		sf::Sound arr_sound;
		sbf.loadFromFile("Sound/open_intro.wav");
		arr_sound.setBuffer(sbf);
		arr_sound.play();
	}
	static void ErrorSound()
	{
		sf::SoundBuffer sbf;
		sf::Sound arr_sound;
		sbf.loadFromFile("Sound/error.wav");
		arr_sound.setBuffer(sbf);
		arr_sound.play();
	}

};


