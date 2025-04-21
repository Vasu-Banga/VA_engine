#include "AudioDB.h"

std::unordered_map < std::string, Mix_Chunk* > AudioDB::audios;


void AudioDB::LoadAudio(const std::string& fileName) {
	std::string filePath = "resources/audio/" + fileName;
    if (!std::filesystem::exists(filePath + ".wav") && !std::filesystem::exists(filePath + ".ogg")) {
        std::cout << "error: failed to play audio clip " << fileName;
        exit(0);
    }

    //Open The Song
    audios[fileName] = AudioHelper::Mix_LoadWAV((std::filesystem::exists(filePath + ".wav")) ? (filePath + ".wav").c_str() : (filePath + ".ogg").c_str());

}

void AudioDB::PlayAudio(int channel, std::string audioName, bool loop) {
    if (!audios.count(audioName)) {
        LoadAudio(audioName);
    }
    AudioHelper::Mix_PlayChannel(channel, audios[audioName],loop * -1);
}

void AudioDB::Halt(int channel) {
    AudioHelper::Mix_HaltChannel(channel);
}

void AudioDB::SetVolume(int channel, float volume) {
    AudioHelper::Mix_Volume(channel, volume);
}