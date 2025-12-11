#include <iostream>
#include <fstream>
#include "FileOperator.h"
#include "organize.h"
using namespace std;

int main(){

    char *FLAGS_BatchFilePath="/home/lisa/list/nginx_tar.list";
    char *outPutPath="/home/lisa/organize/nginx2/";

    //string C1=("echo\"042230\"|sudo -S sh -c 'rm ");
//    string C1 = "echo \"042230\" | sudo -S sh -c 'rm ";
//    string C2(outPutPath);
//    string C3="*'";
//
//    string command=C1+C2+C3;
//    cout<<"Command: "<<command<<endl;
//
//    system(command.c_str());

    ifstream infile(FLAGS_BatchFilePath,ios::binary);
    string subPath;
    cout<<"Restore Batch path: "<<FLAGS_BatchFilePath<<endl;

    uint64_t totalsize=0;
    uint64_t fileDedupSize=0;

    uint64_t version=0;
    uint64_t realLayerNum=0;

    Timer allTimer;
    allTimer.startTime();

    //读取进行层进行去重
    while(getline(infile,subPath)){
        uint64_t size=0;
        version++;
        FileOperator fileOperator((char *)subPath.c_str(),FileOpenType::Read);
        size=FileOperator::size((char *)subPath.c_str());
        totalsize+=size;

        cout<<"Read layer #"<<version<<" size: "<<size/1024.0/1024.0<<" MB path: "<<subPath<<endl;

        auto *buffer=(uint8_t *) malloc(size);
        fileOperator.read(buffer,size);

        //限制参与去重的镜像层大小
        if(size<MinLayerSize){
            free(buffer);
            continue;
        }
        realLayerNum++;
        //读取镜像层文件，进行去重，并记录共享文件的镜像层
        fileDedupSize+= FileDedup(buffer,size,version);
        free(buffer);
    }

    //维护镜像文件与镜像层之间的映射
    uint64_t  realDedupsize=0;
    realDedupsize= organize(realLayerNum,outPutPath);

    {
        cout<<"================*************===================\n";
        cout<<"Layer version: "<<version<<endl;
        cout<<"Total Size: "<<totalsize/1024.0/1024.0<<"MB\n";
        cout<<"Global File dedup size(排除小镜像层、小文件): "<<fileDedupSize/1024.0/1024.0<<"MB\n";
        cout<<"================*************===================\n";
        cout<<"MinCompactFileSize: "<<MinCompactFileSize/1024.0/1024.0<<"MB\n";

    }

    cout<<"Real File dedup size (排除低于限制的共享文件): "<<realDedupsize/1024.0/1024.0<<"MB\n";

#ifdef Restore
    cout<<"================*************===================\n";
    cout<<"Write Begin......\n";

    ifstream infile1(FLAGS_BatchFilePath, ios::binary);
    uint64_t  versionNum=0;
    uint64_t writesize=0;

    string subPath1;
    while(getline(infile1,subPath1)){
        uint64_t size=0;
        versionNum++;
        FileOperator fileOperator((char *)subPath1.c_str(),FileOpenType::Read);
        size=FileOperator::size((char *)subPath1.c_str());
        auto *buffer=(uint8_t*) malloc(size);
        fileOperator.read(buffer,size);

        if(size<MinLayerSize){
            writesize+= archive_small(buffer,size,versionNum,outPutPath);
            free(buffer);
            continue;
        }

        writesize+= archive(buffer,size,versionNum,outPutPath);

        cout<<"Write Finish # "<<versionNum<<endl;
        free(buffer);
    }

    allTimer.endTime();
    cout<<"Organize Dedup Speed: "<<totalsize/1024.0/1024.0/allTimer.timeval<<"MB/s\n";
    cout<<"Write size: "<<writesize/1024.0/1024.0<<"MB\n";
    cout<<"Organize compression ratio: "<<totalsize/1.0/writesize<<endl;
    cout<<"Organize compression efficiency: "<<((totalsize-writesize)/1.0/totalsize)<<endl;

    Timer RestoreTimer;
    uint64_t Rsize=0;

    for(int i=1;i<=versionNum;i++){
        string Command = "echo \"042230\" | sudo -S sh -c 'sync && echo 3 > /proc/sys/vm/drop_caches'";
        system(Command.c_str());

        Rsize+= restore(i,RestoreTimer);

        cout<<"Restore Finish # "<<i<<endl;
    }

    cout<<"Restore time: "<<RestoreTimer.timeval<<"S\n";
    cout<<"Restore data: "<<Rsize/1024.0/1024.0<<"MB\n";
    cout<<"Restore speed: "<<Rsize/RestoreTimer.timeval/1024.0/1024.0<<"MB/s\n";

#endif
    return 0;
}
