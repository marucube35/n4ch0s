#ifndef PCB_H
#define PCB_H
#include "synch.h"
#include "thread.h"

class PCB
{
private:
    Semaphore *joinsem; // semaphore cho quá trình join
    Semaphore *exitsem; // semaphore cho quá trình exit
    Semaphore *mutex;   // semaphore cho quá trình truy xuất độc quyền
    Thread *thread;
    int pid;
    int exitcode;
    int numwait; // số tiến trình đã join

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

    // Tăng giảm số tiến trình đã Join – với việc sử dụng biến mutex
    void IncNumWait(); // Tăng số tiến trình chờ
    void DecNumWait(); // Giảm số tiến trình chờ

    void SetExitCode(int ec) { exitcode = ec; } // Đặt exitcode của tiến trình
    int GetExitCode() { return exitcode; }      // Trả về exitcode

    void SetFileName(char *fn); // Đặt tên của tiến trình
    char *GetFileName();        // Trả về tên của tiến trình
};

#endif