#pragma once

#include <SDL_Mixer.h>
#include <map>
#include <vector>
#include <string>

class SDLAudio
{
private:
	SDLAudio();
	~SDLAudio();

public:
	static SDLAudio* Get();

public:
	void Init();

	void AddSoundFile(const std::string& soundID, const std::string& fileLocation);
	void AddMusicFile(const std::string& soundID, const std::string& fileLocation);

	bool IsSoundPlaying(const std::string& soundID);
	void PlaySFX(const std::string& soundID, float volume);
	void SetMusic(const std::string& soundID, float volume);
	void StopAll();

	void SetGlobalVolume(float newVol);

private:
	std::map<std::string, Mix_Chunk*> m_soundEffectFiles;
	std::map<std::string, Mix_Music*> m_musicFiles;
	std::map<std::string, Uint32> m_lastUsed; //find when a sound was last used

	float m_globalVolume;
};
