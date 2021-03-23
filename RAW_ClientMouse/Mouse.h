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

#pragma once

#ifndef MOUSE_H
#define MOUSE_H

#include <Windows.h>
#include <iostream>
#include <string>
#include "SerialPort.h"

class Mouse
{
private:
	bool suspendThreads = false;
	int threads = 0;
	SerialPort* arduino;

public:
	Mouse(SerialPort * pArduino);

	void Send(int x, int y, int a, int b, int c, int d);
	void terminateThreads();

};

#endif // !MOUSE_H
