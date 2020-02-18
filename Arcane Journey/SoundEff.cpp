#include "SoundEff.h"
#include<SFML/Audio.hpp>
SoundEff::SoundEff()
{
}

SoundEff::~SoundEff() {}

void SoundEff::GameSound()
{
	//Sound effects
	sf::SoundBuffer sbf;
	sf::Sound arr_sound;
	sbf.loadFromFile("Sound/open_intro.wav");
	arr_sound.setBuffer(sbf);
	arr_sound.play();

}

void SoundEff::ErrorSound()
{
	sf::SoundBuffer sbf;
	sf::Sound arr_sound;
	sbf.loadFromFile("Sound/error.wav");
	arr_sound.setBuffer(sbf);
	arr_sound.play();
}
