#include "process.hpp"
#include <iostream>

using namespace std;

#if !defined(_WIN32) || defined(MSYS_PROCESS_USE_SH)
const string PREFIX = "";
const string SLEEP = "sleep";
#else
const string PREFIX = "cmd /C ";
const string SLEEP = "timeout";
#endif

int main() {
  cout << "Example 1 - the mandatory Hello World" << endl;
  Process process1(PREFIX+"echo Hello World", "", [](const char *bytes, size_t n) {
    cout << "Output from stdout: " << std::string(bytes, n);
  });
  auto exit_status=process1.get_exit_status();
  cout << "Example 1 process returned: " << exit_status << " (" << (exit_status==0?"success":"failure") << ")" << endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  
  cout << endl << "Example 2 - cd into a nonexistent directory" << endl;
  Process process2(PREFIX+"cd a_nonexistent_directory", "", [](const char *bytes, size_t n) {
    cout << "Output from stdout: " << std::string(bytes, n);
  }, [](const char *bytes, size_t n) {
    cout << "Output from stderr: " << std::string(bytes, n);
    //add a newline for prettier output on some platforms:
    if(bytes[n-1]!='\n')
      cout << endl;
  });
  exit_status=process2.get_exit_status();
  cout << "Example 2 process returned: " << exit_status << " (" << (exit_status==0?"success":"failure") << ")" << endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  
  cout << endl << "Example 3 - async sleep process" << endl;
  std::thread thread3([]() {
    Process process3(SLEEP+" 5");
    auto exit_status=process3.get_exit_status();
    cout << "Example 3 process returned: " << exit_status << " (" << (exit_status==0?"success":"failure") << ")" << endl;
  });
  thread3.detach();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  
  
  cout << endl << "Example 4 - killing async sleep process after 5 seconds" << endl;
  auto process4=std::make_shared<Process>(SLEEP+" 10");
  std::thread thread4([process4]() {
    auto exit_status=process4->get_exit_status();
    cout << "Example 4 process returned: " << exit_status << " (" << (exit_status==0?"success":"failure") << ")" << endl;
  });
  thread4.detach();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  process4->kill();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  
#if !defined(_WIN32) || defined(MSYS_PROCESS_USE_SH)
  //Example 5 only works when having access to the sh command (for multiple commands)
  //On Windows: install MSYS2 (https://msys2.github.io)
  cout << endl << "Example 5 - multiple commands, stdout and stderr" << endl;
  Process process5("echo Hello && ls an_incorrect_path", "", [](const char *bytes, size_t n) {
    cout << "Output from stdout: " << std::string(bytes, n);
  }, [](const char *bytes, size_t n) {
    cout << "Output from stderr: " << std::string(bytes, n);
    //add a newline for prettier output on some platforms:
    if(bytes[n-1]!='\n')
      cout << endl;
  });
  exit_status=process5.get_exit_status();
  cout << "Example 5 process returned: " << exit_status << " (" << (exit_status==0?"success":"failure") << ")" << endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  
  //Example 6 only works when having access to the bash command
  //On Windows: install MSYS2 (https://msys2.github.io)
  cout << endl << "Example 6 - run bash with input from stdin" << endl;
  Process process6("bash", "", [](const char *bytes, size_t n) {
    cout << "Output from stdout: " << std::string(bytes, n);
  }, nullptr, true);
  process6.write("echo Hello from bash\n");
  process6.write("exit\n");
  exit_status=process6.get_exit_status();
  cout << "Example 6 process returned: " << exit_status << " (" << (exit_status==0?"success":"failure") << ")" << endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  
  //Example 7 only works when having access to the cat command
  //On Windows: install MSYS2 (https://msys2.github.io)
  cout << endl << "Example 7 - send data to cat through stdin" << endl;
  Process process7("cat", "", [](const char *bytes, size_t n) {
    cout << "Output from stdout: " << std::string(bytes, n);
  }, nullptr, true);
  process7.write("Hello cat\n");
  process7.close_stdin();
  exit_status=process7.get_exit_status();
  cout << "Example 7 process returned: " << exit_status << " (" << (exit_status==0?"success":"failure") << ")" << endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
#endif
  return 0;
}
