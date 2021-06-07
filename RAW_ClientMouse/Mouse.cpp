/*
 *Overwatch Aim Assitance
 *Copyright (C) 2016  Juan Xuereb
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.

 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Mouse.h"
#include <vector>

char* intToBytes(int paramInt)
{
	std::vector<char> arrayOfByte(4);
	for (int i = 0; i < 4; i++)
		arrayOfByte[3 - i] = (paramInt >> (i * 8));
	return arrayOfByte.data();
}




Mouse::Mouse(SerialPort* pArduino)
{
	arduino = pArduino;
}

void Mouse::Send(int x, int y, int a, int b, int c, int d, int e, int f)
{
	arduino->writeSerialPort(&x, 1);
	arduino->writeSerialPort(&y, 1);
	arduino->writeSerialPort(&a, 1);
	arduino->writeSerialPort(&b, 1);
	arduino->writeSerialPort(&c, 1);
	arduino->writeSerialPort(&d, 1);
	arduino->writeSerialPort(&e, 1);
	arduino->writeSerialPort(&f, 1);
}

void Mouse::terminateThreads() {
	suspendThreads = (threads > 0) ? true : false;
}
