// CardProject.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include "ConvertUtil.h"
#include <fstream>
#include <map>
#include <time.h>

#define CPU_CARD 0
#define M1_CARD  1

#if M1_CARD
#include "../BHGX_HISReader/BHGX_HISReader.h"
#include "../BHGX_CardLib/BHGX_CardLib.h"

#ifdef _DEBUG
#pragma comment(lib, "../debug/BHGX_CardLib.lib")
#pragma comment(lib, "../debug/BHGX_HISReader.lib")
#else
#pragma comment(lib, "../release/BHGX_HISReader.lib")
#pragma comment(lib, "../release/BHGX_CardLib.lib")
#endif
#endif

#if CPU_CARD
//#include "../BHGX_HISReader/BHGX_HISReader.h"
#include "../BHGX_CPUCardLib/BHGX_CPUCardLib.h"
//#include "../BHGX_CreateCardData/BHGX_CreateCardData.h"

#ifdef _DEBUG
//#pragma comment(lib, "../debug/BHGX_CreateCardData.lib")
//#pragma comment(lib, "../debug/BHGX_HISReader.lib")
#pragma comment(lib, "../debug/BHGX_CPUCardLib.lib")
#else
#pragma comment(lib, "../release/BHGX_CPUCardLib.lib")
#pragma comment(lib, "../release/BHGX_CreateCardData.lib")
#endif
#endif

#pragma comment(lib, "tinyxml/libs/tinyxmld.lib")
#pragma warning (disable : 4996)
using namespace std;

typedef void (*procfunc)();

struct procItem {
	int index;
	procfunc func;
};

#define SAFE_DELETE(a)\
	if (a != NULL)\
{\
	delete a;\
	a = NULL;\
}\

static int WriteFile(char *filename, char *xml)
{
	FILE *handle;

	fopen_s(&handle, filename, "w");
	fwrite(xml, strlen(xml), 1, handle);
	fclose(handle);

	return 0;
}

static int ReadFile(char *filename, char **xml)
{
	FILE *handle;
	int size;

	fopen_s(&handle, filename, "r");

	if (NULL == handle)
		return -1;
	fseek(handle, 0, SEEK_END);
	size = ftell(handle) + 1;
	rewind(handle);
	*xml = (char *) malloc(size);
	memset(*xml, 0, size);
	fread(*xml,size,1,handle);
	fclose(handle);

	return size;
}

static void DbgMem(unsigned char *mem, int size)
{
	int i = 0;
	for(i=0; i< size; i++)
		printf("%02x ", mem[i]);
	printf("\n");
}


char szError[64];

void procCreateGWCard()
{
//	iCreateCardData("艾地热斯·莫明.txt", "北航冠新.license");
}

void procCreateNHCard()
{
//	iCreateCardData("艾地热斯·莫明.txt", "北航冠新.license");
}

void procRead() 
{
	char szRead[4096*2];
	memset(szError, 0, 64);
	memset(szRead, 0, sizeof(szRead));
	int index = 0, ret;
	cout<<"请选择读的区域,从第1块开始:"<<endl;
	cin>>index;
	ret = iReadInfo(0x1<<(index-1), szRead);
	//ret = iQueryInfo("CARDNO", szRead);
	if (ret != 0){
		strcpy(szError, err(ret));
		printf("%s\n", szError);
	}
	else{
		printf("%s\n", szRead);
	}
}

void procWrite()
{
	char *buf;
	char file[256];
	memset(szError, 0, sizeof(szError));
	cout<<"请输入写卡文件："<<endl;
	cin>>file;
	int ret = ReadFile(file, &buf);
	if (ret == -1) {
		printf("农合写卡数据.xml文件不存在\n");
		return;
	}
	ret = iWriteInfo(buf);
	printf("写操作:%d\n", ret);
	if (ret != 0)
	{
		strcpy(szError, err(ret));
		printf("%s\n", szError);
	}
	free(buf);
}

void procFormat()
{
	int nStart = GetTickCount();
	int ret = iFormatCard();
	int nEnd = GetTickCount();
	printf("iFormatCard: %d所需时间: %d ", ret, nEnd - nStart);
	if (ret != 0)
	{
		strcpy(szError, err(ret));
		printf("%s\n", szError);
	}
}

void procCreate()
{
	char *CreateXML;
	cout<<"制卡数据默认文件CreateCard.xml"<<endl;
	int ret = ReadFile("农合制卡数据.xml", &CreateXML);
	if (ret == -1) {
		printf("农合制卡数据.xml文件不存在\n");
		return;
	}
	printf("*************开始制卡*************\n");

	int nStart = GetTickCount();
	ret = iCreateCard(CreateXML);

	int nEnd = GetTickCount();
	printf("iCreateCard: %d所需时间: %d\n ", ret, nEnd - nStart);
	if (ret != 0)
	{
		strcpy(szError, err(ret));
		printf("%s\n", szError);
	}
	free(CreateXML);
}

void procReadHIS()
{
	char szRead[2048 * 4];
	char strCheckWSDL[100], strURL[100];
	memset(szRead, 0, sizeof(szRead));
	int ret = 0;
	ret = iReadHISInfo(strCheckWSDL, strURL, szRead);
	printf("%s", szRead);
	if (ret != 0)
	{
		strcpy(szError, err(ret));
		printf("%s\n", szError);
	}
}

void procRegNHMessage()
{
	char strCheckWSDL[100], strURL[100], szRead[1024*4];
	//cout<<"请输入服务器地址:"<<endl;
	//cin>>szURL;
	//cout<<"请输入WSDL文件地址:"<<endl;
	//cin>>szCheckWSDL;
	//ret = iRegMsgForNH((char*)strURL.c_str(), szRead);
	iCheckMsgForNH(strCheckWSDL, strURL, szRead);
	printf("%s", szRead);
}

void procPatchCard()
{
	char *CreateXML, *buf, *readBuf;
	char szPrinter[100];

	cout<<"制卡数据默认文件CreateCard.xml"<<endl;
	int ret = ReadFile("CreateCard.xml", &CreateXML);
	if (ret == -1) {
		printf("CreateCard.xml文件不存在\n");
		return;
	}

	cout<<"卡面打印数据默认文件cardprintData.xml"<<endl;
	ret = ReadFile("cardprintData.xml", &buf);
	if (ret == -1) {
		printf("cardprintData.xml文件不存在\n");
		return;
	}

	cout<<"卡面风格数据默认文件cardprint_652822.xml"<<endl;
	ret = ReadFile("cardprint_652822.xml", &readBuf);
	if (ret == -1) {
		printf("cardprint_652822.xml文件不存在\n");
		return;
	}

	printf("*************开始制卡*************\n");
	iGetPrinterList(szPrinter);
	printf("探测的打印机列表%s:\n", szPrinter);
	printf("***********************************\n");

	for(int i=0; i<4; ++i)
	{
		int nStart = GetTickCount();
		ret = iPatchCard(CreateXML,buf,"Zebra P330i Card Printer USB", readBuf);

		int nEnd = GetTickCount();
		printf("PatchCard: %d    所需时间: %d \n", ret, nEnd - nStart);
	}
	free(buf);
	free(readBuf);
	free(CreateXML);
}

void procPrintCard()
{
	char *buf, *readBuf;
	cout<<"卡面打印数据默认文件cardprintData.xml"<<endl;
	int ret = ReadFile("cardprintData.xml", &buf);
	if (ret == -1) {
		printf("cardprintData.xml文件不存在\n");
		return;
	}

	cout<<"卡面风格数据默认文件cardprint_652822.xml"<<endl;
	ret = ReadFile("cardprint_652822.xml", &readBuf);
	if (ret == -1) {
		printf("cardprint_652822.xml文件不存在\n");
		return;
	}
	printf("*************开始打印卡*************\n");

	printf("***********************************\n");
	//for(int i=0; i<vecPrinter.size(); ++i)
	//{
	int nStart = GetTickCount();
	ret = iPrintCard("", buf, readBuf);
	int nEnd = GetTickCount();
	printf("iPrintCard: %d    所需时间: %d \n", ret, nEnd - nStart);
	//}
	free(buf);
	free(readBuf);
}

void procIsEmpty()
{
	int ret = iCardIsEmpty();
	if (ret != 0)
	{
		strcpy(szError, err(ret));
		printf("%s\n", szError);
	}
}

void printHelp()
{
	cout<<"*************一卡通系统测试*********************"<<endl;
	cout<<"功能测试：\n\t1:公卫制卡数据\n\t2:农合制卡数据\n\t3:ReadInfo\n\t4:WriteInfo"<<endl;
	cout<<"\t5:FormateCard\n\t6:CreateCard\n\t7:ReadHISInfo\n\t8:卡注册\n\t9:PatchCard"<<endl;
	cout<<"\t10:PrintCard\n\t11:CardIsEmpty"<<endl;
}

procItem procMethods[] = {
	{0, procCreateGWCard},
	{1, procCreateNHCard},
	{2, procRead},
	{3, procWrite},
	{4, procFormat},
	{5, procCreate},
	{6, procReadHIS},
	{7, procRegNHMessage},
	{8, procPatchCard},
	{9, procPrintCard},
	{10, procIsEmpty},
	{11, NULL},
};

int main(int argc, char* argv[])
{
#define NR_MASK(nr) (1 << nr)
#define NOT_NR_MASK(nr) ~(1 << nr)

#define SETBIT(byte, nr) byte |= NR_MASK(nr)
#define CLRBIT(byte, nr) byte &= NOT_NR_MASK(nr)
	int flag = 0;
	SETBIT(flag, 0);
	cout<< flag <<endl;
	printHelp();
	char szSelect[10];
	cout<<"请输入功能测试选项:";
	cin>>szSelect;
	std::string strSelect = szSelect;
	int ret = iCardInit();
	if (ret != 0){
		cout<<"卡初始化失败:"<<ret<<endl;
		return -1;
	}
	while (1)
	{
		if (strSelect == "q" || strSelect == "quit")
			break;

		int nSelect = atoi(szSelect);
		nSelect -- ;
		if (nSelect < 0 || 
			nSelect > sizeof(procMethods)/sizeof(struct procItem) ) {
			printHelp();
		}else if (nSelect < 2) {
			procMethods[nSelect].func();
		} else {
			if (procMethods[nSelect].func != NULL) {
				procMethods[nSelect].func();
			}
		}

		memset(szSelect, 0 , sizeof(szSelect));
		cout<<"\n请输入功能测试选项:";
		cin>>szSelect;
		strSelect = szSelect;
	}
	iCardDeinit();
	getchar();
	return 0;
}

