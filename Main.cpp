#include "Main.h"
#include<math.h>

#define PIE 3.141592653590

struct Vec3
{
	float x = 0;
	float y = 0;
	float z = 0;
};
struct Vec4 { float x, y, z, w; };
struct Vec2 { float x, y; };

BOOL GetPos(HANDLE hProcess, Vec3* pos, DWORD* dwHp, DWORD dwBaseAddress, DWORD i)
{
	DWORD dwTempAddress = 0;
	DWORD dwRetNum = 0;
	float dwTemp = 0;
	ReadProcessMemory(hProcess, (LPCVOID)(dwBaseAddress + 0x013ED5C4), &dwTempAddress, 4, &dwRetNum);
	//ReadProcessMemory(hProcess, (LPCVOID)(dwTempAddress + 0x168), &dwTempAddress, 4, &dwRetNum);
	//ReadProcessMemory(hProcess, (LPCVOID)(dwTempAddress + 0x4), &dwTempAddress, 4, &dwRetNum);
	ReadProcessMemory(hProcess, (LPCVOID)(dwTempAddress + 0x448), &dwTempAddress, 4, &dwRetNum);
	ReadProcessMemory(hProcess, (LPCVOID)(dwTempAddress + 0x560), &dwTempAddress, 4, &dwRetNum);
	ReadProcessMemory(hProcess, (LPCVOID)(dwTempAddress + 0x0 + 0x10 * i), &dwTempAddress, 4, &dwRetNum);
	ReadProcessMemory(hProcess, (LPCVOID)(dwTempAddress + 0x54), pos, 0xC, &dwRetNum);
	ReadProcessMemory(hProcess, (LPCVOID)(dwTempAddress + 0x5C + 0x2DC), dwHp, 12, &dwRetNum);
	return true;
}

DWORD GetpplNum(HANDLE hProcess, DWORD dwBaseAddress)
{
	DWORD dwTempAddress = 0;
	DWORD dwRetNum = 0;
	static DWORD dwTemp = 0;
	ReadProcessMemory(hProcess, (LPCVOID)(dwBaseAddress + 0x1381C90), &dwTemp, 4, &dwRetNum);

	return dwTemp;
}

BOOL AimBot()
{
	SetDebugConsole(L"Test");

	DWORD dwPid = GetProcessIdByProcessName(L"SFGame.exe");
	DWORD dwBaseAddr = GetProcessModuleBaseAddress(dwPid, L"SFGame.exe");
	if (dwBaseAddr == FALSE)
		return 0;
	DWORD dwTmp = 0;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	Sleep(50);

	DWORD dwHp = 0;
	DWORD dwTemp = 0;
	Vec3 Pos = { 0 };
	Vec3 MyPos = { 0 };
	Vec3 RePos = { 0 };
	int dwDis = 0;

	double Pitch = 0;
	double Yaw = 0;

	int GamePitch = 0;
	int GameYaw = 0;

	int TempPitch = 0;
	int TempYaw = 0;
	int tmp = 0;

	double dis = 0;

	BOOL bFlag = FALSE;

	while (TRUE)
	{
		if (GetKeyState(VK_MENU) & 0x8000 || GetKeyState(VK_LBUTTON) & 0x8000 || GetKeyState(VK_RBUTTON) & 0x8000)
		{
			for (size_t i = 0; i < GetpplNum(hProcess, dwBaseAddr) * 2; i++)
			{
				GetPos(hProcess, &Pos, &dwHp, dwBaseAddr, i);
				if (dwHp > 0 && dwHp < 1000)
				{
					//Get My Position
					ReadProcessMemory(hProcess, (LPCVOID)(dwBaseAddr + 0x013ED5C8), &dwTemp, 4, NULL);
					ReadProcessMemory(hProcess, (LPCVOID)(dwTemp + 0x54), &MyPos, 12, NULL);

					RePos.x = Pos.x - MyPos.x;
					RePos.y = Pos.y - MyPos.y;
					RePos.z = Pos.z - MyPos.z;
					//if (RePos.x != 0 & RePos.y != 0 & RePos.z != 0)
					//	continue;
					dis = sqrt((double)RePos.x * RePos.x + (double)RePos.y * RePos.y + (double)RePos.z * RePos.z);
					if (dis < 0.01 || dis > 2000)	continue;

					Pitch = atan2(RePos.y, RePos.x) * 180.f / PIE;
					double tmp1 = pow(RePos.x, 2) + pow(RePos.y, 2);
					Yaw = -atan2(-RePos.z, sqrt(tmp1)) * 180.f / PIE;

					//printf("%f\t%f\n", Pitch, Yaw);

					ReadProcessMemory(hProcess, (LPCVOID)(dwBaseAddr + 0x013ED5C4), &dwTmp, 4, NULL);
					ReadProcessMemory(hProcess, (LPVOID)(dwTmp + 0x64), &TempPitch, 4, NULL);
					ReadProcessMemory(hProcess, (LPVOID)(dwTmp + 0x60), &TempYaw, 4, NULL);

					GamePitch = (int)(Pitch * 65536 / 360) - 10;
					GameYaw = (int)(Yaw * 65536 / 360) - 10;

					GamePitch %= 65536;
					GameYaw %= 65536;
					if (GamePitch < 0)
						GamePitch += 65536;
					if (GameYaw < 0)
						GameYaw += 65536;
					//
					TempPitch %= 65536;
					TempYaw %= 65536;
					if (TempPitch < 0)
						TempPitch += 65536;
					if (TempYaw < 0)
						TempYaw += 65536;

					tmp = TempPitch - GamePitch;
					if (abs(tmp) > 1250)
						continue;
					tmp = TempYaw - GameYaw;
					if (abs(tmp) > 1250)	//2731
						continue;

					//printf("%d\n", TempPitch);
					//printf("%d\t%d\n", GamePitch, GameYaw);

					if (GamePitch != 0 && GameYaw != 0)
					{
						WriteProcessMemory(hProcess, (LPVOID)(dwTmp + 0x64), &GamePitch, 4, NULL);
						WriteProcessMemory(hProcess, (LPVOID)(dwTmp + 0x60), &GameYaw, 4, NULL);
					}
				}
			}
		}
	}
	CloseHandle(hProcess);
	return 0;
}

int main()
{
	AimBot();
	system("pause");
}