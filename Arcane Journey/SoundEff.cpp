#include "SoundEff.h"
#include<SFML/Audio.hpp>
SoundEff::SoundEff()
{
}

SoundEff::~SoundEff() {}

void SoundEff::Openintro(bool startclause,bool endclause)
{
	//Sound effects
	sbf.loadFromFile("Sound/open_intro.wav");
	arr_sound.setBuffer(sbf);
	if(startclause==true)
		arr_sound.play();
	if (endclause == true)
		arr_sound.pause();
}
