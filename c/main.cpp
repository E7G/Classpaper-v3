#include "include/webui.h"

int main() {

    size_t myWindow = webui_new_window();
    webui_show(myWindow, "<html><script src=\"webui.js\"></script> Hello World from C! </html>");
    webui_wait();
    return 0;
}