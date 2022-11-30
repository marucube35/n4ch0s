#ifndef PCB_H
#define PCB_H
#include "synch.h"
#include "thread.h"

class PCB
{
private:
    Semaphore *joinsem; // semaphore cho quá trình join
    Semaphore *exitsem; // semaphore cho quá trình exit
    Semaphore *multex;  // semaphore cho quá trình truy xuất độc quyền
    Thread *thread;
    int pid;
    int exitcode = 0;
    int numwait = 0; // số tiến trình đã join
    char *filename;

public:
    int parentID; // ID của tiến trình cha
    PCB(int id);  // contructor
    ~PCB();       // deconstructor

    // Nạp chương trình có tên lưu trong biến filename và processID là pid
    int Exec(char *filename, int pid);   // Tạo 1 thread mới có tên là filename và process là pid
    int GetID() { return pid; }          // Trả về ProcessID của tiến trình gọi thực hiện
    int GetNumWait() { return numwait; } // Trả về số lượng tiến trình chờ

    // Hai hàm này để quản lý tiến trình Join
    void JoinWait(); // 1. Tiến trình cha đợi tiến trình con kết thúc
    void ExitWait(); // 2. Báo cho tiến trình cha thực thi tiếp

    // Hai hàm này để quản lý tiến trình Release
    void JoinRelease(); // 3. Cho phép tiến trình con kết thúc
    void ExitRelease(); // 4. Tiến trình con kết thúc

    // Tăng giảm số tiến trình đã Join – với việc sử dụng biến multex
    void IncNumWait(); // Tăng số tiến trình chờ
    void DecNumWait(); // Giảm số tiến trình chờ

    void SetExitCode(int ec) { exitcode = ec; } // Đặt exitcode của tiến trình
    int GetExitCode() { return exitcode; }      // Trả về exitcode

    void SetFileName(char *fn) { this->filename = fn; } // Đặt tên của tiến trình
    char *GetFileName() { return this->filename; }      // Trả về tên của tiến trình
};

#endif