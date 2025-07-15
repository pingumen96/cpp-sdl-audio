#define SDL_MAIN_HANDLED  // disabilita il wrapping di SDL_main
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <windows.h>

#include <chrono>
#include <iostream>
#include <thread>  // per std::this_thread::sleep_for

#include "AudioSample.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd) {
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    AudioSample sample("data/test.wav");
    if (!sample.isValid()) {
        std::cerr << "AudioSample non valido!\n";
        SDL_Quit();
        return 1;
    }

    // Crea una finestra come prima
    SDL_Window* win =
        SDL_CreateWindow("Test SDL2", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (!win) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // --- Riproduzione Audio ---
    SDL_AudioSpec want = sample.spec();
    SDL_AudioSpec have;
    SDL_zero(have);

    // Apri device audio compatibile (NULL = default)
    SDL_AudioDeviceID dev =
        SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (dev == 0) {
        std::cerr << "SDL_OpenAudioDevice Error: " << SDL_GetError()
                  << std::endl;
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    std::cout << "Riproduco: " << sample.length()
              << " byte, freq: " << have.freq
              << " Hz, formato: " << static_cast<int>(have.format) << "\n";

    // Invia il buffer al device
    if (SDL_QueueAudio(dev, sample.data(), sample.length()) != 0) {
        std::cerr << "SDL_QueueAudio Error: " << SDL_GetError() << std::endl;
        SDL_CloseAudioDevice(dev);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_PauseAudioDevice(dev, 0);  // Avvia riproduzione

    // Attendi che l'audio finisca o max 10 secondi
    Uint32 ms = static_cast<Uint32>(
        1000.0 * sample.length() /
        (have.freq * (SDL_AUDIO_BITSIZE(have.format) / 8) * have.channels));
    ms = std::min(ms + 300, 10000u);  // aggiungi margine e non oltre 10s

    while (SDL_GetQueuedAudioSize(dev) > 0) {
        SDL_Delay(100);
    }

    // Facoltativo: mostra la finestra per altri 2 secondi
    SDL_Delay(2000);

    // Cleanup
    SDL_CloseAudioDevice(dev);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
