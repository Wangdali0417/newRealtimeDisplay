 /*
// Design unit: Sound Field Acquisition - 60 Channels
//            :
// File name  : ftdi.cpp
//            :
// Description: Contains function definitions for class "ftdi"
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


#include "ftdi.h"

using namespace std;

ftdi::ftdi()
{

}

//Constructor: assigns ID to object
ftdi::ftdi(char _FTDI_ID)
{
    FTDI_ID =_FTDI_ID;
    for(qint64 i = 0; i < ChanNumber; i++)
    {
        bufferPosition[i] = 0;
    }
}

//Set the FTDI device as synchronous parallel FIFO
void ftdi::set_fifo_mode()
{
    ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);
    if (ftStatus == FT_OK) {
        //printf("FTDI%c Initialized.\n", FTDI_ID);
        qDebug() << "FTDI" << FTDI_ID << " Initialized.\n";
    }
    else
    {
        //printf("value failed for FTDI%c. Please check the connection.\n", FTDI_ID);
        qDebug() << "value failed for FTDI" << FTDI_ID << ". Please check the connection.\n";
    }
}

// Send reset command to FTDI which would clear internal buffers
void ftdi::reset_fifo_buffer(void)
{
    ftStatus = FT_ResetDevice(ftHandle);
    if(ftStatus == FT_OK){
        //printf("Device reset done for FTDI%c!\n", FTDI_ID);
        qDebug() << "Device reset done for FTDI"  << FTDI_ID << "\n";
    }
    else{
        //printf("Device reset not successful for FTDI%c!\n", FTDI_ID);
        qDebug() << "Device reset not successful for FTDI"  << FTDI_ID << "\n";
    }
}

//Initialize FTDI
void ftdi::initialize()
{
    //set detected flag
    detected_flag = true;
    //set as 245 sync. fifo mode
    set_fifo_mode();
    //reset buffer of FTDI
    reset_fifo_buffer();
    //look for header byte for sync
    seek_header_byte();
    //reset counter for channel detection
    for(int i=0;i<NO_OF_CHANNELS_PER_FTDI;i++)
        count_for_channel_detection[i]=0;

}

//scan channels algorithm
//void ftdi::scanchannels(string current_date_time_, DWORD chunk_size, string file_path)
//{
//    //count 0xFF for 1024 times, if channel is connected it should receive arbitrary data
//    int ch_sync_count;     //count for keeping sync with filepointers
//    int frame_bit_count;
//    bitset<8>grouped_byte;  //for combining 8bits from different frames in a byte for each channel
//    int size_group_8frame = FRAME_SIZE*8;
//    UCHAR group_8frame[size_group_8frame];   //to make a complete byte for each channel
//    int byte_for_each_channel;      //to gether each bit saperately and group it in one byte
//    int bit_index;

//    for(int i=0;i<chunk_size;i+=size_group_8frame){
//        //create a single frame
//        /* format of frame:
//         *      group_8frame[0]             group_8frame[1]     group_8frame[2]     group_8frame[3]     group_8frame[4].... repeated 8 times for making a byte
//         * <5 dummybits, Ch.30, Ch. 29>     <0,Ch.28,...22>     <0,Ch.21,...15>     <0,Ch.14,...8>      <0,Ch.7,...1>
//        */

//        for(int frame_count = size_group_8frame-1; frame_count>=0; frame_count--){
//            group_8frame[size_group_8frame-frame_count-1] = RxBuffer[i+frame_count];
//        }
//        //group 8 bits for each channel and write in saperate files
//        frame_bit_count = (FRAME_SIZE-1)*8-1;
//        for(ch_sync_count=0; ch_sync_count<NO_OF_CHANNELS_PER_FTDI; ch_sync_count++){
//            frame_bit_count = ((ch_sync_count!=0)&&((ch_sync_count)%7==0)) ? (frame_bit_count-14) : (frame_bit_count+1);

//            //make a group of 8 bits for each channel
//            byte_for_each_channel = 0;
//            for(int k=0;k<8;k++){
//                bit_index = frame_bit_count+ (FRAME_SIZE*8)*(7-k);
//                byte_for_each_channel += GET_BIT(group_8frame, bit_index)*(1<<k);
//            }

//            grouped_byte = byte_for_each_channel;
//            if(grouped_byte==0xFF)
//                count_for_channel_detection[ch_sync_count]++;
//        }
//    }

//    detect_channels();
//    //create list of files only for detected channels
//    qDebug() << "before create file";
//    create_file(current_date_time_, file_path);
//    qDebug() << "after create file";
//}

//seek new header from chunk
void ftdi::seek_header_byte()
{
    reset_fifo_buffer();
    //printf("Looking for header byte in FTDI%c...\n", FTDI_ID);
    qDebug() << "Looking for header byte in FTDI"  << FTDI_ID << "\n";
    while(RxBytes == 0) {
        FT_GetStatus(ftHandle, &RxBytes, &TxBytes, &EventDWord);
    }
    //cout << "First Byte Received."<< endl;
    qDebug() << "First Byte Received.\n";
    //cout <<"detecting header byte..."<< endl;
    qDebug() << "detecting header byte...\n";
    //find header byte
    bool header_found = false;
    while(header_found == false) {
        FT_GetStatus(ftHandle, &RxBytes, &TxBytes, &EventDWord);
        if( RxBytes>0 ){
            //read single byte from device
            ftStatus=FT_Read(ftHandle, RxBuffer, 1, &BytesReceived);
            //printf(".");
            qDebug() << ".";
            if(RxBuffer[0]>=128) header_found=true;
        }
    }
    //cout << "\nHeader byte found for FTDI" << FTDI_ID << ".\n";
    qDebug() << "\nHeader byte found for FTDI" << FTDI_ID << ".\n";
    //discard first frame
    FT_GetStatus(ftHandle, &RxBytes, &TxBytes, &EventDWord);
    if( RxBytes>FRAME_SIZE){
        //read single byte from device
        ftStatus=FT_Read(ftHandle, RxBuffer, FRAME_SIZE-1, &BytesReceived);
    }
}

//creates file array
void ftdi::create_file(string current_date_time, string file_path)
{
    string foldername_ = file_path + current_date_time;

    if (no_of_found_channels==0) {
        //printf("No channels found for FTDI%c, no files were created.\n",FTDI_ID);
        qDebug() << "No channels found for FTDI" << FTDI_ID << ", no files were created.\n";
        return;
    }
    for(int  k=0; k<no_of_found_channels; k++){
        //Create separate directory with timestamp
        mkdir(foldername_.c_str());
        string filenames_ = foldername_ + "\\Ch" + to_string((FTDI_ID-48-1)*15+(connected_channels.at(k)/2)+1);
        if(connected_channels.at(k)%2==0) //left channels
             filenames_ += "L.bin";
        else                              //right channels
            filenames_ += "R.bin";

        remove( filenames_.c_str() );
        fout[connected_channels.at(k)].open( filenames_.c_str() , ios::binary | ios::out);
    }
}

//Detect conncected channels
//void ftdi::detect_channels()
//{
//    for(int i=0;i<NO_OF_CHANNELS_PER_FTDI;i++) {
//        if(count_for_channel_detection[i] < 1024)
//            connected_channels.push_back(i);
//    }

//    //print list of channels
//    no_of_found_channels = connected_channels.size();
//    if (no_of_found_channels >0){
//        //printf("Channels found connected to FTDI%c:\n", FTDI_ID);
//        qDebug() << "Channels found connected to FTDI" << FTDI_ID << ":\n";
//        for(int i=0; i<no_of_found_channels; i++){
//            if(connected_channels.at(i)%2==0) //left channels
//                //printf("Ch%dL\n",(FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1);
//                qDebug() << "Ch" << (FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1 << "L\n";
//                //printf("Ch%dR\n",(FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1);
//                qDebug() << "Ch" << (FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1 << "R\n";
//        }
//    }
//    else
//        //printf("No channels found connected to FTDI%c\n", FTDI_ID);
//        qDebug() << "No channels found connected to FTDI" << FTDI_ID << "\n";

//}


//Decode data from chunk of frames and write into saperate files
void ftdi::write_chunk_data(DWORD chunk_size)
{
    int ch_sync_count;     //count for keeping sync with filepointers
    bitset<8>grouped_byte;  //for combining 8bits from different frames in a byte for each channel
    int size_group_8frame = FRAME_SIZE*8;
    UCHAR group_8frame[size_group_8frame];   //to make a complete byte for each channel
    int byte_for_each_channel;      //to gether each bit saperately and group it in one byte
    int bit_index;
    int bit_index2;
    int current_channel_index;
    int buffer_count = 0;

    for(int i=0;i<chunk_size;i+=size_group_8frame){
            //create a single frame
        /* format of frame:
         *      group_8frame[0]             group_8frame[1]     group_8frame[2]     group_8frame[3]     group_8frame[4].... repeated 8 times for making a byte
         * <5 dummybits, Ch.30, Ch. 29>     <0,Ch.28,...22>     <0,Ch.21,...15>     <0,Ch.14,...8>      <0,Ch.7,...1>
        */
        for(int frame_count = size_group_8frame-1; frame_count>=0; frame_count--){
            group_8frame[size_group_8frame-frame_count-1] = RxBuffer[i+frame_count];
        }
            //group 8 bits for each channel and write in saperate files
        for(ch_sync_count=0; ch_sync_count<no_of_found_channels;ch_sync_count++){
            current_channel_index = connected_channels.at(ch_sync_count);
            byte_for_each_channel = 0;
            for(int k=0;k<8;k++){
                bit_index = frame_bit_count[current_channel_index]+ (FRAME_SIZE*8)*(7-k);
                byte_for_each_channel += GET_BIT(group_8frame, bit_index)*(1<<k);

                bit_index2 = frame_bit_count[current_channel_index]+ (FRAME_SIZE*8)*k;
                receivePDMdata(ch_sync_count, GET_BIT(group_8frame, bit_index2));//First parameter is originally current_channel_index
            }
            grouped_byte = byte_for_each_channel;
            //cout <<grouped_byte<<"\t";

            fout[current_channel_index] << (char) grouped_byte.to_ulong();
        }
    }

}



void ftdi::write_chunk_buffer(DWORD chunk_size)
{
    int ch_sync_count;     //count for keeping sync with filepointers
    bitset<8>grouped_byte;  //for combining 8bits from different frames in a byte for each channel
    int size_group_8frame = FRAME_SIZE*8;
    UCHAR group_8frame[size_group_8frame];   //to make a complete byte for each channel
    int byte_for_each_channel;      //to gether each bit saperately and group it in one byte
    int bit_index;
    int current_channel_index;

    for(int i=0;i<chunk_size;i+=size_group_8frame){
            //create a single frame
        /* format of frame:
         *      group_8frame[0]             group_8frame[1]     group_8frame[2]     group_8frame[3]     group_8frame[4].... repeated 8 times for making a byte
         * <5 dummybits, Ch.30, Ch. 29>     <0,Ch.28,...22>     <0,Ch.21,...15>     <0,Ch.14,...8>      <0,Ch.7,...1>
        */
        for(int frame_count = size_group_8frame-1; frame_count>=0; frame_count--){
            group_8frame[size_group_8frame-frame_count-1] = RxBuffer[i+frame_count];
        }
            // group 8 bits for each channel and write in saperate files
        for(ch_sync_count=0; ch_sync_count<no_of_found_channels;ch_sync_count++){//ch_sync_count
            current_channel_index = connected_channels.at(ch_sync_count);
            byte_for_each_channel = 0;
            for(int k=0;k<8;k++){
                bit_index = frame_bit_count[current_channel_index]+ (FRAME_SIZE*8)*k;
                receivePDMdata(ch_sync_count, GET_BIT(group_8frame, bit_index));//First parameter is originally current_channel_index
                 //byte_for_each_channel += GET_BIT(group_8frame, bit_index)*(1<<k);
            }

        }

    }
    //cout << "++++" << current_channel_index << "xxxxxxxx";
}


void ftdi::receivePDMdata(qint64 channelNumber, qint64 temp){

    if((temp & 1) == 1){

        //m_pdmData[channelNumber].append(1);
        qint64 temPosition = bufferPosition[channelNumber] % bufferSize;
        m_pdmData[channelNumber][temPosition] = 1;
        bufferPosition[channelNumber] = temPosition + 1;

    }
    else{

        //m_pdmData[channelNumber].append(0);
        qint64 temPosition = bufferPosition[channelNumber] % bufferSize;
        m_pdmData[channelNumber][temPosition] = 0;
        bufferPosition[channelNumber] = temPosition + 1;
    }

}


void ftdi::set_ID(char c)
{
    FTDI_ID = c;
}

//-----------------------------------------------------------------------------------------------
//---------------------------------
//--------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//scan channels algorithm
bool ftdi::scanchannels(string current_date_time_, DWORD chunk_size, string file_path)
{
    //count 0xFF for 1024 times, if channel is connected it should receive arbitrary data
    int ch_sync_count;     //count for keeping sync with filepointers
    int frame_bit_count;
    bitset<8>grouped_byte;  //for combining 8bits from different frames in a byte for each channel
    int size_group_8frame = FRAME_SIZE*8;
    UCHAR group_8frame[size_group_8frame];   //to make a complete byte for each channel
    int byte_for_each_channel;      //to gether each bit saperately and group it in one byte
    int bit_index;

    for(int i=0;i<chunk_size;i+=size_group_8frame){
        //create a single frame
        /* format of frame:
         *      group_8frame[0]             group_8frame[1]     group_8frame[2]     group_8frame[3]     group_8frame[4].... repeated 8 times for making a byte
         * <5 dummybits, Ch.30, Ch. 29>     <0,Ch.28,...22>     <0,Ch.21,...15>     <0,Ch.14,...8>      <0,Ch.7,...1>
        */

        for(int frame_count = size_group_8frame-1; frame_count>=0; frame_count--){
            group_8frame[size_group_8frame-frame_count-1] = RxBuffer[i+frame_count];
        }
        //group 8 bits for each channel and write in saperate files
        frame_bit_count = (FRAME_SIZE-1)*8-1;
        for(ch_sync_count=0; ch_sync_count<NO_OF_CHANNELS_PER_FTDI; ch_sync_count++){
            frame_bit_count = ((ch_sync_count!=0)&&((ch_sync_count)%7==0)) ? (frame_bit_count-14) : (frame_bit_count+1);

            //make a group of 8 bits for each channel
            byte_for_each_channel = 0;
            for(int k=0;k<8;k++){
                bit_index = frame_bit_count+ (FRAME_SIZE*8)*(7-k);
                byte_for_each_channel += GET_BIT(group_8frame, bit_index)*(1<<k);
            }

            grouped_byte = byte_for_each_channel;
            if(grouped_byte==0xFF)
                count_for_channel_detection[ch_sync_count]++;
        }
    }
    bool isDetected;
    isDetected = detect_channels();
    //create list of files only for detected channels
    qDebug() << "before create file";
    create_file(current_date_time_, file_path);
    qDebug() << "after create file";
    return isDetected;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Wangdali modified Detect conncected channels
bool ftdi::detect_channels()
{
    for(int i=0;i<NO_OF_CHANNELS_PER_FTDI;i++) {
        if(count_for_channel_detection[i] < 1024)
            connected_channels.push_back(i);
    }

    //print list of channels
    no_of_found_channels = connected_channels.size();
    if (no_of_found_channels >0){
        //printf("Channels found connected to FTDI%c:\n", FTDI_ID);
        qDebug() << "Channels found connected to FTDI" << FTDI_ID << ":\n";
        for(int i=0; i<no_of_found_channels; i++){
            if(connected_channels.at(i)%2==0) //left channels
            { //printf("Ch%dL\n",(FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1);
                qDebug() << "Ch" << (FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1 << "L\n";
                saveConnectedChannel[i] = "Ch"+QString::number((FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1)+"L";
            }
            else
            {
                //printf("Ch%dR\n",(FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1);
                qDebug() << "Ch" << (FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1 << "R\n";
                saveConnectedChannel[i] = "Ch"+QString::number((FTDI_ID-48-1)*15+(connected_channels.at(i)/2)+1)+"R";
            }
        }
        return true;
    }
    else
        //printf("No channels found connected to FTDI%c\n", FTDI_ID);
        qDebug() << "No channels found connected to FTDI" << FTDI_ID << "\n";
    return false;

}



