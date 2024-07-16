#include<iostream>
#include<string>
#include<easyx.h>
#include<random>
#include<ctime>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
using namespace std;
const int window_Width = 500;
const int window_Height = 800;
const int IMGW = 50;
const int IMGH = 64;
const int MAX_NUM = 20;
IMAGE packet_img[2];
IMAGE bomp_img[2];
IMAGE bk_img;
//random_device r;可作为随机数种子
default_random_engine eng((unsigned int)time(0));//作为种子,返回无符号整型

class Dumplings
{ 
public:
	enum Type{Red_packet,Bomp}type;
	Dumplings();
	void initXY();
	int get_Y();
	void drawDumplings();
	void moveDumplings(int &score);
	bool isTouch(ExMessage &msg);
private:
	int m_x;
	int m_y;
	int m_speed;
	ExMessage m_msg;
};

Dumplings::Dumplings()
{
	initXY();
}

void Dumplings::initXY()
{
	uniform_int_distribution<int>a(0, window_Width - 50);//0到670
	this->m_x = a(eng);
	uniform_int_distribution<int>b(-800, -100);
	this->m_y = b(eng);
	uniform_int_distribution<int>c(1, 5);
	this->m_speed = c(eng);
	uniform_int_distribution<int>d(0, 100);
	if (d(eng) < 50)
		this->type = Red_packet;
	else
		this->type = Bomp;
}

int Dumplings::get_Y()
{
	return this->m_y;
}

void Dumplings::drawDumplings()
{
	switch (this->type)
	{
	case Red_packet:
		putimage(this->m_x, this->m_y, packet_img + 0,SRCPAINT);
		putimage(this->m_x, this->m_y, packet_img + 1,SRCAND);
		break;
	case Bomp:
		putimage(this->m_x, this->m_y, bomp_img + 0, SRCPAINT);
		putimage(this->m_x, this->m_y, bomp_img + 1, SRCAND);
		break;
	}
}

void Dumplings::moveDumplings(int &score)
{
	this->m_y += this->m_speed;
	if (this->m_y >= window_Height)
	{
		initXY();
		if(this->type==Dumplings::Red_packet)
			--score;
	}
}

bool Dumplings::isTouch(ExMessage& msg)
{
	if (msg.x >= this->m_x && msg.x <= this->m_x + IMGW)
		if (msg.y >= this->m_y && msg.y <= this->m_y + IMGH)
			return true;
	return false;
}

void showScore(int &score,int &time)
{
	setfillcolor(RGB(252, 148, 39));
	settextcolor(RGB(208, 40, 39));
	settextstyle(40, 0, "楷体");
	solidroundrect((window_Width-300)/2, 480, (window_Width - 300) / 2+300, 580,20,20);
	outtextxy((window_Width - 300) / 2 + 30, 490, "红包数：");
	outtextxy((window_Width - 300) / 2 + 200, 490,to_string(score).c_str());
	outtextxy((window_Width - 300) / 2 + 30, 530, "倒计时：");
	outtextxy((window_Width - 300) / 2 + 200, 530, to_string(60-(int)(time/1000)).c_str());

}

void gameInit()
{
	loadimage(packet_img + 0, "./images/red_packet_1.jpg", IMGW, IMGH);
	loadimage(packet_img + 1, "./images/red_packet_2.jpg", IMGW, IMGH);
	loadimage(bomp_img + 0, "./images/bomp_1.jpg", IMGW, IMGH);
	loadimage(bomp_img + 1, "./images/bomp_2.jpg", IMGW, IMGH);
	loadimage(&bk_img, "./images/background.jpg", window_Width, window_Height);
	mciSendString("open ./images/1.mp3 alias m", 0, 0, 0);
	mciSendString("play m", 0, 0, 0);
}

inline static bool hasMsg(ExMessage &msg) 
{
	return ::peekmessage(&msg, EX_MOUSE); 
}

int main()
{
	HWND hwnd = initgraph(window_Width, window_Height, EX_SHOWCONSOLE);
	mciSendString("open ./images/爆炸.mp3 alias b", 0, 0, 0);

	setbkmode(TRANSPARENT);
	ExMessage msg;
	gameInit();
	int score = 0;//得分
	int time ;//时间亦或者倒计时
	Dumplings dump[MAX_NUM];
	while (true)
	{
		time = clock();
		if (60 - (int)(time / 1000) == -1)
		{
			MessageBox(hwnd, "TimeIsOver", "Lost", MB_OK);
			return 0;
		}
		if (score <= -11)
		{
			MessageBox(hwnd, "GamOver", "Lost", MB_OK);
			return 0;
		}
		BeginBatchDraw();
		cleardevice();
		putimage(0, 0, &bk_img);
		showScore(score,time);
		for (int i = 0; i < MAX_NUM; i++)
		{
			dump[i].drawDumplings();
			dump[i].moveDumplings(score);
		}
		EndBatchDraw();
		while (hasMsg(msg))
		{			
			for (int i = 0; i < MAX_NUM; i++)
			{
				
				if (dump[i].isTouch(msg) && msg.message == WM_LBUTTONDOWN)
				{
					if (dump[i].type == Dumplings::Red_packet)
					{
						dump[i].initXY();
						++score;
					}
					else if (dump[i].type == Dumplings::Bomp)
					{
						MessageBox(hwnd, "GamOver", "Lost", MB_OK);
						return 0;
					}
				}
			}
		}
		Sleep(15);
	}
	closegraph();
	mciSendString("close m",0,0,0);
	system("pause");
	return 0;
}