#ifndef PTABLE_H
#define PTABLE_H
#include "bitmap.h"
#include "pcb.h"
#include "synch.h"
#define MAX_PROCESS 10

class PTable
{
private:
    int psize;
    PCB *pcb[MAX_PROCESS];
    BitMap *bm;       //? Đánh dấu các vị trí đã được sử dụng trong pcb
    Semaphore *bmsem; // Dùng để ngăn chặn trường hợp nạp 2 tiến trình cùng lúc
                      // Nhớ khởi tạo bm và bmsem để sử dụng

public:
    PTable(int size); // Khởi tạo size đối tượng PCB để lưu size process.
                      // Gán giá trị ban đầu là của mỗi phần tử là null
    ~PTable();        // Giải phóng bộ nhớ

    int ExecUpdate(char *filename); // Xử lý cho system call SC_Exit
    int ExitUpdate(int exitcode);   // Xử lý cho system call SC_Exit
    int JoinUpdate(int pid);        // Xử lý cho system call SC_Join

    int GetFreeSlot();          // Tìm free slot để lưu thông tin cho tiến trình mới
    bool IsExist(int pid);      // Kiểm tra xem có tồn tại processID này không
    void Remove(int pid);       // Khi tiến trình kết thúc, delete processID ra khỏi mảng quản lý nó
    char *GetFileName(int pid); // Trả về tên của tiến trình
    void Print();               // In ra danh sách các tiến trình đang chạy
};

#endif