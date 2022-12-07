#ifndef PCB_H
#define PCB_H
#include "thread.h"
#include "synch.h"

class PCB
{
private:
    Semaphore *joinsem; // semaphore cho quá trình join
    Semaphore *exitsem; // semaphore cho quá trình exit
    Semaphore *multex;  // semaphore cho quá trình truy xuất độc quyền
    Thread *thread;     // thread của tiến trình
    int numwait;        // số tiến trình con đã join
    int pid;            // processID
    char *filename;     // tên tiến trình
    int exitcode;

public:
    int parentID; // ID của tiến trình cha
    PCB(int id);  // contructor
    ~PCB();       // deconstructor

    // Nạp chương trình có tên lưu trong biến filename và processID là pid
    int Exec(char *filename, int pid);         // Tạo 1 thread mới có tên là filename và process là pid
    int GetID();                               // Trả về ProcessID của tiến trình gọi thực hiện
    int GetNumWait() { return this->numwait; } // Trả về số lượng tiến trình chờ

    void JoinWait() { joinsem->P(); }       // 1. Tiến trình cha đợi tiến trình con kết thúc
    void ExitWait() { exitsem->V(); }    // 4. Tiến trình con kết thúc
    void JoinRelease() { joinsem->V(); }    // 2. Báo cho tiến trình cha thực thi tiếp
    void ExitRelease() { exitsem->V(); } // 3. Cho phép tiến trình con kết thúc

    void IncNumWait() { this->numwait++; } // Tăng số tiến trình chờ
    void DecNumWait() { this->numwait--; } // Giảm số tiến trình chờ

    void SetExitCode(int ec) { this->exitcode = ec; } // Đặt exitcode của tiến trình
    int GetExitCode() { return this->exitcode; }      // Trả về exitcode

    void SetFileName(char *fn) { this->filename = fn; } // Đặt tên của tiến trình
    char *GetFileName() { return this->filename; }      // Trả về tên của tiến trình
};

#endif