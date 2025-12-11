//
// Created by 86183 on 2025/3/19.
//

#ifndef BISHE_ORGANIZE_H
#define BISHE_ORGANIZE_H
#include <iostream>
#include "thread_pool.h"
#include <openssl/sha.h>

using namespace std;

//限制参与去重的镜像层大小
#define MinLayerSize (1*1024*1024)
//限制参与去重的文件大小
#define MinFileSize (4*1024)

//合并的文件（镜像层集合共享的文件）最小值
#define MinCompactFileSize (4*1024)
#define LogForStatic
#define Restore

uint64_t FileDedup(uint8_t *buffer, uint64_t size, uint64_t version_id);

uint64_t organize(int layerNum, string outPutPath);

uint64_t archive(uint8_t *buffer, uint64_t size, uint64_t version_id, string outPutPath);

uint64_t archive_small(uint8_t *buffer, uint64_t size, uint64_t version_id, string outPutPath);

uint64_t restore(int versionId, Timer &restoreT);

//tar格式文件头部
/*
 * 文件头：每个归档的文件或目录都有一个固定大小的文件头（通常为 512 字节）。
文件内容：文件头之后是文件的实际内容。
块对齐：文件内容以 512 字节块对齐，不足的部分用零填充。
结束标志：归档文件以两个连续的零块结束。*/

struct Header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
};

constexpr std::uint32_t  BLOCK_SIZE = 512;

struct SHA1FP {
//    uint64_t fp1,fp2, fp3, fp4;
    uint64_t fp1;
    uint32_t fp2, fp3, fp4;


    bool operator<(const SHA1FP & other) const {
        if (fp1 != other.fp1) return fp1 < other.fp1;
        if (fp2 != other.fp2) return fp2 < other.fp2;
        if (fp3 != other.fp3) return fp3 < other.fp3;
        return fp4 < other.fp4;
    }

};

struct FPHasher {
    std::size_t
    operator()(const SHA1FP &key) const {
        return key.fp1;
    }
};

struct FPEqualer {
    bool operator()(const SHA1FP &lhs, const SHA1FP &rhs) const {
        return (lhs.fp1 == rhs.fp1) && (lhs.fp2 == rhs.fp2) && (lhs.fp3 == rhs.fp3) && (lhs.fp4 == rhs.fp4);
    }
};

//struct SHA256FP {
//    uint64_t fp1,fp2, fp3, fp4;
//
//
//    bool operator<(const SHA256FP & other) const {
//        if (fp1 != other.fp1) return fp1 < other.fp1;
//        if (fp2 != other.fp2) return fp2 < other.fp2;
//        if (fp3 != other.fp3) return fp3 < other.fp3;
//        return fp4 < other.fp4;
//    }
//
//    // 等于运算符
//    bool operator==(const SHA256FP& other) const {
//        return fp1 == other.fp1 && fp2 == other.fp2 && fp3 == other.fp3 && fp4 == other.fp4;
//    }
//};
//
//// 自定义哈希函数
//struct FPHasher {
//    std::size_t operator()(const SHA256FP& fp) const {
//        std::size_t seed = std::hash<uint64_t>()(fp.fp1);
//        seed ^= std::hash<uint64_t>()(fp.fp2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//        seed ^= std::hash<uint64_t>()(fp.fp3) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//        seed ^= std::hash<uint64_t>()(fp.fp4) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//        return seed;
//    }
//};
//
////计算哈希值
//SHA256FP computeSHA256(const unsigned char* buffer, size_t length) {
//    SHA256_CTX sha256;
//    SHA256_Init(&sha256);
//    SHA256_Update(&sha256, buffer, length);
//    unsigned char hash[SHA256_DIGEST_LENGTH];
//    SHA256_Final(hash, &sha256);
//
//    SHA256FP fp;
//    fp.fp1 = *(reinterpret_cast<const uint64_t*>(hash));
//    fp.fp2 = *(reinterpret_cast<const uint64_t*>(hash + 8));
//    fp.fp3 = *(reinterpret_cast<const uint64_t*>(hash + 16));
//    fp.fp4 = *(reinterpret_cast<const uint64_t*>(hash + 24));
//    return fp;
//}


#endif //BISHE_ORGANIZE_H
