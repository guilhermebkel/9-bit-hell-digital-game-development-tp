#pragma once
#include <string>
#include <unordered_map>
#include "SDL_mixer.h"

class AudioSystem
{
public:
	AudioSystem();

	bool Initialize();
	void Shutdown();

	// Para efeitos sonoros
	void PlaySound(const std::string& fileName);

	// Para música de fundo
	void PlayMusic(const std::string& fileName, int loops = -1);
	void StopMusic();
	void SetMusicVolume(float volume);

private:
	// Mapeamento de nomes de eventos para dados de som
	std::unordered_map<std::string, Mix_Chunk*> mSounds;

	// Música de fundo atual
	Mix_Music* mMusic;
};