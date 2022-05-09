/*  Audio Template
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_AudioPipeline_AudioTemplate_H
#define PokemonAutomation_AudioPipeline_AudioTemplate_H

#include <cstddef>
#include <vector>
#include <QString>
#include "Common/Cpp/AlignedVector.h"

namespace PokemonAutomation{


// Hold the sepctrogram of an audio, used as a template to
// match sounds from an audio stream.
class AudioTemplate{
public:
    ~AudioTemplate();
    AudioTemplate();
    AudioTemplate(size_t frequencies, size_t windows);

    size_t numWindows() const{ return m_numWindows; }
    size_t numFrequencies() const{ return m_numFrequencies; }

    const float* getWindow(size_t windowIndex) const{
        return (const float*)((const char*)m_spectrogram.data() + windowIndex * m_bytes_per_spectrum);
    }
    float* getWindow(size_t windowIndex){
        return (float*)((char*)m_spectrogram.data() + windowIndex * m_bytes_per_spectrum);
    }

//    void scale(float s) { for(auto& v: m_spectrogram) v *= s; }

private:
    size_t m_numWindows = 0;
    size_t m_numFrequencies = 0;
    size_t m_bytes_per_spectrum;
    AlignedVector<float> m_spectrogram;
};

// Load AudioTemplate from disk. Accept .wav format on any OS.
// Loading .mp3 format however is dependent on Qt's platform-dependent backend.
AudioTemplate loadAudioTemplate(const QString& filename, size_t sampleRate = 48000);









}

#endif
