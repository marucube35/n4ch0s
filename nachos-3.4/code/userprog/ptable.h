#ifndef PTABLE_H
#define PTABLE_H
#define MAX_PROCESS 10
#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

// Lớp quản lý các tiến trình đang chạy,
// bản chất là một bảng (mảng) các tiến trình.
class PTable
{
private:
    int psize;             // kích thước của bảng các tiến trình
    PCB *pcb[MAX_PROCESS]; // lưu các pcb của các tiến trình
    BitMap *bm;            // lưu trạng thái của các tiến trình trong bảng các tiến trình (trống hoặc không trống)
    Semaphore *bmsem;      // dùng để ngăn chặn trường hợp nạp 2 tiến trình cùng lúc

public:
    // Khởi tạo 'size' đối tượng pcb để lưu 'size' process.
    PTable(int size);
    ~PTable();

    int ExecUpdate(char *name); // Xử lý cho system call SC_Exec
    int JoinUpdate(int pid);     // Xử lý cho system call SC_Join
    int ExitUpdate(int exitcode);     // Xử lý cho system call SC_Exit

    int GetFreeSlot();         // Tìm chỗ trống trong bảng để lưu thông tin cho tiến trình mới
    bool IsExist(int pid);     // Kiểm tra có tồn tại processID này không?
    void Remove(int pid);      // Khi tiến trình kết thúc, delete processID ra khỏi mảng quản lý nó
    char *GetFileName(int id); // Trả về tên của tiến trình
};

#endif