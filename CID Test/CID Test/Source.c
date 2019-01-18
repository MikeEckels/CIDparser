#include <stdio.h>
#include <stdbool.h>

typedef struct cidMsg_struct
{
	unsigned char cidType;
	unsigned char dataLen;
	unsigned char dataType;
	unsigned char dateAndTimeSize;
	unsigned char date[4]; //uint32_t, int, dword
	unsigned char time[4]; //uint32_t, int, dword
	unsigned char name[8]; //uint64_t, long, long long
	unsigned char phoneNumber[10]; //uint8_t
	unsigned char chkSum;

} cidMsg_struct;

cidMsg_struct message;
unsigned char * temp = NULL;//temporary arrayPtr for Struct

//unsigned char structSize = (sizeof(cidMsg_struct)) / (sizeof(unsigned char));
unsigned char dateAndTimeSize = (sizeof(message.date)) / (sizeof(unsigned char));
unsigned char phoneNumberSize = (sizeof(message.phoneNumber)) / (sizeof(unsigned char));
unsigned char nameSize = (sizeof(message.name)) / (sizeof(unsigned char));

bool sent, validMsg, SDMF, MDMF, dateSpacer, timeSpacer, phoneNumSpacer = false;

//Function Prototypes
bool isMsgValid(unsigned char data[], unsigned char len);
unsigned char DispResults();
unsigned char * CidComm();

void main()
{
	while (temp = (unsigned char *)CidComm()) //Loop untill null returned--casting to unsigned char*
	{
		message.cidType = temp[0];
		message.dataLen = temp[1];

		switch (message.cidType)
		{
		case 0x04:
			SDMF = true;
			for (unsigned char i = 0; i < ((dateAndTimeSize * 2) + phoneNumberSize); i++)
			{
				while (i < dateAndTimeSize)
				{
					message.date[i] = temp[2 + i];
					i++;
				}

				while (i >= dateAndTimeSize && i < (dateAndTimeSize * 2))
				{
					message.time[i - dateAndTimeSize] = temp[2 + i];
					i++;
				}
				message.phoneNumber[i - (dateAndTimeSize * 2)] = temp[2 + i];
			}
			message.chkSum = temp[(message.dataLen + 2)];
			break;

		case 0x80:
			MDMF = true;
			message.dataType = temp[2];
			//Add code here to parse out MDMF data format.
			break;
		default:
			printf("[!] Unknown CID Type");
		}
	}
	DispResults();
}

unsigned char *CidComm()
{
	//SDMF DATA
	unsigned char CidMsg[] = { 0x04, 0x12, 0x30, 0x39, 0x33, 0x30, 0x31,
							   0x32, 0x32, 0x34, 0x36, 0x30, 0x39, 0x35,
							   0x35, 0x35, 0x31, 0x32, 0x31, 0x32, 0x51
	};

	//MDMF DATA
	/*unsigned char CidMsg[] = { 0x80,0x15,0x1,0x8,0x30,0x33,0x31,0x35,0x31,0x30,0x33,0x30,
							   0x2,0x9,0x30,0x33,0x35,0x31,0x2d,0x33,0x32,0x31,0x30,0xe
	};*/

	unsigned char cidMsgSize = (sizeof(CidMsg) / sizeof(unsigned char));

	if (!sent)
	{
		validMsg = isMsgValid(CidMsg, cidMsgSize);

		if (validMsg)
		{
			sent = true;
			return &CidMsg[0];
		}
	}
	return (NULL);
}

bool isMsgValid(unsigned char data[], unsigned char len) {
	unsigned char sum = 0;
	for (unsigned char i = 0; i < (len - 1); ++i) {
		sum += data[i] ;
	}

	//modulo 256 sum
	sum %= 256;

	char chkSum = sum;

	//twos complement
	unsigned char twoscompl = ~chkSum + 1;

	return data[len - 1] == twoscompl;
}

unsigned char DispResults()
{
	unsigned char index = 0;
	if (validMsg)
	{
		printf("\n[+] CID Type: ");
		if (SDMF)
		{
			printf("SDMF");
		}
		else
		{
			printf("MDMF");
			printf("\n[+] Name: ");
			for (index = 0; index < nameSize; index++)
			{
				printf("%c", message.name[index]);
			}
		}

		printf("\n[+] Date: ");
		for (index = 0; index < dateAndTimeSize; index++)
		{
			if (index < 2)
			{
				printf("%c", message.date[index]);
			}
			else
			{
				if (!dateSpacer)
				{
					printf("/");
					dateSpacer = true;
				}
				printf("%c", message.date[index]);
			}
		}

		printf("\n[+] Time: ");
		for (index = 0; index < dateAndTimeSize; index++)
		{
			if (index < 2)
			{
				printf("%c", message.time[index]);
			}
			else
			{
				if (!timeSpacer)
				{
					printf(":");
					timeSpacer = true;
				}
				printf("%c", message.time[index]);
			}
		}

		printf("\n[+] Phone Number: ");
		for (index = 0; index < phoneNumberSize; index++)
		{
			if (index < 3)
			{
				printf("%c", message.phoneNumber[index]);
			}
			else
			{
				if (!phoneNumSpacer)
				{
					printf("-");
					phoneNumSpacer = true;
				}
				else if (index == 6)
				{
					printf("-");
				}
				printf("%c", message.phoneNumber[index]);
				phoneNumSpacer = true;
			}
		}
	}
	else
	{
		printf("\n[!] Invalid ChkSum, Please Resend Message");
	}
	getchar();
	return 0;
}
