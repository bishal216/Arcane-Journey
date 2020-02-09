include "SoundEff.h"
#include<SFML/Audio.hpp>
SoundEff::SoundEff()
{
}

SoundEff::~SoundEff() {}

static void SoundEff::GameSound()
{
	//Sound effects
	sbf.loadFromFile("Sound/open_intro.wav");
	arr_sound.setBuffer(sbf);
		arr_sound.play();
}

static void SoundEff::ErrorSound()
{
	sbf.loadFromFile("Sound/error.wav");
	arr_sound.setBuffer(sbf);
	arr_sound.play();
}
