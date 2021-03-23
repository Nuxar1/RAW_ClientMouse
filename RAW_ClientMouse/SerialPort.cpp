
#include "SerialPort.h"
#include "xor.h"

SerialPort::SerialPort(std::string portName)
{
	connected = false;

	handler = CreateFileA(static_cast<LPCSTR>(portName.c_str()),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (handler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			printf(_xor_("ERROR: Handle was not attached. Reason: %s not available\n").c_str(), portName);
		}
		else
		{
			printf(_xor_("ERROR!!!").c_str());
		}
	}
	else {
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(handler, &dcbSerialParameters)) {
			printf(_xor_("failed to get current serial parameters").c_str());
		}
		else {
			dcbSerialParameters.BaudRate = CBR_56000;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(handler, &dcbSerialParameters))
			{
				printf(_xor_("ALERT: could not set Serial port parameters\n").c_str());
			}
			else {
				connected = true;
				PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}
}

SerialPort::~SerialPort()
{
	if (connected) {
		connected = false;
		CloseHandle(handler);
	}
}

int SerialPort::readSerialPort(char* buffer, unsigned int buf_size)
{
	DWORD bytesRead;
	unsigned int toRead;

	ClearCommError(handler, &errors, &status);

	if (status.cbInQue > 0) {
		if (status.cbInQue > buf_size) {
			toRead = buf_size;
		}
		else toRead = status.cbInQue;
	}

	if (ReadFile(handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

	return 0;
}

bool SerialPort::writeSerialPort(void* buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	if (!WriteFile(handler, (void*)buffer, buf_size, &bytesSend, 0)) {
		ClearCommError(handler, &errors, &status);
		return false;
	}
	else return true;
}

bool SerialPort::isConnected()
{
	return connected;
}