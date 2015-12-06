#include <Windows.h>
#include <winioctl.h>
#include <iostream>
#include <sstream>

void ExitPause(char* msg)
{
	std::cout << msg << std::endl;
	int pause = 0;
	std::cin >> pause;
	return;
}

void ExitPause(std::ostream ost)
{
	ost << std::endl;
	int pause = 0;
	std::cin >> pause;
	return;
}

void SetConsoleWindow(DISK_GEOMETRY diskGeom, int c, int t, int s)
{
	std::ostringstream consoleTitle;
	consoleTitle << "Cylinder: " << c << "/" << diskGeom.Cylinders.QuadPart << "|"
		<< "Track: " << t << "/" << diskGeom.TracksPerCylinder << "|"
		<< "Sector: " << s << "/" << diskGeom.SectorsPerTrack << "|";

	auto str = consoleTitle.str();
	std::wstring stemp = std::wstring(str.begin(), str.end());

	SetConsoleTitle(stemp.c_str());
}

int main(int argc, char* argv[])
{
	std::cout << "Hello" << std::endl << std::endl;

	LPWSTR driveToOpen = L"\\\\.\\G:";

	HANDLE driveHandle;

	driveHandle = CreateFile(driveToOpen, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (driveHandle == INVALID_HANDLE_VALUE)
	{
		ExitPause("Couldnt open drive!");
		return 0;
	}

	DWORD junk;
	DISK_GEOMETRY diskGeom;
	RETRIEVAL_POINTER_BASE rpBase;

	auto result = DeviceIoControl(driveHandle, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &diskGeom, sizeof(diskGeom), &junk, (LPOVERLAPPED)NULL);

	if (result) {

		std::cout << "Drive path      =" << driveToOpen << std::endl;
		std::cout << "Cylinders       =" << diskGeom.Cylinders.QuadPart << std::endl;
		std::cout << "Tracks/cylinder =" << diskGeom.TracksPerCylinder << std::endl;
		std::cout << "Sectors/track   =" << diskGeom.SectorsPerTrack << std::endl;
		std::cout << "Bytes/sector    =" << diskGeom.BytesPerSector << std::endl << std::endl;

		auto diskSize = diskGeom.Cylinders.QuadPart * (ULONG)diskGeom.TracksPerCylinder *
			(ULONG)diskGeom.SectorsPerTrack * (ULONG)diskGeom.BytesPerSector;

		std::cout << "Disk size       =" << diskSize << " (Bytes)" << std::endl;
		std::cout << "                =" << (double)diskSize / (double)(1024 * 1024 * 1024) << " (GB)" << std::endl;
	}
	else
	{
		ExitPause("Drive Info Failed!");
		return 0;
	}

	std::cout << "Locking drive!" << std::endl;
	result = DeviceIoControl(driveHandle, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &junk, (LPOVERLAPPED)NULL);
	if (result)
	{

		result = 1;//DeviceIoControl(driveHandle, FSCTL_GET_RETRIEVAL_POINTER_BASE, NULL, 0, &rpBase, sizeof(rpBase), &junk, (LPOVERLAPPED)NULL);
		
		if (result)
		{
			int bufferSize = diskGeom.BytesPerSector / sizeof(int);
			int* buffer = new int[bufferSize];
			for (int a = 0; a < bufferSize; ++a)
			{
				buffer[a] = 13107;
				//std::cout << std::hex << buffer[a] << " ";
			}



			DWORD bytesRead;
			DWORD bytesWritten;

			auto sfpResult = SetFilePointer(driveHandle, 0, NULL, FILE_BEGIN);

			if (sfpResult != INVALID_SET_FILE_POINTER)
			{
				for (int c = 0; c < diskGeom.Cylinders.QuadPart; ++c)
				{
					for (int t = 0; t < diskGeom.TracksPerCylinder; ++t)
					{
						for (int s = 0; s < diskGeom.SectorsPerTrack; ++s)
						{

							//ReadFile(driveHandle, buffer, diskGeom.BytesPerSector, &bytesRead, (LPOVERLAPPED)NULL);
							//for (int j = 0; j < bufferSize; ++j)
							//{
							//	std::cout << std::hex << buffer[j] << " ";
							//}

							SetConsoleWindow(diskGeom, c, t, s);

							WriteFile(driveHandle, buffer, diskGeom.BytesPerSector, &bytesWritten, (LPOVERLAPPED)NULL);
						}

					}
				}
			}
			else
			{
				std::cout << "Last Error: " << std::hex << (DWORD)(GetLastError()) << std::endl;
			}

			delete buffer;
		}
		else
		{
			std::cout << "Last Error: " << std::hex << (DWORD)(GetLastError()) << std::endl;
		}
	}
	else
	{
		ExitPause("Failed to lock volume");
		return 0;
	}

	std::cout << "Unlocking drive!" << std::endl;
	result = DeviceIoControl(driveHandle, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &junk, (LPOVERLAPPED)NULL);
	if (result)
	{
	}
	else
	{
		ExitPause("Failed to unlock volume");
	}

	CloseHandle(driveHandle);

	ExitPause("Done!");

	return 0;
}

