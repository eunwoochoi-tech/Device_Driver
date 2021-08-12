#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys///ioctl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <linux///ioctl.h>

int main(int argc, char **argv) {
    FILE *y_fp = fopen("enc_sample_input_y.raw", "rb");
    if (y_fp == NULL) {
        printf("file open failed\n");
    }

    fseek(y_fp, 0, SEEK_END);
    int y_fileLen = ftell(y_fp);
    rewind(y_fp);

    uint8_t *y_buf = (uint8_t*)malloc(sizeof(uint8_t) * y_fileLen);
    fread(y_buf, sizeof(uint8_t), y_fileLen, y_fp);

    fclose(y_fp);

    FILE *u_fp = fopen("enc_sample_input_u.raw", "rb");
    if (u_fp == NULL) {
        printf("file open failed\n");
    }

    fseek(u_fp, 0, SEEK_END);
    int u_fileLen = ftell(u_fp);
    rewind(u_fp);

    uint8_t *u_buf = (uint8_t*)malloc(sizeof(uint8_t) * u_fileLen);
    fread(u_buf, sizeof(uint8_t), u_fileLen, u_fp);

    fclose(u_fp);

    FILE *v_fp = fopen("enc_sample_input_v.raw", "rb");
    if (v_fp == NULL) {
        printf("file open failed\n");
    }

    fseek(v_fp, 0, SEEK_END);
    int v_fileLen = ftell(v_fp);
    rewind(v_fp);

    uint8_t *v_buf = (uint8_t*)malloc(sizeof(uint8_t) * v_fileLen);
    fread(v_buf, sizeof(uint8_t), v_fileLen, v_fp);

    fclose(v_fp);

    int mem_fd = open("/dev/mem", O_RDWR);
    if (mem_fd < 0) {
        printf("mem open failed\n");
        return -1;
    } 

    void *p_mem = mmap(0, 0x100000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0x10000000);

    void *y_mem = mmap(0, 0x100000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0x12000000);
    void *u_mem = mmap(0, 0x100000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0x13000000);
    void *v_mem = mmap(0, 0x100000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0x14000000);

    memcpy(y_mem, y_buf, sizeof(uint8_t) * y_fileLen);
    memcpy(u_mem, u_buf, sizeof(uint8_t) * u_fileLen);
    memcpy(v_mem, v_buf, sizeof(uint8_t) * v_fileLen);

    void *reg_mem = mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0x43c01000);

    //uint8_t *result = (uint8_t*)malloc(sizeof(uint8_t) * 128 * 128);

    // [debug] h, v size
    *((unsigned int*)(reg_mem + 0x4))  = 0x00200020;
    usleep(10);

    // [debug] start address of bit stream(즉, output base addr)
    *((unsigned int*)(reg_mem + 0xc))  = 0x10000000;
    usleep(10);

    // [debug] memory read method(YUV)
    *((unsigned int*)(reg_mem + 0x2c))  = 0x00000000;
    usleep(10);

    // [debug] y address
    *((unsigned int*)(reg_mem + 0x30))  = 0x12000000;
    usleep(10);
    // [debug] u address
    *((unsigned int*)(reg_mem + 0x34))  = 0x13000000;
    usleep(10);
    // [debug] v address
    *((unsigned int*)(reg_mem + 0x38))  = 0x14000000;
    usleep(10);

    *((unsigned int*)(reg_mem + 0x3c))  = 0x00000000;
    usleep(10);

    // [debug] read hold count & big endian & start trigger
    *((unsigned int*)(reg_mem + 0x0))  = 0x00400021;
    usleep(10);

    printf("set done\n");

    sleep(3);

    // [debug] output size
    printf("out size %d\n", *((unsigned int*)(reg_mem + 0x28)));

    FILE *out_fp = fopen("out_dump.jpg", "wb");
    if (out_fp == NULL) {
        printf("failed to create out dump");
        return -1;
    }

    fwrite(p_mem, sizeof(uint8_t), *((unsigned int*)(reg_mem + 0x28)), out_fp);

    fclose(out_fp);

    munmap(p_mem, 0x100000);
    munmap(y_mem, 0x100000);
    munmap(u_mem, 0x100000);
    munmap(v_mem, 0x100000);
    munmap(reg_mem, 0x10000);

    close(mem_fd);
    
    printf("end\n");

    return 0;
}
