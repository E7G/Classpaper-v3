package main

import (
	"github.com/webui-dev/go-webui/v2"
	"github.com/getlantern/systray"
	"gopkg.in/ini.v1"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"io"
	"time"
	"math/rand"
	"syscall"
	"unsafe"
)

var (
	w webui.Window
	furl string
)


var user32 = syscall.NewLazyDLL("user32.dll")
var enumWindows = user32.NewProc("EnumWindows")
var getWindowText = user32.NewProc("GetWindowTextW")


func main() {


	defer onExit()
	
	// 获取程序执行目录
	exePath, err := os.Executable()
	if err != nil {
		log.Fatal("Error getting executable path:", err)
		return
	}

	// 获取程序所在目录
	programDir := filepath.Dir(exePath)

	// 切换工作目录至程序所在目录
	err = os.Chdir(programDir)
	if err != nil {
		log.Fatal("Error changing working directory:", err)
		return
	}
	
	

	// 打开或创建日志文件
	logFile, err := os.OpenFile("logfile.txt", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
	if err != nil {
		log.Fatal("Error opening log file:", err)
		return
	}
	defer logFile.Close()

	// 设置日志同时输出到文件和命令行
	log.SetOutput(io.MultiWriter(os.Stdout, logFile))
	
	// 获取当前工作目录
	currentDir, err := os.Getwd()
	if err != nil {
		log.Fatal("Error getting current working directory:", err)
		return
	}

	log.Println("Working directory changed to:", currentDir)
	
	// 获取配置文件路径
	furl, err := readAndConvertPathFromConfig()
	if err != nil {
		log.Fatal("Error:", err)
		return
	}
	log.Println("Converted URL:", furl)
	
	// 启动系统托盘
	go func() {
		systray.Run(onReady, onExit)
	}()

	startwebui()
	w = webui.NewWindow()
	//w.SetKiosk(true)
	//w.SetHide(true)
	w.SetRootFolder("res")
	w.Show(furl)
	//time.Sleep(time.Second)
	
	//webui.Wait()
	// 等待
	
	webui.Wait()
	gethwnd()
}


func startwebui(){
}

func gethwnd(){		
	rand.Seed(time.Now().UnixNano())
	// 生成6位随机字母数字
	randomString := generateRandomString(6)

	/*
	b,err := w.Script("document.title='"+randomString+"';return document.title;",webui.ScriptOptions{100,0})
	if err != nil {
		log.Fatal("Error:", err)
		onExit()
	}
	log.Println("document.title='"+randomString+"';\n返回："+b)
	
	w.Run("document.title='"+randomString+"'")
	log.Println("document.title='"+randomString+"'")
	*/
	w.Run("alert('hi')")
	log.Println("alert('hi');")
	
	
	//time.Sleep(time.Second)
	
	var hwndList []syscall.Handle

	// EnumWindows函数的回调函数
	callback := syscall.NewCallback(func(h syscall.Handle, p uintptr) uintptr {
		title, err := getWindowTextFunc(h)
		if err == nil && containsSubstring(title, randomString) {
			hwndList = append(hwndList, h)
		}
		return 1 // 继续枚举窗口
	})

	// 调用EnumWindows遍历所有窗口
	enumWindows.Call(callback, 0)

	// 输出包含指定字符串的窗口句柄
	log.Println("Window handles with title containing", randomString, ":", hwndList)

}

func getWindowTextFunc(hwnd syscall.Handle) (string, error) {
	const bufferSize = 256
	var buffer [bufferSize]uint16
	_, _, err := getWindowText.Call(uintptr(hwnd), uintptr(unsafe.Pointer(&buffer[0])), bufferSize)
	if err != nil {
		return "", err
	}
	return syscall.UTF16ToString(buffer[:]), nil
}


func containsSubstring(s, substr string) bool {
	return len(s) >= len(substr) && s[len(s)-len(substr):] == substr
}

func generateRandomString(length int) string {
	const charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

	randomString := make([]byte, length)
	for i := range randomString {
		randomString[i] = charset[rand.Intn(len(charset))]
	}

	return string(randomString)
}

func readAndConvertPathFromConfig() (string, error) {
	// 读取配置文件
	cfg, err := ini.Load("config.ini")
	if err != nil {
		return "", err
	}

	// 获取配置项
	path := cfg.Section("").Key("path").String()
	return path, nil

}




func onReady() {
	//systray.SetIcon(getIcon("icon.png"))
	systray.SetTooltip("Wallpaper App")

	// 添加菜单项
	reloadMenuItem := systray.AddMenuItem("Reload", "Reload the wallpaper")
	restartMenuItem := systray.AddMenuItem("Restart", "Restart the wallpaper app")
	quitMenuItem := systray.AddMenuItem("Quit", "Quit the wallpaper app")
	
	// 处理菜单项点击事件
	go func() {
		for {
			select {
			case <-reloadMenuItem.ClickedCh:
				// 在此处处理重新加载事件
				reloadWebpage()
			case <-restartMenuItem.ClickedCh:
				// 在此处处理重启事件
				restartApp()
			case <-quitMenuItem.ClickedCh:
				// 在此处处理退出事件
				systray.Quit()
			}
		}
	}()
	

	
	
}

func onExit() {
	
	// 在此处执行退出时的清理操作
	w.Close()
	webui.Exit()
}


func reloadWebpage() {
	// 在此处重新加载Web页面的内容
	// 可以根据需要执行相应的操作
	// Run JavaScript quickly with no waiting for the response
	//w.Run("location.reload(true)")
	w.Close()
    w.Show(furl)
	w.Run("location.reload(true)")
}

func restartApp() {
	executablePath, err := os.Executable()
	if err != nil {
		log.Fatal("Error:", err)
		return
	}
	// 在此处执行应用程序的重启操作
	cmd := exec.Command(executablePath)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Start()
	systray.Quit()
}


