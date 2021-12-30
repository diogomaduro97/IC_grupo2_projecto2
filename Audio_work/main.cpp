#include <fstream>
#include <iostream>
#include <system_error>
#include "AudioFile/AudioFile.h"
#include <math.h> // somewhere earler (for M_PI and sinf())
#include <map>
#include <opencv2/opencv.hpp>
#include "AudioGolomb.cpp"

#define MAX_SAMPLE_SIZE exp2(15)
#define POW_MAX_SAMPLE_SIGNAL pow(MAX_SAMPLE_SIZE*2,2)
#define WAIT_KEY 1          
#define HISTO_WINDOWSIZE_Y 1000
#define HISTO_WINDOWSIZE_X 1500
#define RECTANGLE_DIVIDER 0
#define HISTO_SIZE 600
#define HISTO_NUMCOLLUMS 1
#define SHIFT_BITS 7

using namespace std;
using namespace cv;

map<double,int>::iterator it;
/* 
    this function generate a histogram from an audioFile   
    input args: 
        - AudioFile<double> audioFile             ->  Audio file to be computed 
    output:
        vector<map<double,int>> histo             ->  the histogram calculated
*/
vector<map<double,int>> generate_histogram(AudioFile<double> audio){
    vector<map<double,int>> histo;
    int numChannels = audio.getNumChannels();
    int numSamples = audio.getNumSamplesPerChannel();
    double tmp;
    for (int channel = 0; channel < numChannels; channel++){
        map<double,int> mapa;
        histo.push_back(mapa);
        for (int i = 0; i < numSamples; i++){
            tmp = audio.samples[channel][i];
            histo[channel][tmp * MAX_SAMPLE_SIZE]++;
        }
    }
    return histo;
}
/* 
    this function fils an Audiofile::buffer from an audiofile sample by sample
    and calculate at the same time a histogram of the samples 
    input args: 
        - AudioFile<double> audioFile             -> Audio file from a track 
    output:
        -tuple<                                         
            AudioFile<double>::AudioBuffer buffer -> return the buffer from the audiofile 
            vector<map<double,int>> histo         -> and the histogram calculated
        > 
*/
tuple<AudioFile<double>::AudioBuffer, vector<map<double,int>>> fillbuffer(AudioFile<double> audioFile){
    // Create an AudioBuffer 
    vector<map<double,int>> histo;
    AudioFile<double>::AudioBuffer buffer;
    int numSamples = audioFile.getNumSamplesPerChannel();
    int numChannels = audioFile.getNumChannels();

    buffer.resize(numChannels);

    // Set number of samples per channel,i
    // fill the buffer with samples, e.g.
    double tmp;
    for (int channel = 0; channel < numChannels; channel++){
        map<double,int> mapa;
        histo.push_back(mapa);
        buffer[channel].resize(numSamples);
        for (int i = 0; i < numSamples; i++){
            tmp = audioFile.samples[channel][i];
            histo[channel][tmp * MAX_SAMPLE_SIZE]++;
            buffer[channel][i] = tmp;        
        }
    }
    return make_tuple(buffer,histo);
}
/* 
    this function copy an Audiofile to another sample by sample
    and calculate at the same time a histogram of the samples using the fillbuffer() function  
    input args: 
        - AudioFile<double> audioFile      -> Audio file from a track 
    output:
        -tuple<                                         
            AudioFile<double> audioFile_out, -> return the buffer from the audiofile 
            vector<map<double,int>> histo,  -> and the histogram calculated 
            bool ok                         -> error value from the setAudioBuffer()
        > 
*/
tuple<AudioFile<double>,vector<map<double,int>>,bool> copyAudioFile(AudioFile<double> audioFile){
    AudioFile<double> audioFile_out;
    vector<map<double,int>> histo;
    AudioFile<double>::AudioBuffer buffer;
    tie(buffer,histo) = fillbuffer(audioFile);
    bool ok;
    ok = audioFile_out.setAudioBuffer(buffer);
    audioFile_out.setBitDepth(audioFile.getBitDepth());
    audioFile_out.setNumChannels(audioFile.getNumChannels());
    audioFile_out.setNumSamplesPerChannel(audioFile.getNumSamplesPerChannel());
    audioFile_out.setSampleRate(audioFile.getSampleRate());
    return make_tuple(audioFile_out,histo,ok);
}
/* 
    function to calculate the entropy of a histogram from an audio sample
    input args: 
        -vector<map<double,int>> histo -> the histogram calculated with the function fillbuffer()
        -int numChannels               -> number of channels 
        -int numSamples                -> number of samples of the audiofile 
    output:
        -vector<double> entropy        -> vector with the entropy calculated for each channel 
*/
vector<double> histoEntropy(vector<map<double,int>> histo,int numChannels, int numSamples){
    vector<double> entropy;
    for(int i = 0; i < histo.size(); i++){
        double tmp = 0;
        entropy.push_back(tmp);
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            double prob = (double)it->second/(double)numSamples;
            entropy[i] = entropy[i] - prob*log(prob);
        }
    }
    return entropy;
}
/* 
    this function fils an Audiofile::buffer from an audiofile 
    and calculate at the same time a histogram of the samples 
    input args: 
        - vector<map<double,int>> histo -> Histogram of the audiotrack  
        - vector<double> entropy        -> Entropy calculated based on the histogram 
    output:
        - Mat histogram_image ->  image's matrix of the histogram generated  
*/
Mat imageHisto(vector<map<double,int>> histo,vector<double> entropy, uint32_t rectangle_divider = RECTANGLE_DIVIDER){
    Mat histogram_image(HISTO_WINDOWSIZE_Y, HISTO_WINDOWSIZE_X,CV_8UC3, Scalar(0,0,0));

    for (int i = 0; i < histo.size(); i++){
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            rectangle(histogram_image, Point((HISTO_SIZE*i) +  2*((it->first+MAX_SAMPLE_SIZE)*500/(MAX_SAMPLE_SIZE*2) ) , histogram_image.rows - (it->second >> rectangle_divider)), 
                Point((HISTO_SIZE*i) + 2*((it->first+MAX_SAMPLE_SIZE)*500/(MAX_SAMPLE_SIZE*2) ), histogram_image.rows), Scalar(i == 1?0:255 ,i == 1?0:255,255));
        }
        const string toprint = to_string(entropy[i]);
        putText(histogram_image, "Entropy = " + toprint , Point(250 + (HISTO_SIZE*i) , 200), FONT_HERSHEY_COMPLEX_SMALL ,0.8, Scalar(i == 1?0:255 ,i == 1?0:255,255));
    }
    return histogram_image;
}
Mat snrOnHisto(Mat histo,vector<double> snr, int numChannels ){
    for (int i = 0; i < numChannels; i++){
        putText(histo, "Peak SignaltoNoise Ratio = " + to_string(snr[i]) , Point(250 + (HISTO_SIZE*i) , 240), FONT_HERSHEY_COMPLEX_SMALL ,0.8, Scalar(i == 1?0:255 ,i == 1?0:255,255));
    }
    return histo;
}
/* 
    this function generate a text file with the histogram\s
    input args: 
        - vector<map<double,int>> histo -> Histogram of the audiotrack  
        - int numChannels               -> number of channels 
*/

void histoToFile(vector<map<double,int>> histo,int numChannels, string name){
    // ofstream ofs("../Histograms/histo.txt");
    ofstream ofs("../Histograms/" + name + "_histo.txt");
    for(int i = 0; i< numChannels; i++){
        ofs << "canal numero: " << i << endl;
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            ofs<< it->first << " : " << it->second << endl;

        }
    }
}
/* 
    function to shift the value of each sample to the right
    input args: 
        - AudioFile<double> audioFile       -> Audio file from a track 
        - uint8_t bits                      -> number of bits to shift right 
    output:
        - AudioFile<double> audio_compress  -> Audio file with bits reduced 

*/

AudioFile<double> compress(AudioFile<double> audio, uint8_t bits=SHIFT_BITS){
    AudioFile<double> audioOut;
    audioOut.setBitDepth(audio.getBitDepth());
    audioOut.setNumChannels(audio.getNumChannels());
    audioOut.setNumSamplesPerChannel(audio.getNumSamplesPerChannel());
    audioOut.setSampleRate(audio.getSampleRate());
    audioOut.samples.resize(audio.getNumChannels());
    for(int c=0; c< audio.getNumChannels();c++){
        audioOut.samples[c].resize(audio.getNumSamplesPerChannel());
        for (int s = 0; s < audio.getNumSamplesPerChannel(); s++){
            int sample = (int)((audio.samples[c][s]+1)*MAX_SAMPLE_SIZE);
            double tmp = (double)(sample>>bits);
            audioOut.samples[c][s] = (tmp/(MAX_SAMPLE_SIZE))-1;
        }
    }
    
    return audioOut;
}


/* 
    function to shift the value of each sample to the left
    input args: 
        - AudioFile<double> audioFile       -> Audio file from a track 
        - uint8_t bits                      -> number of bits to shift right 
    output:
        - AudioFile<double> audio_decompress  -> Audio file with bits gained 

*/
AudioFile<double> decompress(AudioFile<double> audio, uint8_t bits=SHIFT_BITS){
    AudioFile<double> audioOut;
    audioOut.setBitDepth(audio.getBitDepth());
    audioOut.setNumChannels(audio.getNumChannels());
    audioOut.setNumSamplesPerChannel(audio.getNumSamplesPerChannel());
    audioOut.setSampleRate(audio.getSampleRate());
    
    audioOut.samples.resize(audio.getNumChannels()); 
    for(int c=0; c< audio.getNumChannels();c++){
        audioOut.samples[c].resize(audio.getNumSamplesPerChannel());
        for (int s = 0; s < audio.getNumSamplesPerChannel(); s++){
            int sample = (int)((audio.samples[c][s]+1)*MAX_SAMPLE_SIZE);
            double tmp = (double)(sample<<bits);
            audioOut.samples[c][s] = (tmp/(MAX_SAMPLE_SIZE))-1;
        }
    }
    return audioOut;
}

void lossyCompress(AudioFile<double> audio, const char* out_fname, uint8_t bits=SHIFT_BITS)
{
    AudioFile<double> audioOut;
    audioOut = compress(audio, bits);
    AudioGolomb ag(audioOut, 32, 'e');
    ag.losslessEncodeTo(out_fname);
}

AudioFile<double> lossyDecompress(const char* audioFilename, uint8_t bits=SHIFT_BITS)
{
    AudioGolomb ag(audioFilename, 32, 'd');
    AudioFile<double> audio = ag.decode();
    return decompress(audio, bits);
}

/* 
    function to calculate the peak of signal to noise between an original audio and the altered audio
    input args: 
        - AudioFile<double> audio1       -> Audio file Original 
        - AudioFile<double> audio2       -> Audio file Altered 
    output:
        -vector<double> psnr        -> vector with the psnr calculated for each channel 
*/
vector<double> signalToNoise(AudioFile<double> audio1, AudioFile<double> audio2){
    vector<double> sums;
    vector<double> psnr;
    for (int c = 0; c < audio1.getNumChannels(); c++){
        double sum = 0;
        sums.push_back(sum);
        for (int s = 0; s < audio1.getNumSamplesPerChannel(); s++){
                sums[c] = sums[c] + pow((audio1.samples[c][s] - audio2.samples[c][s])*MAX_SAMPLE_SIZE,2);
                // cout << ((audio1.samples[c][s] - audio2.samples[c][s]))*MAX_SAMPLE_SIZE<<endl;
        }
        double snr = 10*log10(POW_MAX_SAMPLE_SIGNAL*audio1.getNumSamplesPerChannel()/sums[c]);
        psnr.push_back(snr);
    }
    return psnr;
}

string fileName(string filename) {
    const size_t last_slash_idx = filename.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        filename.erase(0, last_slash_idx + 1);
    }

    // Remove extension if present.
    const size_t period_idx = filename.rfind('.');
        if (std::string::npos != period_idx)
        {
            filename.erase(period_idx);
        }
        return filename;
}

int main(int argc, char** argv) {

    string histoFile = fileName(argv[1]);

    const char* fileIn = argc > 1? argv[1]:"../Sounds/sample01.wav";
    const char* losslessFileOut = argc > 2? argv[2]:"../Sounds_Out/lossless_out.iclac";
    const char* lossyFileOut = argc > 2? argv[3]:"../Sounds_Out/lossy_out.wav";
    const char* losslessFileOutDecompressed = argc > 2? argv[4]:"../Sounds_Out/lossless_out_decompressed.wav";
    const char* lossyFileOutDecompressed = argc > 2? argv[5]:"../Sounds_Out/lossy_out_decompressed.wav";

    AudioFile<double> audioFile;
    audioFile.load(fileIn);
    int numChannels = audioFile.getNumChannels();

    AudioGolomb audioGolombCompress(fileIn, 512, 'e');
    audioGolombCompress.losslessEncodeTo(losslessFileOut);

    AudioGolomb audioGolombDecompress(losslessFileOut, 512, 'd');
    audioGolombDecompress.losslessDecodeTo(losslessFileOutDecompressed);

    // Create AudioFileOut
    AudioFile<double> audioFile_out; 
    vector<map<double,int>> histo;
    bool ok;
    tie(audioFile_out,histo,ok) = copyAudioFile(audioFile);
    vector<double> entropy = histoEntropy(histo,numChannels,audioFile.getNumSamplesPerChannel());
    histoToFile(histo,numChannels,histoFile);
    
    lossyCompress(audioFile, lossyFileOut);
    AudioFile<double> audio_out_decompressed = lossyDecompress(lossyFileOut);
    audio_out_decompressed.save(lossyFileOutDecompressed);
    vector<map<double,int>> histo_decompressed = generate_histogram(audio_out_decompressed);
    vector<double> entropy_decompressed = histoEntropy(histo_decompressed,numChannels,audioFile.getNumSamplesPerChannel());
    
    vector<double> psnr = signalToNoise(audioFile,audio_out_decompressed);
    for (int i = 0; i < numChannels; i++)
    {
        cout << psnr[i] << endl;
    }
    
    // printf("ficheiro de entrada : %s \n", fileIn);
    // audioFile.printSummary();
    // printf("ficheiro de saida : %s \n", fileOut);
    // audioFile_out.printSummary();

    Mat histo_image = imageHisto(histo,entropy);
    namedWindow("Histogram", WINDOW_AUTOSIZE );
    imshow("Histogram", histo_image);
    // imwrite("../Histograms/histo.jpg", histo_image);
    imwrite("../Histograms/" + histoFile + "_histo.jpg", histo_image);
    Mat histo_image_decompressed = snrOnHisto(imageHisto(histo_decompressed,entropy_decompressed),psnr,numChannels);
    namedWindow("Histogram Decompressed", WINDOW_AUTOSIZE);
    imshow("Histogram Decompressed", histo_image_decompressed);
    // imwrite("../Histograms/histo_decompressed.jpg", histo_image_decompressed);
    imwrite("../Histograms/" + histoFile + "_histoDecompressed.jpg", histo_image_decompressed);
    waitKey(WAIT_KEY);
    return 0;
}

