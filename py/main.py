from webui import webui

MyWindow = webui.window()
MyWindow.show('<html><script src="webui.js"></script> Hello World! </html>')
webui.wait()
