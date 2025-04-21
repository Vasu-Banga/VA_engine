#pragma once
#include <string>
#include "AudioHelper.h"
#include "SDL_mixer.h"
#include <filesystem>
#include <unordered_map>
#include <iostream>

class AudioDB
{
private:
	static std::unordered_map < std::string, Mix_Chunk* > audios;
public:
	static void LoadAudio(const std::string&);
	static void PlayAudio(int channel, std::string audioName, bool loop);
	static void Halt(int channel);
	static void SetVolume(int channel, float volume);
};

