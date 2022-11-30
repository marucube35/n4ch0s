#ifndef PTABLE_H
#define PTABLE_H
#define MAX_PROCESS 10
#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

class PTable
{
private:
    BitMap *bm;
    PCB *pcb[MAX_PROCESS];
    int psize;
    Semaphore *bmsem; // dùng để ngăn chặn trường hợp nạp 2 tiến trình cùng lúc

public:
    PTable(int size); // Khởi tạo size đối tượng pcb để lưu size process.
                      // Gán giá trị ban đầu là null.
                      // Nhớ khởi tạo *bm và *bmsem để sử dụng
    ~PTable();        // Hủy các đối tượng đã tạo

    int ExecUpdate(char *name); // Xử lý cho system call SC_Exec
    int ExitUpdate(int ec);     // Xử lý cho system call SC_Exit
    int JoinUpdate(int id);     // Xử lý cho system call SC_Join

    int GetFreeSlot();         // Tìm free slot để lưu thông tin cho tiến trình mới
    bool IsExist(int pid);     // Kiểm tra tồn tại processID này không?
    void Remove(int pid);      // Khi tiến trình kết thúc, delete processID ra khỏi mảng quản lý nó
    char *GetFileName(int id); // Trả về tên của tiến trình
};

#endif