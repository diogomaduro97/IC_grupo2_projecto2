#include "AudioFile/AudioFile.h"
#include "./headers/Bitstream.h"
#include <math.h>

using namespace std;

#define MAX_SAMPLE_SIZE exp2(15)

class AudioGolomb
{
    private:
        AudioFile<double> audioFile;
        const char* encodedFilename;
        uint32_t mconst;
        uint32_t rMax;
        BitStream bifs, bofs;
    public:
        AudioGolomb(const char* f, uint32_t m, char op)
        {
            if(op == 'e')
            {
                audioFile.load(f);
                mconst = m;
                rMax = floor(log2(m-1));
            }
            else if(op == 'd')
            {
                encodedFilename = f;
                mconst = m;
                rMax = floor(log2(m-1));
            }
        }
        AudioGolomb(AudioFile<double> audio, uint32_t m, char op)
        {
            if(op == 'e')
            {
                audioFile = audio;
                mconst = m;
                rMax = floor(log2(m-1));
            }
            else if(op == 'd')
            {
                audioFile = audio;
                mconst = m;
                rMax = floor(log2(m-1));
            }
        }

        // Encode signed int
        void encode_int(int value)
        {
            int signal = 0;

            // Check if value is negative, invert value and change signal
            if(value < 0)
            {
                signal = 1;
                value = -value;    
            }

            // Golomb q and r
            uint16_t q = (uint16_t) floor((value/mconst));
            uint16_t r = value - q*mconst;
            
            // Write q bits
            for(int i = 0; i < q; i++){
                bifs.writeBit(0);
            }
            
            // Write 1 bit
            bifs.writeBit(1);

            // Write r bits
            bifs.writeInteger(r,rMax);

            // If value is negative, write an extra 1 bit, else write 0
            bifs.writeBit(signal);
        }

        int decode_int()
        {
            int bit = bofs.readBit();
            int q = 0;
            int flag = 0;
            int r;
            int signal = 0;

            while(bit != -2)
            {
                if(flag == 0)
                {
                    bit?flag=1:q++;
                }
                if(flag == 1)
                {
                    r = bofs.readInteger(rMax+1);
                    if(r == -2) break;
                    
                    signal = bofs.readBit();
                    int value = q*mconst+r;

                    if(signal == 1)
                    {
                        value = value*(-1);
                    }

                    return value;
                }
                bit = bofs.readBit();
            }
            return 0;

        }

        void losslessEncodeTo(const char* fout)
        {
            bifs.open(fout, 'w');

            // Get audioFile parameters
            int numChannels = audioFile.getNumChannels();
            int numSamples = audioFile.getNumSamplesPerChannel();
            int sampleRate = audioFile.getSampleRate();
            int bitDepth = audioFile.getBitDepth();

            // Encode audio parameters
            encode_int(numChannels);
            encode_int(numSamples);
            encode_int(sampleRate);
            encode_int(bitDepth);

            int current;
            int value;
            int lastSample;

            for (int channel = 0; channel < numChannels; channel++)
            {
                // Get and encode the first sample of each channel
                int initSample = (int)((audioFile.samples[channel][0]+1)*MAX_SAMPLE_SIZE);
                encode_int(initSample);

                for (int i = 1; i < numSamples; i++)
                {
                    // Get previous sample
                    lastSample = (int)((audioFile.samples[channel][i-1]+1)*MAX_SAMPLE_SIZE);

                    // Get current sample
                    current = (int)((audioFile.samples[channel][i]+1)*MAX_SAMPLE_SIZE);

                    // Value to encode is the last neighbour - current sample
                    value = lastSample - current;
                    encode_int(value);
                }
            }
        }

        void losslessDecodeTo(const char* fout)
        {
            bofs.open(encodedFilename, 'r');

            // Decode audioFile parameters 
            int numChannels = decode_int();
            int numSamples = decode_int();
            int sampleRate = decode_int();
            int bitDepth = decode_int();
            
            // Set previouly decoded parameters
            audioFile.setNumChannels(numChannels);
            audioFile.setNumSamplesPerChannel(numSamples);
            audioFile.setSampleRate(sampleRate);
            audioFile.setBitDepth(bitDepth);

            for (int channel = 0; channel < numChannels; channel++)
            {
                // Decode and convert the first sample of each channel
                int initSample = decode_int();
                audioFile.samples[channel][0] = (initSample/MAX_SAMPLE_SIZE)-1;
                
                for (int i = 1; i < numSamples; i++)
                {
                    // Get the last sample
                    int lastSample = (int)((audioFile.samples[channel][i-1]+1)*MAX_SAMPLE_SIZE);

                    // Decode de current computed value
                    int decoded_value = decode_int();

                    // The current sample is computed using the inverse of the encoding algorithm
                    // Value = sample_A - sample_B
                    // sample_B = sample_A - Value
                    int sample = lastSample - decoded_value;

                    // Convert back the sample value
                    audioFile.samples[channel][i] = (sample/MAX_SAMPLE_SIZE)-1;
                }
            }
            audioFile.save(fout);
        }

        AudioFile<double> decode()
        {
            bofs.open(encodedFilename, 'r');

            // Decode audioFile parameters 
            int numChannels = decode_int();
            int numSamples = decode_int();
            int sampleRate = decode_int();
            int bitDepth = decode_int();
            
            // Set previouly decoded parameters
            audioFile.setNumChannels(numChannels);
            audioFile.setNumSamplesPerChannel(numSamples);
            audioFile.setSampleRate(sampleRate);
            audioFile.setBitDepth(bitDepth);

            for (int channel = 0; channel < numChannels; channel++)
            {
                // Decode and convert the first sample of each channel
                int initSample = decode_int();
                audioFile.samples[channel][0] = (initSample/MAX_SAMPLE_SIZE)-1;
                
                for (int i = 1; i < numSamples; i++)
                {
                    // Get the last sample
                    int lastSample = (int)((audioFile.samples[channel][i-1]+1)*MAX_SAMPLE_SIZE);

                    // Decode de current computed value
                    int decoded_value = decode_int();

                    // The current sample is computed using the inverse of the encoding algorithm
                    // Value = sample_A - sample_B
                    // sample_B = sample_A - Value
                    int sample = lastSample - decoded_value;

                    // Convert back the sample value
                    audioFile.samples[channel][i] = (sample/MAX_SAMPLE_SIZE)-1;
                }
            }
            return audioFile;
        }
};

// int main() {
//     // File names
//     const char* fin = "./sample02.wav";
//     const char* fout = "./outAudioFile";
    
//     // Encode
//     AudioGolomb ge(fin, 128, 'e');
//     ge.losslessEncodeTo("outAudioFile");

//     // Decode
//     AudioGolomb gd(fout, 128, 'd');
//     gd.losslessDecodeTo("decoded_sample02.wav");
// }
