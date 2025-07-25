// ClassPaper 独立设置程序 - 完整实现所有 WebUI 绑定函数
#include "webui.hpp"
#include "toml.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <algorithm>
#include <windows.h>

namespace fs = std::filesystem;

class ConfigManager
{
private:
    fs::path config_toml_path;
    fs::path config_js_path;

public:
    ConfigManager()
    {
        config_toml_path = "config.toml";
        config_js_path = "res/config/config.js";
    }

    // 读取 config.toml
    std::string loadTomlConfig()
    {
        try
        {
            if (fs::exists(config_toml_path))
            {
                std::ifstream file(config_toml_path);
                std::string content((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
                return content;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error reading config.toml: " << e.what() << std::endl;
        }

        // 返回默认配置
        return R"([default]
browser_path = ""
url = "./res/index.html"
)";
    }

    // 保存 config.toml
    bool saveTomlConfig(const std::string &browser_path, const std::string &url)
    {
        try
        {
            std::ofstream file(config_toml_path);
            file << "[default]\n";
            file << "browser_path = \"" << browser_path << "\"\n";
            file << "url = \"" << url << "\"\n";
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error saving config.toml: " << e.what() << std::endl;
            return false;
        }
    }

    // 扫描壁纸目录
    std::vector<std::string> scanWallpaperDir()
    {
        std::vector<std::string> wallpapers;
        fs::path wallpaper_dir = "res/wallpaper";

        try
        {
            if (fs::exists(wallpaper_dir) && fs::is_directory(wallpaper_dir))
            {
                for (const auto &entry : fs::directory_iterator(wallpaper_dir))
                {
                    if (entry.is_regular_file())
                    {
                        std::string filename = entry.path().filename().string();
                        std::string ext = entry.path().extension().string();

                        // 转换为小写进行比较
                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".gif")
                        {
                            wallpapers.push_back("wallpaper/" + filename);
                        }
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error scanning wallpaper directory: " << e.what() << std::endl;
        }

        return wallpapers;
    }
};

// 全局配置管理器
ConfigManager config_manager;

// 参考 main.go 实现的 WebUI 回调函数

// 读取配置 - 对应 main.go 中的 readConfig 绑定
void readConfig(webui::window::event *e)
{
    try
    {
        std::cout << "[设置] 读取配置..." << std::endl;

        // 读取 TOML 配置
        auto toml_content = config_manager.loadTomlConfig();
        auto config = toml::parse_str(toml_content);

        std::string browser_path = toml::find_or(config, "default", "browser_path", std::string(""));
        std::string url = toml::find_or(config, "default", "url", std::string("./res/index.html"));

        std::cout << "[设置] 读取到配置: URL=" << url << ", BrowserPath=" << browser_path << std::endl;

        // 构建 JSON 响应 - 与 main.go 返回格式一致
        std::string response = R"({
            "Default": {
                "URL": ")" + url +
                               R"(",
                "BrowserPath": ")" +
                               browser_path + R"("
            }
        })";

        e->return_string(response); // WebUI 2.5+ 标准响应方式
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 读取配置失败: " << ex.what() << std::endl;
        std::string error_response = R"({"error": ")" + std::string(ex.what()) + R"("})";
        e->return_string(error_response);
    }
}

// 保存配置 - 对应 main.go 中的 saveConfig 绑定
void saveConfig(webui::window::event *e)
{
    try
    {
        std::string data = e->get_string(); // 直接获取字符串参数
        std::cout << "[设置] 保存配置JSON: " << data << std::endl;

        // 解析 JSON 数据 - 与 main.go 解析方式一致
        auto getValue = [&](const std::string &key) -> std::string
        {
            std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]+)\"");
            std::smatch match;
            if (std::regex_search(data, match, pattern))
            {
                return match[1].str();
            }
            return "";
        };

        // 提取配置值
        std::string browser_path = getValue("BrowserPath");
        std::string url = getValue("URL");

        std::cout << "[设置] 解析配置: URL=" << url << ", BrowserPath=" << browser_path << std::endl;

        // 保存 TOML 配置
        if (!config_manager.saveTomlConfig(browser_path, url))
        {
            e->return_string(R"({"success": false, "error": "Failed to save TOML config"})");
            return;
        }

        std::cout << "[设置] 配置已保存" << std::endl;
        e->return_string(R"({"success": true})");
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 保存配置失败: " << ex.what() << std::endl;
        std::string error_response = R"({"success": false, "error": ")" + std::string(ex.what()) + R"("})";
        e->return_string(R"({"success": false, "error": ")" + std::string(ex.what()) + "\"}");
    }
}

// 写入文件 - 对应 main.go 中的 writeFile 绑定
void writeFile(webui::window::event *e)
{
    try
    {
        std::string path = e->get_string(0);
        std::string content = e->get_string(1);

        std::cout << "[设置] 写入文件: " << path << std::endl;

        // 确保目录存在
        fs::path file_path(path);
        fs::create_directories(file_path.parent_path());

        std::ofstream file(path);
        file << content;

        if (file.good())
        {
            std::cout << "[设置] 文件写入成功" << std::endl;
            e->return_string("true");
        }
        else
        {
            std::cout << "[设置] 文件写入失败" << std::endl;
            e->return_string("false");
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 写入文件异常: " << ex.what() << std::endl;
        e->return_string("false");
    }
}

// 读取文件 - 对应 main.go 中的 readFile 绑定
void readFile(webui::window::event *e)
{
    try
    {
        std::string path = e->get_string();
        std::cout << "[设置] 读取文件: " << path << std::endl;

        if (fs::exists(path))
        {
            std::ifstream file(path);
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            e->return_string(content);
        }
        else
        {
            std::cout << "[设置] 文件不存在: " << path << std::endl;
            e->return_string("");
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 读取文件异常: " << ex.what() << std::endl;
        e->return_string("");
    }
}

// 读取目录 - 对应 main.go 中的 readDir 绑定
void readDir(webui::window::event *e)
{
    try
    {
        std::string dir_path = e->get_string();
        std::cout << "[设置] 读取目录: " << dir_path << std::endl;

        std::vector<std::string> files;

        if (fs::exists(dir_path) && fs::is_directory(dir_path))
        {
            for (const auto &entry : fs::directory_iterator(dir_path))
            {
                files.push_back(entry.path().filename().string());
            }
        }

        // 构建 JSON 数组
        std::string response = "[";
        for (size_t i = 0; i < files.size(); ++i)
        {
            response += "\"" + files[i] + "\"";
            if (i < files.size() - 1)
            {
                response += ",";
            }
        }
        response += "]";

        std::cout << "[设置] 目录内容: " << response << std::endl;
        e->return_string(response);
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 读取目录异常: " << ex.what() << std::endl;
        e->return_string("[]");
    }
}

// 扫描壁纸目录 - 对应 main.go 中的 scanWallpaperDir 绑定
void scanWallpaperDir(webui::window::event *e)
{
    try
    {
        std::cout << "[设置] 扫描壁纸目录..." << std::endl;

        auto wallpapers = config_manager.scanWallpaperDir();

        // 构建与Go版本完全一致的JSON格式
        std::string response = "[";
        for (auto &wp : wallpapers)
        {
            response += "\"" + wp + "\",";
        }
        if (!wallpapers.empty())
            response.pop_back(); // 移除末尾逗号
        response += "]";
        e->return_string(response);
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 扫描壁纸目录异常: " << ex.what() << std::endl;
        e->return_string("[]");
    }
}

// 重新加载主窗口 - 对应 main.go 中的 reloadMainWindow 绑定
void reloadMainWindow(webui::window::event *e)
{
    try
    {
        std::cout << "[设置] 刷新主窗口请求" << std::endl;
        // 由于是独立程序，无法直接刷新主窗口，但返回成功状态
        e->return_string("true");
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 刷新主窗口异常: " << ex.what() << std::endl;
        e->return_string("false");
    }
}

// 在浏览器中打开URL - 对应 main.go 中的 openURLInBrowser 绑定
void openURLInBrowser(webui::window::event *e)
{
    try
    {
        std::string url = e->get_string();
        std::cout << "[设置] 在浏览器中打开: " << url << std::endl;

        // 处理本地文件路径，转换为完整的file://协议URL
        std::string processedUrl = url;
        if (!url.empty() && url.find("http://") != 0 && url.find("https://") != 0 && url.find("file://") != 0)
        {
            // 如果是相对路径，转换为绝对路径
            if (url.find("./") == 0 || url.find("res/") == 0)
            {
                fs::path currentPath = fs::current_path();
                fs::path fullPath;
                
                if (url.find("./") == 0)
                {
                    fullPath = currentPath / url.substr(2); // 移除 "./"
                }
                else
                {
                    fullPath = currentPath / url;
                }
                
                // 规范化路径并转换为file://协议
                fullPath = fs::weakly_canonical(fullPath);
                processedUrl = "file:///" + fullPath.string();
                
                // 替换反斜杠为正斜杠（Windows路径处理）
                std::replace(processedUrl.begin(), processedUrl.end(), '\\', '/');
                
                std::cout << "[设置] 转换后的URL: " << processedUrl << std::endl;
            }
        }

        int result = -1;

#ifdef _WIN32
        // 使用ShellExecute替代system命令，更安全
        HINSTANCE hResult = ShellExecuteA(NULL, "open", processedUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
        intptr_t resultCode = reinterpret_cast<intptr_t>(hResult);
        result = (resultCode > 32) ? 0 : -1;
        
        if (result != 0)
        {
            std::cout << "[设置] ShellExecute错误代码: " << resultCode << std::endl;
            // 常见错误代码说明
            switch (resultCode)
            {
                case 2: std::cout << "[设置] 错误: 找不到指定的文件" << std::endl; break;
                case 3: std::cout << "[设置] 错误: 找不到指定的路径" << std::endl; break;
                case 5: std::cout << "[设置] 错误: 访问被拒绝" << std::endl; break;
                case 8: std::cout << "[设置] 错误: 内存不足" << std::endl; break;
                case 31: std::cout << "[设置] 错误: 没有关联的应用程序" << std::endl; break;
                default: std::cout << "[设置] 错误: 未知错误" << std::endl; break;
            }
            
            // 尝试备用方案：使用system命令
            std::cout << "[设置] 尝试备用方案..." << std::endl;
            std::string command = "start \"\" \"" + processedUrl + "\"";
            std::cout << "[设置] 执行命令: " << command << std::endl;
            result = system(command.c_str());
            std::cout << "[设置] 备用方案结果: " << result << std::endl;
            
            // 如果还是失败，尝试直接打开浏览器
            if (result != 0)
            {
                std::cout << "[设置] 尝试直接启动浏览器..." << std::endl;
                // 尝试常见浏览器路径
                std::vector<std::string> browsers = {
                    "C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe",
                    "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe",
                    "C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe",
                    "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe"
                };
                
                for (const auto& browser : browsers)
                {
                    if (fs::exists(browser))
                    {
                        std::cout << "[设置] 找到浏览器: " << browser << std::endl;
                        std::string browserCommand = "\"" + browser + "\" \"" + processedUrl + "\"";
                        HINSTANCE hBrowserResult = ShellExecuteA(NULL, "open", browser.c_str(), processedUrl.c_str(), NULL, SW_SHOWNORMAL);
                        if (reinterpret_cast<intptr_t>(hBrowserResult) > 32)
                        {
                            result = 0;
                            std::cout << "[设置] 浏览器启动成功" << std::endl;
                            break;
                        }
                    }
                }
            }
        }
#elif __APPLE__
        std::string command = "open \"" + processedUrl + "\"";
        result = system(command.c_str());
#else
        std::string command = "xdg-open \"" + processedUrl + "\"";
        result = system(command.c_str());
#endif

        bool success = (result == 0);
        std::cout << "[设置] 打开浏览器" << (success ? "成功" : "失败") << std::endl;
        
        // 返回JSON格式以兼容前端
        std::string response = success ? 
            R"({"success": true})" : 
            R"({"success": false, "error": "Failed to open browser"})";
        e->return_string(response);
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 打开浏览器异常: " << ex.what() << std::endl;
        std::string error_response = R"({"success": false, "error": ")" + std::string(ex.what()) + R"("})";
        e->return_string(error_response);
    }
}

// 获取配置 - 为兼容性添加的函数
void getConfig(webui::window::event *e)
{
    try
    {
        std::cout << "[设置] 获取配置..." << std::endl;

        // 读取 TOML 配置
        auto toml_content = config_manager.loadTomlConfig();
        auto config = toml::parse_str(toml_content);

        std::string browser_path = toml::find_or(config, "default", "browser_path", std::string(""));
        std::string url = toml::find_or(config, "default", "url", std::string("./res/index.html"));

        // 构建简化的 JSON 响应
        std::string response = R"({
            "url": ")" + url + R"(",
            "browser_path": ")" +
                               browser_path + R"("
        })";

        e->return_string(response);
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 获取配置失败: " << ex.what() << std::endl;
        std::string error_response = R"({"error": ")" + std::string(ex.what()) + R"("})";
        e->return_string(error_response);
    }
}

// 在 ConfigManager 类后添加以下代码

void selectFile(webui::window::event *e)
{
    try
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = {0};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "HTML Files\0*.html;*.htm\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrTitle = "选择页面文件";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn))
        {
            std::cout << "[设置] 文件选择成功: " << ofn.lpstrFile << std::endl;
            std::string response = R"({"success": true, "file": ")" + std::string(ofn.lpstrFile) + R"("})";
            e->return_string(response);
        }
        else
        {
            DWORD error = CommDlgExtendedError();
            if (error == 0)
            {
                // 用户取消选择
                std::cout << "[设置] 用户取消文件选择" << std::endl;
                e->return_string(R"({"success": false, "cancelled": true})");
            }
            else
            {
                std::cout << "[设置] 文件选择失败，错误代码: " << error << std::endl;
                e->return_string(R"({"success": false, "error": "File selection failed"})");
            }
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 文件选择异常: " << ex.what() << std::endl;
        e->return_string(R"({"success": false, "error": "Exception occurred"})");
    }
}

void selectBrowser(webui::window::event *e)
{
    try
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = {0};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "Executable Files\0*.exe\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrTitle = "选择浏览器程序";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn))
        {
            std::cout << "[设置] 浏览器选择成功: " << ofn.lpstrFile << std::endl;
            std::string response = R"({"success": true, "browser": ")" + std::string(ofn.lpstrFile) + R"("})";
            e->return_string(response);
        }
        else
        {
            DWORD error = CommDlgExtendedError();
            if (error == 0)
            {
                // 用户取消选择
                std::cout << "[设置] 用户取消浏览器选择" << std::endl;
                e->return_string(R"({"success": false, "cancelled": true})");
            }
            else
            {
                std::cout << "[设置] 浏览器选择失败，错误代码: " << error << std::endl;
                e->return_string(R"({"success": false, "error": "Browser selection failed"})");
            }
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 浏览器选择异常: " << ex.what() << std::endl;
        e->return_string(R"({"success": false, "error": "Exception occurred"})");
    }
}

// 前端兼容性函数 - saveSettings (简化版saveConfig)
void saveSettings(webui::window::event *e)
{
    try
    {
        std::string url = e->get_string(0);
        std::string browser_path = e->get_string(1);
        
        std::cout << "[设置] 保存设置: URL=" << url << ", BrowserPath=" << browser_path << std::endl;

        // 保存 TOML 配置
        if (!config_manager.saveTomlConfig(browser_path, url))
        {
            e->return_string(R"({"success": false, "error": "Failed to save TOML config"})");
            return;
        }

        std::cout << "[设置] 设置已保存" << std::endl;
        e->return_string(R"({"success": true})");
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 保存设置失败: " << ex.what() << std::endl;
        e->return_string(R"({"success": false, "error": ")" + std::string(ex.what()) + R"("})");
    }
}

// 前端兼容性函数 - loadSettings (简化版readConfig)
void loadSettings(webui::window::event *e)
{
    try
    {
        std::cout << "[设置] 加载设置..." << std::endl;

        // 读取 TOML 配置
        auto toml_content = config_manager.loadTomlConfig();
        auto config = toml::parse_str(toml_content);

        std::string browser_path = toml::find_or(config, "default", "browser_path", std::string(""));
        std::string url = toml::find_or(config, "default", "url", std::string("./res/index.html"));

        std::cout << "[设置] 加载到设置: URL=" << url << ", BrowserPath=" << browser_path << std::endl;

        // 构建简化的 JSON 响应
        std::string response = R"({
            "success": true,
            "url": ")" + url + R"(",
            "browser_path": ")" + browser_path + R"("
        })";

        e->return_string(response);
    }
    catch (const std::exception &ex)
    {
        std::cout << "[设置] 加载设置失败: " << ex.what() << std::endl;
        std::string error_response = R"({"success": false, "error": ")" + std::string(ex.what()) + R"("})";
        e->return_string(error_response);
    }
}

// 调试函数 - 帮助诊断浏览器打开问题
void debugOpenBrowser(webui::window::event *e)
{
    try
    {
        std::string url = e->get_string();
        std::cout << "[调试] 开始调试浏览器打开功能..." << std::endl;
        std::cout << "[调试] 输入URL: " << url << std::endl;
        
        // 检查当前工作目录
        fs::path currentPath = fs::current_path();
        std::cout << "[调试] 当前工作目录: " << currentPath << std::endl;
        
        // 检查文件是否存在
        if (!url.empty() && url.find("http://") != 0 && url.find("https://") != 0)
        {
            fs::path filePath;
            if (url.find("./") == 0)
            {
                filePath = currentPath / url.substr(2);
            }
            else if (url.find("res/") == 0)
            {
                filePath = currentPath / url;
            }
            else
            {
                filePath = currentPath / url;
            }
            
            std::cout << "[调试] 检查文件路径: " << filePath << std::endl;
            std::cout << "[调试] 文件存在: " << (fs::exists(filePath) ? "是" : "否") << std::endl;
            
            if (fs::exists(filePath))
            {
                std::cout << "[调试] 文件大小: " << fs::file_size(filePath) << " 字节" << std::endl;
            }
        }
        
        // 检查可用的浏览器
        std::vector<std::string> browsers = {
            "C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe",
            "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe",
            "C:\\Program Files\\Microsoft\\Edge\\Application\\msedge.exe",
            "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe",
            "C:\\Program Files\\Mozilla Firefox\\firefox.exe",
            "C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe"
        };
        
        std::cout << "[调试] 检查可用浏览器:" << std::endl;
        for (const auto& browser : browsers)
        {
            bool exists = fs::exists(browser);
            std::cout << "[调试] " << browser << ": " << (exists ? "存在" : "不存在") << std::endl;
        }
        
        // 构建调试信息JSON
        std::string debugInfo = R"({
            "success": true,
            "currentPath": ")" + currentPath.string() + R"(",
            "url": ")" + url + R"(",
            "message": "Debug information collected"
        })";
        
        e->return_string(debugInfo);
    }
    catch (const std::exception &ex)
    {
        std::cout << "[调试] 调试异常: " << ex.what() << std::endl;
        std::string error_response = R"({"success": false, "error": ")" + std::string(ex.what()) + R"("})";
        e->return_string(error_response);
    }
}

// 在main函数中添加窗口样式设置
int main()
{
    // 创建 WebUI 窗口
    webui::window settings_window;

    std::cout << "[设置] 启动 ClassPaper 设置程序..." << std::endl;

    // 绑定所有需要的回调函数 - 参考 main.go 中的绑定
    std::cout << "[设置] 开始绑定函数..." << std::endl;

    // 配置管理相关函数
    settings_window.bind("readConfig", readConfig);
    settings_window.bind("saveConfig", saveConfig);
    settings_window.bind("getConfig", getConfig);
    
    // 文件操作相关函数
    settings_window.bind("writeFile", writeFile);
    settings_window.bind("readFile", readFile);
    settings_window.bind("readDir", readDir);
    
    // 壁纸管理相关函数
    settings_window.bind("scanWallpaperDir", scanWallpaperDir);
    
    // 系统操作相关函数
    settings_window.bind("reloadMainWindow", reloadMainWindow);
    settings_window.bind("openBrowser", openURLInBrowser);
    settings_window.bind("openURLInBrowser", openURLInBrowser); // 前端可能使用这个名字
    
    // 文件选择相关函数
    settings_window.bind("selectFile", selectFile);
    settings_window.bind("selectBrowser", selectBrowser);
    
    // 前端兼容性函数（settings.html中使用的函数名）
    settings_window.bind("saveSettings", saveSettings);
    settings_window.bind("loadSettings", loadSettings);
    
    // 调试函数
    settings_window.bind("debugOpenBrowser", debugOpenBrowser);

    std::cout << "[设置] 函数绑定完成" << std::endl;

    // 设置根目录并显示窗口
    settings_window.set_root_folder("res/");
    settings_window.set_size(800, 600); // 设置窗口尺寸
    settings_window.set_position(100, 100); // 设置窗口位置
    settings_window.show("settings.html");

    // 等待窗口加载完成（最多等待2秒）
    std::cout << "[设置] 开始等待窗口准备就绪..." << std::endl;
    for (int i = 0; i < 20; ++i)
    {
        if (settings_window.is_shown())
        {
            std::cout << "[设置] 窗口已就绪 (第 " << i+1 << " 次检测)" << std::endl;
            break;
        }
        std::cout << "[设置] 等待窗口准备就绪 (" << i + 1 << "/20) 当前状态: " 
                  << (settings_window.is_shown() ? "已显示" : "未显示") << std::endl;
        Sleep(100);
    }
    
    // 验证窗口状态
    if (settings_window.is_shown())
    {
        std::cout << "[设置] 窗口成功显示，开始设置窗口属性..." << std::endl;
        
        // 设置窗口标题
        settings_window.run("document.title = 'ClassPaper 设置';");
        
        // 确保窗口尺寸正确
        settings_window.set_size(800, 600);
        
        std::cout << "[设置] 窗口属性设置完成" << std::endl;
    }
    else
    {
        std::cout << "[警告] 窗口可能未正确显示，但继续运行..." << std::endl;
    }
    
    // 短暂延迟确保窗口完全加载
    Sleep(500);

    // 最终确认窗口尺寸
    settings_window.set_size(800, 600);

// #ifdef _WIN32
//     // 获取窗口句柄后立即设置样式
//     std::cout << "[设置] 开始获取窗口句柄..." << std::endl;
//     auto hwnd = (HWND)settings_window.win32_get_hwnd();
//     std::cout << "[设置] 通过webui接口获取句柄结果: " << (hwnd ? "成功" : "失败") 
//               << " 句柄值: " << hwnd << std::endl;

//     // 备选方案：通过窗口标题查找
//     if (!hwnd)
//     {
//         std::cout << "[设置] 正在尝试通过窗口标题查找句柄..." << std::endl;
//         const std::string targetTitle = "ClassPaper Settings";
        
//         std::cout << "[设置] 调用FindWindowA查找精确标题..." << std::endl;
//         hwnd = FindWindowA(NULL, targetTitle.c_str());
//         std::cout << "[设置] FindWindowA结果: " << (hwnd ? "找到" : "未找到") 
//                   << " 句柄值: " << hwnd << std::endl;

//         if (!hwnd) {
//             std::cout << "[设置] 开始深度搜索窗口 (EnumWindows回调)..." << std::endl;
//             EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
//                 char title[256] = {0};
//                 GetWindowTextA(hwnd, title, sizeof(title));
//                 std::cout << "[EnumWindows] 检查窗口: " << hwnd 
//                           << " 标题: " << title << std::endl;
                
//                 if (strstr(title, "ClassPaper Settings")) {
//                     *reinterpret_cast<HWND*>(lParam) = hwnd;
//                     std::cout << "[EnumWindows] 找到匹配窗口: " << hwnd << std::endl;
//                     return FALSE;
//                 }
//                 return TRUE;
//             }, reinterpret_cast<LPARAM>(&hwnd));
//             std::cout << "[设置] 深度搜索最终结果: " << (hwnd ? "成功" : "失败") 
//                       << " 句柄值: " << hwnd << std::endl;
//         }

//         if (hwnd) {
//             std::cout << "[设置] 找到备选窗口句柄，验证可见性..." << std::endl;
//             bool isVisible = IsWindowVisible(hwnd);
//             std::cout << "[设置] 窗口可见性: " << (isVisible ? "可见" : "不可见") 
//                       << " 句柄值: " << hwnd << std::endl;
//             if (!isVisible) {
//                 hwnd = nullptr;
//                 std::cout << "[设置] 因窗口不可见，已重置句柄" << std::endl;
//             }
//         }
//     }

//     // Windows API设置窗口样式
//     if (hwnd)
//     {
//         std::cout << "[设置] 开始设置窗口样式..." << std::endl;
        
//         // 方案一：通过WebUI配置禁用Chrome原生调整

//         settings_window.set_size(800, 600); // 强制设置窗口尺寸
//         //啊，这一行代码就行了？那我还弄那么多？

//         // settings_window.run(R"(
//         //     webui.setSize(800, 600);
//         //     webui.setMaxSize(800, 600);
//         //     webui.setMinSize(800, 600);
//         // )");

//         // 方案二：通过Windows API强制调整
//         // RECT rect = {0, 0, 800, 600};
//         // AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
        
//         // SetWindowPos(hwnd, NULL, 0, 0, 
//         //     rect.right - rect.left, 
//         //     rect.bottom - rect.top,
//         //     SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        
//         // // 移除调整尺寸的样式
//         // LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
//         // style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
//         // SetWindowLongPtr(hwnd, GWL_STYLE, style);
//     }

//     else
//     {
//         std::cout << "[设置] 错误：无法获取窗口句柄" << std::endl;
//     }
// #endif
    std::cout << "[设置] 设置窗口已打开，等待用户操作..." << std::endl;

    // 等待窗口关闭
    webui::wait();

    std::cout << "[设置] 设置窗口已关闭" << std::endl;

    return 0;
}