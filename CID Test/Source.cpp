#include <stdio.h>
#include <stdbool.h>
#include <string>

typedef struct cidMsg_struct
{
	unsigned char cidType;
	unsigned char dataLen;
	unsigned char dataType;
	unsigned char dateAndTimeSize;
	std::string date;
	std::string time; 
	char name[8]; 
	std::string phoneNumber;
	unsigned char chkSum;

} cidMsg_struct;

cidMsg_struct message;

#define ARR_SZ(x) (sizeof(x) / sizeof(x[0]))

bool validMsg = false;
bool SDMF = false;
bool MDMF = false;

bool isMsgValid(unsigned char data[], unsigned char len);
unsigned char DispResults();

void main()
{
	//SDMF DATA
	static unsigned char temp[] = { 0x04, 0x12, 0x30, 0x39, 0x33, 0x30, 0x31,
		0x32, 0x32, 0x34, 0x36, 0x30, 0x39, 0x35,
		0x35, 0x35, 0x31, 0x32, 0x31, 0x32, 0x51
	};

	validMsg = isMsgValid(temp, ARR_SZ(temp));
	if (!validMsg)
		return;

	message.cidType = temp[0];
	message.dataLen = temp[1];

	switch (message.cidType)
	{
	case 0x04: {
		SDMF = true;

		int cursorStart = 2;
		int cursor = cursorStart;
		for (; cursor < cursorStart + 4; cursor++) {
			if (cursor == cursorStart + 2)
				message.date += '/';
			message.date += (char)temp[cursor];
		}

		cursorStart = cursor;
		for (; cursor < cursorStart + 4; cursor++) {
			if (cursor == cursorStart + 2)
				message.time += ':';
			message.time += (char)temp[cursor];
		}

		cursorStart = cursor;
		for (; cursor < cursorStart + 10; cursor++) {
			if (cursor == (cursorStart + 3) || cursor == (cursorStart + 6))
				message.phoneNumber += '-';
			message.phoneNumber += temp[cursor];
		}

		message.chkSum = temp[(message.dataLen + 2)];
	}break;
	case 0x80:
		MDMF = true;
		message.dataType = temp[2];
		//Add code here to parse out MDMF data format.
		break;
	default:
		printf("[!] Unknown CID Type");
	}
	
	DispResults();
}

bool isMsgValid(unsigned char data[], unsigned char len) {
	unsigned char sum = 0;
	for (unsigned char i = 0; i < (len - 1); ++i) {
		sum += data[i] ;
	}

	//modulo 256 sum
	sum %= 256;

	//twos complement
	unsigned char twoscompl = ~sum + 1;

	return data[len - 1] == twoscompl;
}

unsigned char DispResults()
{
	unsigned char index = 0;
	if (validMsg)
	{
		printf("[+] CID Type: ");
		if (SDMF)
		{
			printf("SDMF\n");
		}
		else
		{
			printf("MDMF\n");
			printf("[+] Name: %s\n", message.name);
		}

		printf("[+] Date: %s\n", message.date.c_str());

		printf("[+] Time: %s\n", message.time.c_str());

		printf("[+] Phone Number: %s\n", message.phoneNumber.c_str());
	}
	else
	{
		printf("[!] Invalid ChkSum, Please Resend Message\n");
	}
	getchar();
	return 0;
}
