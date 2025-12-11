#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <sstream>
#include <openssl/sha.h>
#include <dirent.h>
#include"thread_pool.h"
#include "FileOperator.h"

using namespace std;
struct SHA1FP{ //结构体存储指纹
    uint64_t fp1;
    uint32_t fp2,fp3,fp4;
};
struct FPHasher{ //哈希函数类，将SHA1FP类型的对象映射为一个哈希值；在这里直接从上一结构体中获取
    std::size_t operator()(const SHA1FP &key)const{
        return key.fp1;
    }
};
struct FPEqualer{
    bool operator()(const SHA1FP &lhs,const SHA1FP &rhs)const{
        return (lhs.fp1==rhs.fp1)&&(lhs.fp2==rhs.fp2)&&(lhs.fp3==rhs.fp3)&&(lhs.fp4==rhs.fp4);
    }
};
uint64_t totalSize=0;
uint64_t totalDedupSize=0;
uint64_t uniqueSize = 0;
uint64_t TotalFileNum = 0;

uint64_t File_100B_NuM = 0;
uint64_t File_500B_NuM = 0;
uint64_t File_1K_NuM = 0;
uint64_t File_2K_NuM = 0;
uint64_t File_4K_NuM = 0;
uint64_t File_8K_NuM = 0;
uint64_t File_16K_NuM = 0;
uint64_t File_32K_NuM = 0;
uint64_t File_64K_NuM = 0;
uint64_t File_LK_NuM = 0;

uint64_t File_100B_SIZE = 0;
uint64_t File_500B_SIZE = 0;
uint64_t File_1K_SIZE = 0;
uint64_t File_2K_SIZE = 0;
uint64_t File_4K_SIZE = 0;
uint64_t File_8K_SIZE = 0;
uint64_t File_16K_SIZE = 0;
uint64_t File_32K_SIZE = 0;
uint64_t File_64K_SIZE = 0;
uint64_t File_LK_SIZE = 0;

float File_100B_IO = 0;
float File_500B_IO = 0;
float File_1K_IO = 0;
float File_2K_IO = 0;
float File_4K_IO = 0;
float File_8K_IO = 0;
float File_16K_IO = 0;
float File_32K_IO = 0;
float File_64K_IO = 0;
float File_LK_IO = 0;

uint64_t TotalDedupFileNum = 0;
uint64_t DedupFile_100B_NuM = 0;
uint64_t DedupFile_500B_NuM = 0;
uint64_t DedupFile_1K_NuM = 0;
uint64_t DedupFile_2K_NuM = 0;
uint64_t DedupFile_4K_NuM = 0;
uint64_t DedupFile_8K_NuM = 0;
uint64_t DedupFile_16K_NuM = 0;
uint64_t DedupFile_32K_NuM = 0;
uint64_t DedupFile_64K_NuM = 0;
uint64_t DedupFile_LK_NuM = 0;

uint64_t DedupFile_100B_SIZE = 0;
uint64_t DedupFile_500B_SIZE = 0;
uint64_t DedupFile_1K_SIZE = 0;
uint64_t DedupFile_2K_SIZE = 0;
uint64_t DedupFile_4K_SIZE = 0;
uint64_t DedupFile_8K_SIZE = 0;
uint64_t DedupFile_16K_SIZE = 0;
uint64_t DedupFile_32K_SIZE = 0;
uint64_t DedupFile_64K_SIZE = 0;
uint64_t DedupFile_LK_SIZE = 0;

#define KB (1*1024)

std::unordered_map<SHA1FP,string ,FPHasher,FPEqualer> Sha1Map;

Timer IOTimer;
Timer DedupTimer;
Timer BufferTimer;

void showStatus(){
    cout<<"=============File status==============="<<endl;
    cout<<"Total File Num:"<<TotalFileNum<< " Total File Size:"<< totalSize/1024.0/1024.0 << "MB"<< endl;
    cout<<"100B File NUM:"<<File_100B_NuM<<  " 100B File Size:" << File_100B_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"500B File NUM:"<<File_500B_NuM<<  " 500B File Size:" << File_500B_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"1K File NUM:"<<File_1K_NuM<<  " 1K File Size:" << File_1K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"2K File NUM:"<<File_2K_NuM<<  " 2K File Size:" << File_2K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"4K File NUM:"<<File_4K_NuM<<  " 4K File Size:" << File_4K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"8K File NUM:"<<File_8K_NuM<<  " 8K File Size:" << File_8K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"16K File NUM:"<<File_16K_NuM<<  " 16K File Size:" << File_16K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"32K File NUM:"<<File_32K_NuM<<  " 32K File Size:" << File_32K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"64K File NUM:"<<File_64K_NuM<<  " 64K File Size:" << File_64K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"LK File NUM:"<<File_LK_NuM<<  " LK File Size:" << File_LK_SIZE/1024.0/1024.0 << "MB"<< endl;

    cout<<"===============IO status============="<<endl;
    IOTimer.showTime("Read IO ");
//    cout<<"Total IO:"<< IOTimer. << " Total File Size:"<< totalSize/1024.0/1024.0 << "MB"<< endl;
    cout<<"100B File IO:"<<File_100B_IO <<  "S" << endl;
    cout<<"500B File IO:"<<File_500B_IO<<  "S" << endl;
    cout<<"1K File IO:"<<File_1K_IO<<  "S"<< endl;
    cout<<"2K File IO:"<<File_2K_IO<<   "S"<< endl;
    cout<<"4K File IO:"<<File_4K_IO<<  "S"<< endl;
    cout<<"8K File IO:"<<File_8K_IO <<  "S"<< endl;
    cout<<"16K File IO:"<<File_16K_IO << "S"<< endl;
    cout<<"32K File IO:"<<File_32K_IO <<  "S"<< endl;
    cout<<"64K File IO:"<<File_64K_IO <<  "S"<< endl;
    cout<<"LK File IO:"<<File_LK_IO <<  "S"<< endl;
}

//遍历文件的时候，记录文件相关信息，文件数量，文件总大小，文件总IO时间
void addFile(uint64_t fsize,float IO_time){
    TotalFileNum++;
    if(fsize<100){
        File_100B_NuM++;
        File_100B_SIZE+=fsize;
        File_100B_IO+=IO_time;
    }
    else if(fsize < 500)
    {
        File_500B_NuM++;
        File_500B_SIZE+=fsize;
        File_500B_IO += IO_time;
    }
    else if(fsize < 1*KB)
    {
        File_1K_NuM++;
        File_1K_SIZE+=fsize;
        File_1K_IO += IO_time;
    }
    else if(fsize < 2*KB)
    {
        File_2K_NuM++;
        File_2K_SIZE+=fsize;
        File_2K_IO += IO_time;
    }
    else if(fsize < 4*KB)
    {
        File_4K_NuM++;
        File_4K_SIZE+=fsize;
        File_4K_IO += IO_time;
    }
    else if(fsize < 8*KB)
    {
        File_8K_NuM++;
        File_8K_SIZE+=fsize;
        File_8K_IO += IO_time;
    }
    else if(fsize < 16*KB)
    {
        File_16K_NuM++;
        File_16K_SIZE+=fsize;
        File_16K_IO += IO_time;
    }
    else if(fsize < 32*KB)
    {
        File_32K_NuM++;
        File_32K_SIZE+=fsize;
        File_32K_IO += IO_time;
    }
    else if(fsize < 64*KB)
    {
        File_64K_NuM++;
        File_64K_SIZE+=fsize;
        File_64K_IO += IO_time;
    }
    else
    {
        File_LK_NuM++;
        File_LK_SIZE+=fsize;
        File_LK_IO += IO_time;
    }
}

void addDedupFile(uint64_t fsize){
    TotalDedupFileNum++;
    totalDedupSize+=fsize;
    if(fsize < 100)
    {
        DedupFile_100B_NuM++;
        DedupFile_100B_SIZE+=fsize;
    }
    else if(fsize < 500)
    {
        DedupFile_500B_NuM++;
        DedupFile_500B_SIZE+=fsize;
    }
    else if(fsize < 1*KB)
    {
        DedupFile_1K_NuM++;
        DedupFile_1K_SIZE+=fsize;
    }
    else if(fsize < 2*KB)
    {
        DedupFile_2K_NuM++;
        DedupFile_2K_SIZE+=fsize;
    }
    else if(fsize < 4*KB)
    {
        DedupFile_4K_NuM++;
        DedupFile_4K_SIZE+=fsize;
    }
    else if(fsize < 8*KB)
    {
        DedupFile_8K_NuM++;
        DedupFile_8K_SIZE+=fsize;
    }
    else if(fsize < 16*KB)
    {
        DedupFile_16K_NuM++;
        DedupFile_16K_SIZE+=fsize;
    }
    else if(fsize < 32*KB)
    {
        DedupFile_32K_NuM++;
        DedupFile_32K_SIZE+=fsize;
    }
    else if(fsize < 64*KB)
    {
        DedupFile_64K_NuM++;
        DedupFile_64K_SIZE+=fsize;
    }
    else
    {
        DedupFile_LK_NuM++;
        DedupFile_LK_SIZE+=fsize;
    }
}

void showDedupStatus()
{
    cout<<"============dedup status================"<<endl;
    cout<<"Total DedupFile Num:"<<TotalDedupFileNum<< " Total DedupFile Size:"<< totalDedupSize/1024.0/1024.0 << "MB"<< endl;
    cout<<"100B DedupFile NUM:"<<DedupFile_100B_NuM<<  " 100B DedupFile Size:" << DedupFile_100B_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"500B DedupFile NUM:"<<DedupFile_500B_NuM<<  " 500B DedupFile Size:" << DedupFile_500B_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"1K DedupFile NUM:"<<DedupFile_1K_NuM<<  " 1K DedupFile Size:" << DedupFile_1K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"2K DedupFile NUM:"<<DedupFile_2K_NuM<<  " 2K DedupFile Size:" << DedupFile_2K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"4K DedupFile NUM:"<<DedupFile_4K_NuM<<  " 4K DedupFile Size:" << DedupFile_4K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"8K DedupFile NUM:"<<DedupFile_8K_NuM<<  " 8K DedupFile Size:" << DedupFile_8K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"16K DedupFile NUM:"<<DedupFile_16K_NuM<<  " 16K DedupFile Size:" << DedupFile_16K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"32K DedupFile NUM:"<<DedupFile_32K_NuM<<  " 32K DedupFile Size:" << DedupFile_32K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"64K DedupFile NUM:"<<DedupFile_64K_NuM<<  " 64K DedupFile Size:" << DedupFile_64K_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"LK DedupFile NUM:"<<DedupFile_LK_NuM<<  " LK DedupFile Size:" << DedupFile_LK_SIZE/1024.0/1024.0 << "MB"<< endl;
    cout<<"============================"<<endl;
}
//哈希1函数的实现
void sha1(const std::string &filepath, std::ofstream &hash_file){
    //打开文件
    std::ifstream file(filepath,std::ios::binary);

    if(TotalFileNum % 5000 == 0)
        showStatus();

    int size=0;

    if(!file){
        size=0;
        return;
    }
    IOTimer.startTime();

    FileOperator fileOperator((char *) filepath.c_str(),FileOpenType::Read);
    //读取文件的大小
    size=FileOperator::size((char *) filepath.c_str());
//    BufferTimer.startTime();
    uint8_t *buffer=(uint8_t*) malloc(size);
//    BufferTimer.endTime();
    //将文件内容读取到缓冲区
    fileOperator.read(buffer,size);

    IOTimer.endTime();

    DedupTimer.startTime();

    //计算哈希
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA_CTX sha1;
    SHA1_Init(&sha1); //初始哈希
    SHA1_Update(&sha1,buffer,size); //更新哈希计算
    SHA1_Final(digest,&sha1); //哈希计算结果存储在digest中

    if(size>100){
        hash_file<<filepath<<endl; //将文件路径写入hashfile
    }

    SHA1FP fp;
//    memcpy(&fp.fp1,digest,20);
// 将 digest 的前 8 字节存储到 fp1
    memcpy(&fp.fp1, digest, sizeof(fp.fp1));
    // 将剩余的 12 字节存储到 fp2, fp3, fp4
    memcpy(&fp.fp2, digest + sizeof(fp.fp1), sizeof(fp.fp2));
    memcpy(&fp.fp3, digest + sizeof(fp.fp1) + sizeof(fp.fp2), sizeof(fp.fp3));
    memcpy(&fp.fp4, digest + sizeof(fp.fp1) + sizeof(fp.fp2) + sizeof(fp.fp3), sizeof(fp.fp4));

    //过滤文件大小小于100B的文件


    totalSize+=size;
    uint64_t  fsize=size;
    addFile(fsize,IOTimer.onetimeval);

    //查找键fp
    auto t=Sha1Map.find(fp);

    //fp不存在映射中
    if(t==Sha1Map.end()){
        uniqueSize+=size;
        Sha1Map[fp]=filepath;
    }else{
        addDedupFile(fsize);
    }

    DedupTimer.endTime();

    free(buffer);

    return;
}

void traverse_directory(const string &path,const string &output_file,std::ofstream &hash_file){
    //打开目录
    DIR* dir=opendir(path.c_str());
    if(dir== nullptr){
        cerr<<"Error opening directory: "<<path<<endl;
        return;
    }

    //读取目录项
    struct  dirent* entry;
    while((entry= readdir(dir)) != nullptr){
        //跳过"."和".."
        if(strcmp(entry->d_name,".")==0|| strcmp(entry->d_name,"..")==0){
            continue;
        }

        //构建完整路径
        string entry_path=path+"/"+entry->d_name;
        //如果是目录，则递归遍历
        if(entry->d_type==DT_DIR){
            traverse_directory(entry_path,output_file,hash_file);
        }
        else if(entry->d_type==DT_REG){
            //如果是文件，则输出文件名
            try{
                int size;
                std::ifstream file(entry_path,std::ios::binary);
//                cout<<"Processing file "<<entry_path<<": "<<endl;
                sha1(entry_path,hash_file); //计算文件的hash
            }catch(const std::exception &e) {
                cerr<<"Error processing file "<<entry_path<<": "<<e.what()<<endl;
            }

        }
    }
    closedir(dir);//关闭目录

}

int main() {
   Timer allTimer;
   allTimer.startTime();

   string root_path="/home/lisa/data/nginx";
   string outfile="/home/lisa/list/nginx.list2";
   std::ofstream hash_file(outfile,ios::app);
   cout<<"File dedup path: "<<root_path<<endl;

   //用于写入哈希值的文件流
   if(!hash_file.is_open()){
       cerr<<"Error opening output file for writing: "<<outfile<<endl;
       exit(0);
   }
    traverse_directory(root_path,outfile,hash_file);

   showStatus();
   showDedupStatus();
   allTimer.endTime();
   allTimer.showTime("File Dedup");
    cout<<"TotalSize:"<< totalSize << " UniqueSize:" << uniqueSize << endl;
    cout<<"dedup rate: "<< totalSize/1.0/uniqueSize <<endl;
    cout << "File Dedup ALL time:" << (allTimer.timeval) << "s" << endl;
    cout<<"Search File Dedup time:"<< (DedupTimer.timeval) << "s" <<endl;
    cout<<"IO file time:"<< (IOTimer.timeval) << "s" <<endl;
    cout<<"IO Traverse time:"<< (allTimer.timeval - IOTimer.timeval - DedupTimer.timeval) << "s" <<endl;
    //BufferTimer.showTime("Malloc memory");
    cout << "File Dedup Speed:" << totalSize/1024.0/1024.0/ (allTimer.timeval) << " MB/s" << endl;
    return 0;
}
