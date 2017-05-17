/*
// Design unit: Sound Field Acquisition - 60 Channels
//            :
// File name  : ftdi.h
//            :
// Description: Contains class definition for ftdi
//
// Author     : Kishan Pethani
//            : Master Thesis Student, DFKI GmbH Kaiserslautern, Germany
//            : MSc. EMECS Student, Technical University of Kaiserslautern, Germany
//            : Kishan_Viththalbhai.Pethani@dfki.uni-kl.de
//
// Revision   : version 1.0
//
// All rights reserved by DFKI GmbH
*/


#ifndef FTDI_H
#define FTDI_H


#include <windows.h>
#include "ftd2xx.h"
#include <string.h>
#include <unistd.h>
#include <iostream>	
#include <fstream>
#include <QDebug>
#include <bitset>
#include <ctime>
#include <sys/time.h>
#include <vector>
#include <QDebug>
#include <QBitArray>
#include "global.h"


#define FRAME_SIZE                  5
#define NO_OF_CHANNELS_PER_FTDI     30

//macro to read bit-by-bit from array
#define GET_BIT(p, n) ((((unsigned char *)p)[n/8] >> (n%8)) & 0x01)

using namespace std;

class ftdi
{

private:
    char FTDI_ID;

public:
    FT_STATUS ftStatus;
    FT_HANDLE ftHandle;
    DWORD TxBytes;
    DWORD RxBytes;
    DWORD EventDWord;
    DWORD BytesReceived;
    UCHAR RxBuffer[65536];
    UCHAR Mask = 0xff;
    UCHAR Mode = 0x40;                          // for synchronous parallel fifo
    std::ofstream fout[NO_OF_CHANNELS_PER_FTDI];
    bool detected_flag = false;                 //true= detected, false=not detected
    int count_for_channel_detection[NO_OF_CHANNELS_PER_FTDI];
    int no_of_found_channels = 0;
    //int buffer_count = 0;
    vector <int> connected_channels;// vector array of list of found connected channels
    qint64 bufferPosition[ChanNumber];//现在假设channelnumber是从0依次往后

    int frame_bit_count[30] = {                 // mapping for frame bits to channel no. ,refer Designer's Guide for better understanding
                                                32,//Ch0
                                                33,//1
                                                34,//2
                                                35,//3
                                                36,//4
                                                37,//5
                                                38,//6
                                                24,//7
                                                25,//8
                                                26,//9
                                                27,//10
                                                28,//11
                                                29,//12
                                                30,//13
                                                16,//14
                                                17,//15
                                                18,//16
                                                19,//17
                                                20,//18
                                                21,//19
                                                22,//20
                                                8,//21
                                                9,//22
                                                10,//23
                                                11,//24
                                                12,//25
                                                13,//26
                                                14,//27
                                                0,//28
                                                1,//29
                              };


    ftdi();

    //Constructor: assigns ID to object
    ftdi(char _FTDI_ID);


    //Initialize FTDI
    void initialize();

    //Set the FTDI device as synchronous parallel FIFO
    void set_fifo_mode();

    //Send reset command to FTDI which would clear internal buffers
    void reset_fifo_buffer();

    //Seek new header from chunk
    void seek_header_byte();

    //Creates file array
    void create_file(string,string);

    //Detect conncected channels
    //void detect_channels();
    bool detect_channels();

    //scan channels algorithm
    //void scanchannels(string,DWORD,string);
    bool scanchannels(string,DWORD,string);

    //Decode data from chunk of frames and write into saperate files
    void write_chunk_data(DWORD chunk_size);

    void write_chunk_buffer(DWORD);

    void receivePDMdata(qint64, qint64);

    void set_ID(char);
};

#endif // FTDI_H
