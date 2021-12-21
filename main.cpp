#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <map>
#include <math.h>
#include "headers/GolombCode.h"
using namespace cv;
using namespace std;
#define WAIT_KEY 1          //0 para imagem e 1 para video
#define COLOR_CHANNELS 3    
#define IMG_COLOR -1
#define HISTO_BITSCAPTURE 1 // quanto maior o numero menos pixeis sao contados para o hitograma, mais rapida e a captura de ecra
#define HISTO_WINDOWSIZE_Y 1000
#define HISTO_WINDOWSIZE_X 1920
#define HISTO_SIZE 510      // 
#define RECTANGLE_DIVIDER 2 // quanto maior o numero menor o tamanho do rectagulo
#define SHIFT_BITS 2        // [0..7] qunato bits se filtram por cada pixel 
#define FOOTER 150
map<short,int>::iterator it;
/* 
    this function generate a histogram from an audioFile   
    input args: 
        - ;at image                     ->  Image file to be computed 
    output:
        - vector<map<short,int>> histo  ->  the histogram calculated
*/
vector<map<short,int>> createHistogram(Mat image){
    vector<map<short,int>> histo;
    for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
        map<short,int> mapa;
        histo.push_back(mapa);
        for (int r = 0; r < image.rows; r++){
            for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
                short temp = image.at<Vec3b>(r,c)[channel];            
                // cout << image.at<Vec3b>(r,c)[i] << ",";
                histo[channel][temp] = histo[channel][temp] + HISTO_BITSCAPTURE;
            }
            // cout << "(" << image.at<Vec3b>(r,c) <<") | ";
        }
        // cout << ";" <<endl;
    }
    return histo;
}
/* 
    this function generate a text file with the histogram\s
    input args: 
        - vector<map<double,int>> histo -> Histogram of the image 
*/
void histoToFile(string file, vector<map<short,int>> histo){
    ofstream ofs(file);
    for(int i = 0; i< COLOR_CHANNELS; i++){
        ofs << "canal numero: " << i << endl;
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            ofs<< it->first << " : " << it->second << endl;

        }
    }

}
/* 
    this function generate an image based on a histogram and his entropy
    input args: 
        - vector<map<short,int>> histo -> Histogram of the audiotrack  
        - vector<double> entropy        -> Entropy calculated based on the histogram 
    output:
        - Mat histogram_image ->  image's matrix of the histogram generated  
*/
// int values[6] = [0,50,100,150,200,255];
Mat imageHisto(vector<map<short,int>> histo,vector<double> entropy,uint8_t ractangle_divider = RECTANGLE_DIVIDER){
    Mat histogram_image(HISTO_WINDOWSIZE_Y, HISTO_WINDOWSIZE_X,CV_8UC3, Scalar(0,0,0));
    
    for (int i = 0; i < histo.size(); i++){
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            rectangle(histogram_image, 
                Point((HISTO_SIZE*i) + 2*it->first, histogram_image.rows - (it->second >> ractangle_divider) - FOOTER), 
                Point((HISTO_SIZE*i) + 2*(it->first+1), histogram_image.rows - FOOTER ),
                i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255));
            // putText(histogram_image,);    
        }
        const string toprint = to_string(entropy[i]);
        putText(histogram_image, toprint , Point(250 + (HISTO_SIZE*i) , 200), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
        putText(histogram_image, "entropy : " , Point(150 + (HISTO_SIZE*i) , 200), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
    }
    return histogram_image;
}
/* 
    this function add the snr value to the generated histogram image 
    input args:   
        - vector<double> snr  -> Snr calculated from an altered image 
    output:
        - Mat histogram_image ->  image's matrix of the histogram generated with snr value added  
*/
Mat snrOnHisto(Mat histo,vector<double> snr ){
    for (int i = 0; i < COLOR_CHANNELS; i++){
        const string toprint = to_string(snr[i]);
        putText(histo, toprint , Point(250 + (HISTO_SIZE*i) , 300), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
        putText(histo, "snr : " , Point(175 + (HISTO_SIZE*i) , 300), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
    }
    return histo;
}
/* 
    function to calculate the entropy of a histogram from an Image
    input args: 
        -vector<map<short,int>> histo -> the histogram calculated with the function createHIstogram 
        -int Sample_size                -> number of samples of the audiofile 
    output:
        -vector<double> entropy        -> vector with the entropy calculated for each color
*/
vector<double> histoEntropy(vector<map<short,int>> histo, int sample_size ){
    vector<double> entropy;
    for(int i = 0; i < histo.size(); i++){
        double tmp = 0;
        entropy.push_back(tmp);
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            double prob = (double)it->second/((double)sample_size*HISTO_BITSCAPTURE);
            entropy[i] = entropy[i] - prob*log(prob);
        }
    }
    return entropy;
}
Mat quatization(Mat image, int color = IMG_COLOR,uint8_t bits = SHIFT_BITS){
    Mat image_quantizated(image.rows, image.cols,CV_8UC3,Scalar(0,0,0));
    for (int r = 0; r < image.rows; r++){
            for (int c = 0; c < image.cols; c++){
                for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                    image_quantizated.at<Vec3b>(r,c)[channel] = image.at<Vec3b>(r,c)[channel]>>bits;            
                    image_quantizated.at<Vec3b>(r,c)[channel] = image.at<Vec3b>(r,c)[channel]<<bits;            
                }
            }

        }
    return image_quantizated;     
}
/* 

    function to shift the value of each pixel to the right
    input args: 
        - Mat Image             -> Image file from a track 
        - uint8_t bits          -> number of bits to shift right 
    output:
        - Mat audio_decompress  -> Audio file with bits reduced 

*/
Mat compress(Mat image, int color = IMG_COLOR ,uint8_t bits=SHIFT_BITS){
    Mat image_compressed(image.rows,image.cols,CV_8UC3,Scalar(0,0,0));
    // if(color != 0){
        for (int r = 0; r < image.rows; r++){
            for (int c = 0; c < image.cols; c++){
                for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                    image_compressed.at<Vec3b>(r,c)[channel] = image.at<Vec3b>(r,c)[channel]>>bits;            
                }
            }
        }
    // }else{
    //     for (int r = 0; r < image.rows; r++){
    //         for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
    //             image_compressed.at<uint32_t>(r,c) = image.at<uint32_t>(r,c)>> bits;                            
    //         }
    //     }
    // }
    return image_compressed;
}
/* 
    function to shift the value of each pixel to the left
    input args: 
        - Mat Image             -> Image file from a track 
        - uint8_t bits          -> number of bits to shift right 
    output:
        - Mat audio_decompress  -> Audio file with bits gained 

*/
Mat decompress(Mat image,int color = IMG_COLOR, uint8_t bits=SHIFT_BITS){
    Mat image_decompressed(image.rows,image.cols,CV_8UC3,Scalar(0,0,0));
    // if(color != 0){
        for (int r = 0; r < image.rows; r++){
            for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
                for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                    image_decompressed.at<Vec3b>(r,c)[channel] = image.at<Vec3b>(r,c)[channel]<<bits;            
                }
            }
        }
    // }else{
    //     for (int r = 0; r < image.rows; r++){
    //         for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
    //             image_decompressed.at<uint32_t>(r,c) = image.at<uint32_t>(r,c) << bits;            
    //         }
    //     }
    // }
    return image_decompressed;
}
/* 
    function to calculate the peak of signal to noise between an original image and the altered image
    input args: 
        - Mat image1       -> image file Original 
        - Mat image2       -> image file Altered 
    output:
        -vector<double> psnr        -> vector with the PSNR calculated for each channel 
*/
vector<double> signalToNoise(Mat image1, Mat image2){
    vector<double> sums;
    vector<double> psnr;
    for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
        double sum = 0;
        sums.push_back(sum);
        for (int r = 0; r < image1.rows; r++){
            for (int c = 0; c < image2.cols; c++){
                sums[channel] = sums[channel] + pow(image1.at<Vec3b>(r,c)[channel] - image2.at<Vec3b>(r,c)[channel],2);
                
            }
        }
        double snr = 10*log10(pow(256,2)*image1.rows*image1.cols/sums[channel]);
        psnr.push_back(snr);
    }
    return psnr;
}
void saveImage(string path,Mat image, uint8_t show = 1 ){
    if(show){
        namedWindow(path, WINDOW_AUTOSIZE );
        imshow(path, image);
    }
    imwrite(path, image);
}
int main(int argc, char** argv){
    Mat image = imread("../Images/cao.jpg",IMG_COLOR);
    Mat image_yuv = image;
    GolombCode gc_encoded("gc_encoded.txt",4,'w');
    //image RGB2YUV 
    // cvtColor(image,image_yuv,COLOR_BGR2YUV);
    int before,after,now;
    ofstream ofs1("predict1.txt");
    ofstream ofs2("predict2.txt");
    gc_encoded.encode_int(image_yuv.rows);
    gc_encoded.encode_int(image_yuv.cols);
    for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
        for(int r = 0; r < image_yuv.rows; r++){
        before = image_yuv.at<Vec3b>(r,0)[channel];
        gc_encoded.encode_int(image_yuv.at<Vec3b>(r,0)[channel]);
        ofs1 << before << " ";

            for(int c= 1; c<image_yuv.cols; c++){
                    after = image_yuv.at<Vec3b>(r,c)[channel]; 
                    // image_yuv.at<Vec3b>(r,c)[channel] = 1;
                    now = before - image_yuv.at<Vec3b>(r,c)[channel];
                    ofs1 << now << " ";

                    if(now < 0){
                        // image_yuv.at<Vec3b>(r,c)[channel] = ((now*-1)<<1)+1;
                        gc_encoded.encode_int(((now*-1)<<1)+1);
                        // int x = image_yuv.at<Vec3b>(r,c)[channel];
                        // int y = ((now*-1)<<1)+1;
                        // ofs << x << " : " << y << " | ";
                        // cout << image_yuv.at<Vec3b>(r,c)[channel] << " : " << ((now*-1)<<1)+1 << endl;
                    }else{
                        // image_yuv.at<Vec3b>(r,c)[channel] = now << 1;
                        gc_encoded.encode_int((now << 1));
                        
                        // int x = image_yuv.at<Vec3b>(r,c)[channel];
                        // int y = now << 1;
                        // ofs << x << " : " << y << " | ";
                    }

                    before = after;            
                    // image_yuv.at<Vec3b>(r,c)[channel] = image_yuv.at<Vec3b>(r,c)[channel]<<SHIFT_BITS;            
            }
            ofs1 << endl;
            // ofs << endl;
            // image_yuv.at<Vec3b>(r,c)[0] = 200;
        }
    }

    
    GolombCode gc_decoded("gc_encoded.txt",4,'r');

    uint32_t rows,cols;
    rows = gc_decoded.decode_int();
    cols = gc_decoded.decode_int();
    Mat image_yuv_out(rows,cols,CV_8UC3,Scalar(0,0,0));
    // cvtColor(image_yuv, image_yuv_out, COLOR_YUV2BGR);
    for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
        for(int r = 0; r < rows; r++){
        image_yuv_out.at<Vec3b>(r,0)[channel] = gc_decoded.decode_int();
        ofs2 << (uint32_t)image_yuv_out.at<Vec3b>(r,0)[channel] << " ";
        // gc_decoded.encode_char(image_yuv_out.at<Vec3b>(r,0)[channel]);
            for(int c= 1; c<cols; c++){
                    uint32_t now = gc_decoded.decode_int();;
                    // image_yuv_out.at<Vec3b>(r,c)[channel] = (uint8_t)gc_decoded.decode_int(); 
                    ofs2 << now << " ";
                    // image_yuv_out.at<Vec3b>(r,c)[channel] = 1;
                    if( now & 0x01){
                        image_yuv_out.at<Vec3b>(r,c)[channel] = (uint8_t)(now >> 1) + image_yuv_out.at<Vec3b>(r,c-1)[channel];
                        // int x = image_yuv_out.at<Vec3b>(r,c)[channel];
                        // int y = ((now*-1)<<1)+1;
                        // ofs << x << " : " << y << " | ";
                        // cout << image_yuv_out.at<Vec3b>(r,c)[channel] << " : " << ((now*-1)<<1)+1 << endl;
                    }else{
                        image_yuv_out.at<Vec3b>(r,c)[channel] = image_yuv_out.at<Vec3b>(r,c-1)[channel] + (uint8_t)((now >> 1)*-1);

                        
                        // int x = image_yuv.at<Vec3b>(r,c)[channel];
                        // int y = now << 1;
                        // ofs << x << " : " << y << " | ";
                    }
           
                    // image_yuv.at<Vec3b>(r,c)[channel] = image_yuv.at<Vec3b>(r,c)[channel]<<SHIFT_BITS;            
            }
            ofs2 << endl;
            // image_yuv.at<Vec3b>(r,c)[0] = 200;
        }
    }
    // Mat image_yuv2bgr;
    // cvtColor(image_yuv_out, image_yuv2bgr,COLOR_YUV2BGR);
    // saveImage("../Images_Out/imagedecodedout.jpg", image_yuv2bgr);
    // image quantization 
    Mat image_compress = compress(image,IMG_COLOR,SHIFT_BITS);
    Mat image_decompress = decompress(image_compress,IMG_COLOR,SHIFT_BITS);
    
    //Histograms
    vector<map<short,int>> histoOutYUV = createHistogram(image_yuv);
    vector<map<short,int>> histoOut = createHistogram(image);
    
    //entropy
    vector<double> entropyOutYUV = histoEntropy(histoOutYUV,image_yuv.rows*image_yuv.cols);
    vector<double> entropyOut = histoEntropy(histoOut,image_decompress.rows*image_decompress.cols);
    
    //Signal-Noise Ratio 
    vector<double> snrYUV = signalToNoise(image, image_yuv_out);
    vector<double> snrQuantization = signalToNoise(image, image_decompress);
    
    //histoimages
    Mat histo_image_yuv = snrOnHisto(imageHisto(histoOutYUV,entropyOutYUV,6),snrYUV);
    Mat histo_image_decompress = snrOnHisto(imageHisto(histoOut,entropyOut,2),snrQuantization);
    saveImage("../Images_Out/imagem_yuv.jpg",image_yuv);

    saveImage("../Images_Out/imagem_yuv_out.jpg",image_yuv_out);
    saveImage("../Images_Out/imagem_out.jpg",image_decompress);
    saveImage("../Histograms/histo_yuv_out.jpg",histo_image_yuv);
    saveImage("../Histograms/histo_out.jpg",histo_image_decompress);

    cout << " yuv: " << snrYUV[0] << " | normal: " << snrQuantization[0] << endl;
    waitKey(1);
}
// int main(int argc, char** argv ) // main para usar imagens (meter WAIT_KEY a 0)
// {
//     Mat image = imread("../Images/cao.jpg",IMG_COLOR);
//     Mat image_yuv;
//     cvtColor(image,image_yuv,COLOR_BGR2YUV);
//     saveImage("../Images_Out/image_yuv.jpg", image_yuv);
    
//     Mat image_compressed = compress(image);
//     Mat image_decompressed = decompress(image_compressed);
//     vector<double> snr = signalToNoise(image, image_decompressed);
    
//     vector<map<short,int>> histo = createHistogram(image_yuv);
//     string file = "../Histograms/histo.txt";
//     histoToFile(file,histo);
//     vector<double> entropy = histoEntropy(histo,image.rows*image.cols/HISTO_BITSCAPTURE);
//     Mat histo_image = imageHisto(histo,entropy);

    
//     vector<map<short,int>> histoOut = createHistogram(image_decompressed);
//     histoToFile(file,histo);
//     vector<double> entropyOut = histoEntropy(histoOut,image_decompressed.rows*image_decompressed.cols/HISTO_BITSCAPTURE);
//     Mat histo_image_decompressed = snrOnHisto(imageHisto(histoOut,entropyOut,5),snr);
    
//     if ( !image.data )
//     {
//         printf("No image data \n");
//         return -1; 
//     }
//     // saveImage("imagem.jpg", image);
//     saveImage(argc!=3 ?"../Images_Out/out.jpg":argv[2], image_decompressed);
//     saveImage("../Histograms/histo.jpg", histo_image);
//     saveImage("../Histograms/histo_out.jpg", histo_image_decompressed);
//     waitKey(WAIT_KEY);
    
//     return 0;
// } 

// int main(int argc, char** argv )  // main para usar webcam (meter WAIT_KEY diferente de 0)
// {
//     VideoCapture cap(0);
//     Mat image;
//     while(1)
//     {
//         cap.read(image);

//         vector<map<short,int>> histo = createHistogram(image);

//         vector<double> entropy = histoEntropy(histo,image.rows*image.cols/HISTO_BITSCAPTURE);
//         cout << entropy[0] << endl;
//         Mat histo_image = imageHisto(histo,entropy);
        
//         namedWindow("Histogram", WINDOW_AUTOSIZE );
//         imshow("Histogram", histo_image);
//         namedWindow("Display Image", WINDOW_AUTOSIZE );
//         imshow("Display Image", image);
//         waitKey(WAIT_KEY);
//     }
//     return 0;
// } 
