 #include "datareciever.h"

DataReciever::DataReciever()
{
    ft1.set_ID('1');
    ft2.set_ID('2');

}

DataReciever::~DataReciever()
{ 
}

void DataReciever::dataRecieving()
{
    // create the device information list



    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus == FT_OK)
    {
        //printf("Number of devices is %d\n",numDevs);
        QString str = "Number of devices is " + QString::number(numDevs);
        emit showText(str);
    }

    if (numDevs > 0)
    {
        // allocate storage for list based on numDevs
        devInfo =  (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
        // get the device information list
        ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
        if (ftStatus == FT_OK) {
            // Attempt to open FTDI port
            ft1.ftStatus = FT_OpenEx((PVOID)"FTDI1",FT_OPEN_BY_DESCRIPTION,&ft1.ftHandle);
            ft2.ftStatus = FT_OpenEx((PVOID)"FTDI2",FT_OPEN_BY_DESCRIPTION,&ft2.ftHandle);

            // Print status of FTDIs
            if((ft1.ftStatus!= FT_OK) & (ft2.ftStatus!= FT_OK)) {
                //printf("FTDI1 not found, FTD2 not found! Check port connection.\n");
                QString str = "FTDI1 not found, FTD2 not found! Check port connection.\n";
                emit showText(str);
                return;
            }
            if (ft1.ftStatus== FT_OK)
            {
                //printf("FTDI1 found!\n");
                QString str = "FTDI1 found!\n";
                emit showText(str);
                ft1.initialize();
            }
            if (ft2.ftStatus== FT_OK)
            {
                //printf("FTDI2 found!\n");
                QString str = "FTDI2 found!\n";
                emit showText(str);
                ft2.initialize();
            }
        }
    }
    else
    {
        //printf("No devices detected. Exiting...\n");
        QString str = "No devices detected. Exiting...\n";
        emit showText(str);
        return;
    }

    // Get current date and time stamp for creating folder
    current_date_time_ = currentDateTime();

    //printf("Detecting connected channels...\n");
    QString str = "Detecting connected channels...\n";
    emit showText(str);

    // scan which microphone channels are connected to hardware
    //scan_channels();
    //-----------------------------------------------------------------------------------------------
    // Wangdali modified to show connected channel infos
    bool isScaned ;
    isScaned = scan_channels();
    if(isScaned)
        {
         emit showConnectedChannel();
        }
    else
        {
         emit showText("No channels found connected to FTDI");
        }
    //-----------------------------------------------------------------------------------------------


    //qDebug() << "before loop";
    //wait for user input
    int sum = 0;
    do
    {
       if(sum == 0)
       {
           QString str = "Press  continue to run ...";
           emit showText(str);
           emit cont();
       }
       sum++;
      // qDebug() << "flage thread: " + QString::number(flag);
    }while(flag);
    //qDebug() << "flage after thread: " + QString::number(flag);


    if(ft1.no_of_found_channels == 0 && ft2.no_of_found_channels==0)
    {
        //printf("No channels found! Exiting...\n");
        QString str = "No channels found! Exiting...";
        emit showText(str);
        return;
    }

    //to record start time for checking transmission speed
    clock_t begin = clock();
    str = "---starttime = " + QString::number(double(begin)) + "ms.---\n";
    str = str + "Total Recording Time(s): " + QString::number(RECORDING_TIME) + "\n";
    str = str + "Remaining Time(s): \n";
    str = str + "\r" + QString::number(RECORDING_TIME) + "..";

    emit showText(str);
    //cout <<"---starttime = " << double(begin) << "ms.---"<< endl;
    //cout <<"Total Recording Time(s): " << RECORDING_TIME << endl;
    //cout << "Remaining Time(s): " << endl;
    //cout << '\r' << RECORDING_TIME << ".. ";
    int lost_count = 0;
    int current_recording_time = 0;
    //int chunkcount = 0;


    // Run this loop forever until the recording time is covered
    // Press Ctrl+C to stop recording in between
    //qint64 recordIndex = 0;
    emit startplot();
    while(1){
            //recording time
        if(int(global_count/NO_OF_SAMPLES_IN_SEC) > current_recording_time) {
            current_recording_time++;
            //cout << '\r' << RECORDING_TIME - current_recording_time << ".. ";
            //qDebug() << "========================================";
//            if(current_recording_time == 1)
//            {
//                emit plot();
//            }
            str = '\r' + QString::number((RECORDING_TIME - current_recording_time)) + ".. ";
            emit showText(str);

        }
            //get the device status including number of characters in receive queue
         if(ft1.detected_flag)   FT_GetStatus(ft1.ftHandle, &ft1.RxBytes, &ft1.TxBytes, &ft1.EventDWord);
         if(ft2.detected_flag)   FT_GetStatus(ft2.ftHandle, &ft2.RxBytes, &ft2.TxBytes, &ft2.EventDWord);

            //chunk_size defines how many frames are read from FTDI at a time
        DWORD chunk_size = FRAME_SIZE*8*(1024);           //must be multiple 8 for packing 8bits into a byte for each channel
        if( (~ft1.detected_flag | ft1.RxBytes>chunk_size) && (~ft2.detected_flag | ft2.RxBytes>chunk_size) ){
            //read data from the device
            if(ft1.detected_flag)   ft1.ftStatus=FT_Read(ft1.ftHandle, ft1.RxBuffer, chunk_size, &ft1.BytesReceived);
            if(ft2.detected_flag)   ft2.ftStatus=FT_Read(ft2.ftHandle, ft2.RxBuffer, chunk_size, &ft2.BytesReceived);
            //verify header
            if((ft1.detected_flag && ft1.RxBuffer[0] <128) || (ft2.detected_flag && ft2.RxBuffer[0]<128) ){        //found currept header
                //cout <<"Lost header...\n";
                str = "Lost header...\n";
                emit showText(str);

                lost_count++;
                //look for header byte for sync
                 if(ft1.detected_flag)  ft1.seek_header_byte();
                 if(ft2.detected_flag)  ft2.seek_header_byte();
            }
            else{   //write chunk into files


                //double time_start = (double)clock();

                freeSpace.acquire(1024*8);
                //qDebug() << "worker freeSpace: " << freeSpace.available();
                if(ft1.detected_flag)  ft1.write_chunk_data(chunk_size);
                if(ft2.detected_flag)  ft2.write_chunk_data(chunk_size);
                usedSpace.release(1024*8);
                //chunkcount++;
                //qDebug() << "worker usedSpace: " << usedSpace.available();


                //double time_end = (double)clock();
                //qDebug() << "write_chunk_data: " << QString::number(time_end - time_start) << "\n";


                //emit plot();


                /*for(qint64 i = 0; i < 900000000; i++)
                {}*/
                global_count += ft1.BytesReceived;

//                qDebug() << "BytesReceived:" <<  ft1.BytesReceived;

//                qDebug() << "workerAllDataCount:" <<  ((qint64)(global_count / 5));

            }



            if(global_count>TOTAL_REQUIRED_SAMPLES){//Recording time expires

                workerAllDataCount = global_count / 5;
//                qDebug() << "************";
//                qDebug() << "worker and widget all data: "<< workerAllDataCount << " " << widgetAllDataCount;
//                qDebug() << "************\n";


                //qDebug() << "freeSpace: " << freeSpace.available();
                //qDebug() << "usedSpace: " << usedSpace.available();
                //qDebug() << "chunkcount" << chunkcount;

                emit changeFlagComp();

                for(i=0;i<FRAME_SIZE;i++){
                    ft1.fout[i].close();
                    ft2.fout[i].close();
                }
                //Print Summary
                qDebug() << "\nFile written. Collected Samples = " << global_count;
                clock_t end = clock();
                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
                qDebug() <<"---endtime = " << double(end);
                qDebug() << "Elapsed_secs = " << elapsed_secs << "\t DataRate:" << global_count/elapsed_secs << " BytesPerSecond";
                qDebug() << "lost_count = " << lost_count;
                break;
            }
        } // end of if( ft1.RxBytes>FRAME_SIZE && ft2.RxBytes>FRAME_SIZE )
    }//end of while


    //close FTDI ports
    FT_Close(ft1.ftHandle);FT_Close(ft2.ftHandle);
}



void DataReciever::handleTimeout()
{
    emit startplot();

}



string DataReciever::currentDateTime()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y %I_%M",timeinfo);   //format of the date and time stamp
    std::string str(buffer);
    return str;

}


//void DataReciever::scan_channels()
//{
//        //get the device status including number of characters in receive queue
//    if(ft1.detected_flag)   FT_GetStatus(ft1.ftHandle, &ft1.RxBytes, &ft1.TxBytes, &ft1.EventDWord);
//    if(ft2.detected_flag)   FT_GetStatus(ft2.ftHandle, &ft2.RxBytes, &ft2.TxBytes, &ft2.EventDWord);
//    DWORD chunk_size = FRAME_SIZE*8*1024;           //must be multiple 8 for packing 8bits into a byte for each channel

//    if( (~ft1.detected_flag | ft1.RxBytes>chunk_size) && (~ft2.detected_flag | ft2.RxBytes>chunk_size) ){
//        //read data from the device
//            if(ft1.detected_flag)   ft1.ftStatus=FT_Read(ft1.ftHandle, ft1.RxBuffer, chunk_size, &ft1.BytesReceived);
//            if(ft2.detected_flag)   ft2.ftStatus=FT_Read(ft2.ftHandle, ft2.RxBuffer, chunk_size, &ft2.BytesReceived);
//        //verify header
//        if( (ft1.detected_flag && ft1.RxBuffer[0] <128) || (ft2.detected_flag && ft2.RxBuffer[0]<128)){        //found currept header
//            cout <<"Lost header...\n";
//                //look for header byte for sync
//            if(ft1.detected_flag)  ft1.seek_header_byte();
//            if(ft2.detected_flag)  ft2.seek_header_byte();
//        }
//        else{
//            if(ft1.detected_flag)   ft1.scanchannels(current_date_time_, chunk_size, file_path);
//            if(ft2.detected_flag)   ft2.scanchannels(current_date_time_, chunk_size, file_path);


//        }
//    }
//}


//----------------------------------------------------------------------------------------------------
bool DataReciever::scan_channels()
{
    bool isScaned1,isScaned2;
        //get the device status including number of characters in receive queue
    if(ft1.detected_flag)   FT_GetStatus(ft1.ftHandle, &ft1.RxBytes, &ft1.TxBytes, &ft1.EventDWord);
    if(ft2.detected_flag)   FT_GetStatus(ft2.ftHandle, &ft2.RxBytes, &ft2.TxBytes, &ft2.EventDWord);
    DWORD chunk_size = FRAME_SIZE*8*1024;           //must be multiple 8 for packing 8bits into a byte for each channel

    if( (~ft1.detected_flag | ft1.RxBytes>chunk_size) && (~ft2.detected_flag | ft2.RxBytes>chunk_size) ){
        //read data from the device
            if(ft1.detected_flag)   ft1.ftStatus=FT_Read(ft1.ftHandle, ft1.RxBuffer, chunk_size, &ft1.BytesReceived);
            if(ft2.detected_flag)   ft2.ftStatus=FT_Read(ft2.ftHandle, ft2.RxBuffer, chunk_size, &ft2.BytesReceived);
        //verify header
        if( (ft1.detected_flag && ft1.RxBuffer[0] <128) || (ft2.detected_flag && ft2.RxBuffer[0]<128)){        //found currept header
            cout <<"Lost header...\n";
                //look for header byte for sync
            if(ft1.detected_flag)  ft1.seek_header_byte();
            if(ft2.detected_flag)  ft2.seek_header_byte();
        }
        else{
            if(ft1.detected_flag)
            {
                isScaned1 = ft1.scanchannels(current_date_time_, chunk_size, file_path);
                return isScaned1;
            }
            if(ft2.detected_flag)
            {
                isScaned2 =ft2.scanchannels(current_date_time_, chunk_size, file_path);
                return isScaned2;
            }


        }
    }
}

