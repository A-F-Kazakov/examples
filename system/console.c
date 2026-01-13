#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void console_set_cursor_position(short x, short y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void console_set_color(WORD color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }

void console_get_size(int* width, int* height)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	*width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void console_clear(HANDLE hStdOut)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hStdOut, &csbi);

	DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	DWORD count;
	COORD homeCoords = {0, 0};

	FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count);

	FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count);

	SetConsoleCursorPosition(hStdOut, homeCoords);
}

void console_wait_for_enter()
{
	printf("\nPress any key to continue...");
	_getch();
}

DWORD console_clear_line(HANDLE hnd, COORD pos, SHORT length)
{
	DWORD count = 0;
	FillConsoleOutputCharacter(hnd, ' ', length, pos, &count);
	// FillConsoleOutputAttribute(hnd, csbi.wAttributes, csbi.dwSize.X, pos, &count);
	return count;
}

void console_scroll_up(HANDLE hStdout, CONSOLE_SCREEN_BUFFER_INFO* info)
{
	SMALL_RECT scrollRect;
	COORD scrollTarget;
	CHAR_INFO fill;

	scrollRect.Left	= 0;
	scrollRect.Top		= 1;
	scrollRect.Right	= info->dwSize.X - 1;
	scrollRect.Bottom = info->dwSize.Y - 2;

	scrollTarget.X = 0;
	scrollTarget.Y = 0;

	fill.Char.AsciiChar = ' ';
	fill.Attributes	  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

	ScrollConsoleScreenBuffer(hStdout, &scrollRect, NULL, scrollTarget, &fill);
}

int main()
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hStdin	= GetStdHandle(STD_INPUT_HANDLE);

	printf("Basic Console API Example\n\n");

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(GetConsoleScreenBufferInfo(hStdout, &csbi))
	{
		printf("Console Information:\n");
		printf("  Window Size: %dx%d\n",
				 csbi.srWindow.Right - csbi.srWindow.Left + 1,
				 csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
		printf("  Buffer Size: %dx%d\n", csbi.dwSize.X, csbi.dwSize.Y);
		printf("  Cursor Position: (%d, %d)\n", csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y);
		printf("  Maximum Window Size: %dx%d\n", csbi.dwMaximumWindowSize.X, csbi.dwMaximumWindowSize.Y);
	}

	console_wait_for_enter();

	console_clear(hStdout);

	printf("Text Colors Demo:\n\n");

	console_set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("This is red text\n");

	console_set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("This is green text\n");

	console_set_color(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("This is blue text\n");

	console_set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("This is yellow text\n");

	console_set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("Back to normal color\n\n");

	console_wait_for_enter();

	console_clear(hStdout);

	printf("Cursor Control Demo:\n\n");

	CONSOLE_SCREEN_BUFFER_INFO savedPos;
	GetConsoleScreenBufferInfo(hStdout, &savedPos);

	console_set_cursor_position(10, 5);
	printf("Text at position (10, 5)");

	console_set_cursor_position(20, 8);
	printf("Text at position (20, 8)");

	SetConsoleCursorPosition(hStdout, savedPos.dwCursorPosition);

	printf("\n\nCursor returned to original position\n");

	console_wait_for_enter();

	console_clear(hStdout);

	printf("Input Mode Demo:\n\n");

	DWORD oldMode;
	GetConsoleMode(hStdin, &oldMode);

	SetConsoleMode(hStdin, ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT);

	printf("Type some characters (press 'q' to quit):\n");

	char buffer[256];
	int counter = 0;

	memset(buffer, 0, 256);

	GetConsoleScreenBufferInfo(hStdout, &csbi);

	while(1)
	{
		COORD pos = {0, csbi.dwSize.Y - 1};

		console_clear_line(hStdout, pos, csbi.dwSize.X - 1);

		SetConsoleCursorPosition(hStdout, pos);

		SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		printf("$ ");

		SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		for(int i = 0; i < counter; ++i)
			printf("%c", buffer[i]);

		int ch = _getch();

		buffer[counter++] = (char)ch;

		if(ch == VK_RETURN)
		{
			buffer[--counter] = '\0'; // skip \r
			counter				= 0;

			if(strcmp(buffer, "exit") == 0)
				break;

			if(csbi.dwCursorPosition.Y >= csbi.dwSize.Y - 2)
				console_scroll_up(hStdout, &csbi);
			else
				csbi.dwCursorPosition.Y++;

			SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
			printf("%s", buffer);
		}
		else if(ch == VK_BACK)
		{
			if(counter >= 2)
				counter -= 2;

			buffer[counter] = '\0';
		}
	}

	SetConsoleMode(hStdin, oldMode);

	console_wait_for_enter();

	console_clear(hStdout);

	printf("\n\nChanging console buffer size...\n");

	COORD newSize = {1200, 600};
	if(SetConsoleScreenBufferSize(hStdout, newSize))
		printf("Buffer size changed to 120x500\n");
	else
		printf("Failed to change buffer size. Error: %lu\n", GetLastError());

	char title[256];
	GetConsoleTitleA(title, sizeof(title));
	printf("\nCurrent console title: %s\n", title);

	SetConsoleTitleA("Console API Demo");

	printf("\nTitle changed!");

	console_wait_for_enter();

	return 0;
}
