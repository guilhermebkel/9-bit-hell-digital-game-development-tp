#include "AudioSystem.h"
#include <SDL.h>

AudioSystem::AudioSystem()
	: mMusic(nullptr)
{
}

bool AudioSystem::Initialize()
{
	// Inicializa o SDL_mixer
	// Frequência, formato, canais (2 para estéreo), tamanho do chunk
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		SDL_Log("Failed to initialize SDL_mixer: %s", Mix_GetError());
		return false;
	}
	return true;
}

void AudioSystem::Shutdown()
{
	// Libera a música
	StopMusic();

	// Libera todos os efeitos sonoros
	for (auto& sound : mSounds)
	{
		Mix_FreeChunk(sound.second);
	}
	mSounds.clear();

	// Fecha o SDL_mixer
	Mix_CloseAudio();
}

void AudioSystem::PlayMusic(const std::string& fileName, int loops)
{
	// Para a música anterior se houver
	StopMusic();

	// Carrega a nova música
	mMusic = Mix_LoadMUS(fileName.c_str());
	if (!mMusic)
	{
		SDL_Log("Failed to load music %s: %s", fileName.c_str(), Mix_GetError());
		return;
	}

	// Toca a música (loops = -1 para loop infinito)
	if (Mix_PlayMusic(mMusic, loops) == -1)
	{
		SDL_Log("Failed to play music %s: %s", fileName.c_str(), Mix_GetError());
	}
}

void AudioSystem::PlaySound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;

	// Tenta encontrar o som no cache
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		// Carrega o som e o adiciona ao cache
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("Failed to load sound %s: %s", fileName.c_str(), Mix_GetError());
			return;
		}
		mSounds.emplace(fileName, chunk);
	}

	// Toca o som no primeiro canal disponível
	Mix_PlayChannel(-1, chunk, 0);
}

void AudioSystem::StopMusic()
{
	if (mMusic)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(mMusic);
		mMusic = nullptr;
	}
}

void AudioSystem::SetMusicVolume(float volume)
{
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;
	
	int sdlVolume = static_cast<int>(volume * MIX_MAX_VOLUME);
	Mix_VolumeMusic(sdlVolume);
}