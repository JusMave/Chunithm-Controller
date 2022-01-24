#include <iostream>
#include <string>
#include <stdlib.h>
#include <SerialPort.h>

using namespace std;
const char* port = "\\\\.\\COM3";
char output[MAX_DATA_LENGTH];
char imcoming[MAX_DATA_LENGTH];

int main()
{
	SerialPort LEDcontrol(port);

	if (LEDcontrol.isConnected())
		cout << "Connection Success" << endl;
	else
		cout << "Connection Fail" << endl;

	while (LEDcontrol.isConnected())
	{
		string command;
		cin >> command;
		char* charArray = new char[command.size() + 1];
		copy(command.begin(), command.end(), charArray);
		charArray[command.size()] = '\n';

		LEDcontrol.writeSerialPort(charArray, MAX_DATA_LENGTH);
		LEDcontrol.readSerialPort(output, MAX_DATA_LENGTH);

		cout << output;

		delete[] charArray;
	}
	return 0;
}