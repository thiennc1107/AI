#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>

#define LEFT 21 //Port 29 O4
#define RIGHT 22 //Port 31 O3
#define UP 23 //Port 33 O2
#define DOWN 24 //Port 35 O1


unsigned char Board[8] = {0};
unsigned char Change[8] = {0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b000000010, 0b00000001};
int SnakeLong = 5;
int Snake[5] = {43,33,23,13,03};
int SnakeStart[5]= {43,33,23,13,03};
int head = 53;//Snake[0]
int NewHead;
int tf;

void send_data(unsigned char address,unsigned char value)
{
   unsigned char data[2];
   data[0]=address;
   data[1]=value;
wiringPiSPIDataRW(0,data,2);
}

void DisplayResest(void)
{
   for(int i=0;i<8;i++) {send_data(i+1,0);};
}


void Init(void)
{
   send_data(0x09,0x00); //Decode mode: Code B decode for digits
   send_data(0x0a,0x08); //Intensity: 80% 
   send_data(0x0b,0x07); //Scan Limit: 8 
   send_data(0x0c,0x01); //Shutdown: 0 
   send_data(0x0f,0x00); //DisplayTest: No 
   DisplayResest(); // Reset screen
} 

void Startgame(void)
{
   wiringPiSPISetup(0,10000000);
   wiringPiSetup();
   Init();
   DisplayResest();
   for (int i=0;i<5;i++)
   {
        Snake[i]=SnakeStart[i];
   }
   for (int i=0;i<8;i++)
   {
        Board[i]=0;
   } 
   for (int i=0;i<5;i++) {Board[Snake[i]/10]=Board[Snake[i]/10]+Change[Snake[i]%10];}
   for(int i=0;i<8;i++) {send_data(i+1,Board[i]);}
}

void check(void)  // kiểm tra tong khung+kiểm tra đầu trúng thân
{
    tf = 1;
    for (int i=1;i<5;i++)
    {
        if (NewHead == Snake[i]) {tf = 0;}
    }
    if ((NewHead/10 <1) || (NewHead/10>7) || (NewHead%10<0) || (NewHead%10 >7))
    {tf = 0;}
}

void UpdateBoard(void)
{
    check();
    if (tf == 1)
    {
    Board[(Snake[4]/10)]=Board[(Snake[4]/10)]-Change[(Snake[4]%10)]; //xóa đuôi
    for(int i=4;i>-1;i--) {Snake[i]=Snake[i-1];} //cập nhật lại chuỗi rắn
    Snake[0]=NewHead;
    Board[(Snake[0]/10)]=Board[(Snake[0]/10)]+Change[(Snake[0]%10)]; //tạo đầu mới
    }
    else
    {
        for(int i=0;i<8;i++) {send_data(i+1,0xff);};
        delay(1000);
        Startgame();
    }
}

void UpdateScreen(void)
{
    UpdateBoard();
    for(int i=0;i<8;i++) {send_data(i+1,Board[i]);}
}

void PressLeft()
{
    NewHead = Snake[0] - 1;
    
     UpdateScreen();
}

void PressRight()
{
    NewHead = Snake[0] + 1;
    
     UpdateScreen();
}

void PressUp()
{
    NewHead = Snake[0] - 10;

     UpdateScreen();
}

void PressDown()
{
    NewHead = Snake[0] + 10;
    
     UpdateScreen();
}




int main(void)
 {   
    Startgame();
    wiringPiISR(LEFT, INT_EDGE_RISING, &PressLeft);
    wiringPiISR(RIGHT, INT_EDGE_RISING, &PressRight);
    wiringPiISR(UP, INT_EDGE_RISING, &PressUp);
    wiringPiISR(DOWN, INT_EDGE_RISING, &PressDown);
   while(1)
   {   
   }
 }
 

