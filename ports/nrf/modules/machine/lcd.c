/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Afantor.lan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "mphalport.h"
#include "lcd.h"
#include "lcdfont.h"
#include "bmp.h"
#include "mpconfigboard.h"


#if MICROPY_PY_MACHINE_LCD

uint16_t BACK_COLOR;   //背景色
uint8_t USE_HORIZONTAL = 3;

uint8_t LCD_W = 240;
uint8_t LCD_H = 135;

typedef struct _machine_lcd_obj_t {
    mp_obj_base_t base;
} machine_lcd_obj_t;

/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(uint8_t dat) 
{   
    uint8_t i;            
    for(i=0;i<8;i++)
    {             
        LCD_SCLK_Clr();
        if(dat&0x80)
           LCD_SDIN_Set();
        else 
           LCD_SDIN_Clr();
        LCD_SCLK_Set();
        dat<<=1;   
    }           
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(uint8_t dat)
{
    LCD_DC_Set();//写数据
    LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(uint16_t dat)
{
    LCD_DC_Set();//写数据
    LCD_Writ_Bus(dat>>8);
    LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(uint8_t dat)
{
    LCD_DC_Clr();//写命令
    LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    if(USE_HORIZONTAL==0)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+52);
        LCD_WR_DATA(x2+52);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+40);
        LCD_WR_DATA(y2+40);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==1)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+53);
        LCD_WR_DATA(x2+53);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+40);
        LCD_WR_DATA(y2+40);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==2)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+40);
        LCD_WR_DATA(x2+40);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+53);
        LCD_WR_DATA(y2+53);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==3)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+40);
        LCD_WR_DATA(x2+40);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+52);
        LCD_WR_DATA(y2+52);
        LCD_WR_REG(0x2c);//储存器写
    }
}

void lcd_init(uint8_t dir) {
    // uint8_t log[]= "MicroPython";
    nrf_gpio_cfg_output(LCD_SDA_PIN);       //配置管脚为输出
    nrf_gpio_cfg_output(LCD_SCL_PIN);
    nrf_gpio_cfg_output(LCD_RES_PIN);
    nrf_gpio_cfg_output(LCD_DC_PIN);
    nrf_gpio_cfg_output(LCD_CS_PIN);

    nrf_gpio_pin_set(LCD_SDA_PIN);
    nrf_gpio_pin_set(LCD_SCL_PIN);
    nrf_gpio_pin_set(LCD_RES_PIN);
    nrf_gpio_pin_set(LCD_DC_PIN);
    nrf_gpio_pin_clear(LCD_CS_PIN);
    mp_hal_delay_ms(10);
    LCD_RST_Clr();
    mp_hal_delay_ms(50);
    LCD_RST_Set();
    mp_hal_delay_ms(20);

    
    //************* Start Initial Sequence **********// 
    LCD_WR_REG(0x36); 
    USE_HORIZONTAL = dir;
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else if(USE_HORIZONTAL==3)LCD_WR_DATA8(0xA0);

    if (USE_HORIZONTAL==0 || USE_HORIZONTAL==1)
    {
        LCD_W = 135;
        LCD_H = 240;
    }
    else if (USE_HORIZONTAL==2 || USE_HORIZONTAL==3)
    {
        LCD_W = 240;
        LCD_H = 135;
    }
    LCD_WR_REG(0x3A); 
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33); 

    LCD_WR_REG(0xB7); 
    LCD_WR_DATA8(0x35);  

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x19);

    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x2C);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x12);   

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20);  

    LCD_WR_REG(0xC6); 
    LCD_WR_DATA8(0x0F);    

    LCD_WR_REG(0xD0); 
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2B);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x4C);
    LCD_WR_DATA8(0x18);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x0B);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x23);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2C);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x44);
    LCD_WR_DATA8(0x51);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x20);
    LCD_WR_DATA8(0x23);

    LCD_WR_REG(0x21); 

    LCD_WR_REG(0x11); 
    //Delay (120); 
    
    LCD_WR_REG(0x29); 
    mp_hal_delay_ms(20);
    LCD_Clear(LCD_WHITE);
    BACK_COLOR = LCD_WHITE;
    LCD_ShowLogo();
    // LCD_ShowPicture(99,37,138,76);
    // LCD_ShowString(76,85,log,LCD_RED);
}

/******************************************************************************
      函数说明：LCD清屏函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Clear(uint16_t Color)
{
    uint16_t i,j;   
    LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);

    for(i=0;i<LCD_W;i++)
     {
      for (j=0;j<LCD_H;j++)
        {
            LCD_WR_DATA(Color);              
        }

      }
}

void LCD_ShowDisable(void)
{
    nrf_gpio_pin_set(LCD_CS_PIN);
}
void LCD_ShowEnable(void)
{
    nrf_gpio_pin_clear(LCD_CS_PIN);
}

/******************************************************************************
      函数说明：LCD显示汉字
      入口数据：x,y   起始坐标
                index 汉字的序号
                size  字号
      返回值：  无
******************************************************************************/
void LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t index,uint8_t size,uint16_t color)   
{  
    uint8_t i,j;
    uint8_t *temp,size1;
    if(size==16){temp=Hzk16;}//选择字号
    if(size==32){temp=Hzk32;}
    LCD_Address_Set(x,y,x+size-1,y+size-1); //设置一个汉字的区域
    size1=size*size/8;//一个汉字所占的字节
    temp+=index*size1;//写入的起始位置
    for(j=0;j<size1;j++)
    {
        for(i=0;i<8;i++)
        {
            if((*temp&(1<<i))!=0)//从数据的低位开始读
            {
                LCD_WR_DATA(color);//点亮
            }
            else
            {
                LCD_WR_DATA(BACK_COLOR);//不点亮
            }
        }
        temp++;
    }
}


/******************************************************************************
      函数说明：LCD显示汉字
      入口数据：x,y   起始坐标
      返回值：  无
******************************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
    LCD_Address_Set(x,y,x,y);//设置光标位置 
    LCD_WR_DATA(color);
} 


/******************************************************************************
      函数说明：LCD画一个大的点
      入口数据：x,y   起始坐标
      返回值：  无
******************************************************************************/
void LCD_DrawBigPoint(uint16_t x,uint16_t y,uint16_t color)
{
    LCD_Fill(x-1,y-1,x+1,y+1,color);
} 


/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
      返回值：  无
******************************************************************************/
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{          
    uint16_t i,j; 
    LCD_Address_Set(xsta,ysta,xend,yend);      //设置光标位置 
    for(i=ysta;i<=yend;i++)
    {                                                           
        for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//设置光标位置      
    }                           
}


/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
      返回值：  无
******************************************************************************/
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
    uint16_t t; 
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //计算坐标增量 
    delta_y=y2-y1;
    uRow=x1;//画线起点坐标
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向 
    else if (delta_x==0)incx=0;//垂直线 
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//水平线 
    else {incy=-1;delta_y=-delta_x;}
    if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        LCD_DrawPoint(uRow,uCol,color);//画点
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}


/******************************************************************************
      函数说明：画矩形
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
      返回值：  无
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
      返回值：  无
******************************************************************************/
void LCD_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
    int a,b;
    a=0;b=r;      
    while(a<=b)
    {
        LCD_DrawPoint(x0-b,y0-a,color);             //3           
        LCD_DrawPoint(x0+b,y0-a,color);             //0           
        LCD_DrawPoint(x0-a,y0+b,color);             //1                
        LCD_DrawPoint(x0-a,y0-b,color);             //2             
        LCD_DrawPoint(x0+b,y0+a,color);             //4               
        LCD_DrawPoint(x0+a,y0-b,color);             //5
        LCD_DrawPoint(x0+a,y0+b,color);             //6 
        LCD_DrawPoint(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))//判断要画的点是否过远
        {
            b--;
        }
    }
}


/******************************************************************************
      函数说明：显示字符
      入口数据：x,y    起点坐标
                num    要显示的字符
                mode   1叠加方式  0非叠加方式
      返回值：  无
******************************************************************************/
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t mode,uint16_t color)
{
    uint8_t temp;
    uint8_t pos,t;
    uint16_t x0=x;     
    if(x>LCD_W-16||y>LCD_H-16)return;       //设置窗口         
    num=num-' ';//得到偏移后的值
    LCD_Address_Set(x,y,x+8-1,y+16-1);      //设置光标位置 
    if(!mode) //非叠加方式
    {
        for(pos=0;pos<16;pos++)
        { 
            temp=asc2_1608[(uint16_t)num*16+pos];        //调用1608字体
            for(t=0;t<8;t++)
            {                 
                if(temp&0x01)LCD_WR_DATA(color);
                else LCD_WR_DATA(BACK_COLOR);
                temp>>=1; 
                x++;
            }
            x=x0;
            y++;
        }   
    }
    else//叠加方式
    {
        for(pos=0;pos<16;pos++)
        {
            temp=asc2_1608[(uint16_t)num*16+pos];        //调用1608字体
            for(t=0;t<8;t++)
            {                 
                if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//画一个点     
                temp>>=1; 
            }
        }
    }                 
}


/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y    起点坐标
                *p     字符串起始地址
      返回值：  无
******************************************************************************/
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t color)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-16){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;LCD_Clear(LCD_RED);}
        LCD_ShowChar(x,y,*p,1,color);
        x+=8;
        p++;
    }  
}


/******************************************************************************
      函数说明：显示数字
      入口数据：m底数，n指数
      返回值：  无
******************************************************************************/
uint32_t mypow(uint8_t m,uint8_t n)
{
    uint32_t result=1;   
    while(n--)result*=m;    
    return result;
}


/******************************************************************************
      函数说明：显示数字
      入口数据：x,y    起点坐标
                num    要显示的数字
                len    要显示的数字个数
      返回值：  无
******************************************************************************/
void LCD_ShowNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t color)
{           
    uint8_t t,temp;
    uint8_t enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                LCD_ShowChar(x+8*t,y,' ',1,color);
                continue;
            }else enshow=1; 
             
        }
        LCD_ShowChar(x+8*t,y,temp+48,1,color); 
    }
} 


/******************************************************************************
      函数说明：显示小数
      入口数据：x,y    起点坐标
                num    要显示的小数
                len    要显示的数字个数
      返回值：  无
******************************************************************************/
void LCD_ShowFloat(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t color)
{           
    uint8_t t,temp;
    uint16_t num1;
    num1=num*100;
    for(t=0;t<len;t++)
    {
        temp=(num1/mypow(10,len-t-1))%10;
        if(t==(len-2))
        {
            LCD_ShowChar(x+8*(len-2),y,'.',1,color);
            t++;
            len+=1;
        }
        LCD_ShowChar(x+8*t,y,temp+48,1,color);
    }
}


/******************************************************************************
      函数说明：显示40x40图片
      入口数据：x,y    起点坐标
      返回值：  无
******************************************************************************/
void LCD_ShowPicture(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    int i;
    LCD_Address_Set(x1,y1,x2,y2);
    for(i=0;i<1600;i++)
    {   
        LCD_WR_DATA8(image[i*2+1]);  
        LCD_WR_DATA8(image[i*2]);           
    }           
}

/******************************************************************************
      函数说明：显示240x135图片
      入口数据：x,y    起点坐标
      返回值：  无
******************************************************************************/
void LCD_ShowLogo(void)
{
    int i;
    LCD_Address_Set(0,0,239,134);
    for(i=0;i<32400;i++)
    {   
        LCD_WR_DATA8(gImage_logo[i*2+1]);  
        LCD_WR_DATA8(gImage_logo[i*2]);           
    }           
}

/******************************************************************************
      函数说明：显示40x40图片
      入口数据：x,y    起点坐标
      返回值：  无
******************************************************************************/
void LCD_ShowBLE(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    int i;
    LCD_Address_Set(x1,y1,x2,y2);
    for(i=0;i<1600;i++)
    {   
        LCD_WR_DATA8(gImage_ble[i*2+1]);  
        LCD_WR_DATA8(gImage_ble[i*2]);           
    }           
}
/// \method __str__()
/// Return a string describing the lcd object.
STATIC void machine_lcd_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_lcd_obj_t *self = o;

    (void)self;

    mp_printf(print, "LCD Display");
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

STATIC mp_obj_t machine_lcd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    machine_lcd_obj_t *self = m_new_obj(machine_lcd_obj_t);
    
    self->base.type = &machine_lcd_type;

    return MP_OBJ_FROM_PTR(self);
}

/// \method init()
/// init lcd display.
STATIC mp_obj_t machine_lcd_init(mp_obj_t self_in, mp_obj_t dir_in) {
    machine_lcd_obj_t *self = self_in;
    uint8_t dir = mp_obj_get_int(dir_in);
    (void)self;
    lcd_init(dir);
    mp_hal_delay_ms(20);
    LCD_Clear(LCD_WHITE);
    BACK_COLOR = LCD_WHITE;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_lcd_init_obj, machine_lcd_init);

/// \method setRotation()
/// set lcd display Rotation.
STATIC mp_obj_t machine_lcd_setRotation(mp_obj_t self_in, mp_obj_t dir_in) {
    machine_lcd_obj_t *self = self_in;
    uint8_t dir = mp_obj_get_int(dir_in);
    (void)self;
    USE_HORIZONTAL = dir;

    if (USE_HORIZONTAL==0 || USE_HORIZONTAL==1)
    {
        LCD_W = 135;
        LCD_H = 240;
    }
    else if (USE_HORIZONTAL==2 || USE_HORIZONTAL==3)
    {
        LCD_W = 240;
        LCD_H = 135;
    }
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else if(USE_HORIZONTAL==3)LCD_WR_DATA8(0xA0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_lcd_setRotation_obj, machine_lcd_setRotation);

/// \method fillScreen()
/// fill screen lcd display.
STATIC mp_obj_t machine_lcd_fillScreen(mp_obj_t self_in, mp_obj_t color_in) {
    machine_lcd_obj_t *self = self_in;

    // get the character to write (might be 9 bits)
    uint16_t color = mp_obj_get_int(color_in);

    (void)self;
    LCD_Clear(color);
    BACK_COLOR = color;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_lcd_fillScreen_obj, machine_lcd_fillScreen);

/// \method clear()
/// clear lcd screen display by color.
STATIC mp_obj_t machine_lcd_clear(mp_obj_t self_in, mp_obj_t color_in) {
    machine_lcd_obj_t *self = self_in;

    // get the character to write (might be 9 bits)
    uint16_t color = mp_obj_get_int(color_in);

    (void)self;
    LCD_Clear(color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_lcd_clear_obj, machine_lcd_clear);

/// \method showChinese()
/// show chinese word, in parameters: x,y,index,size,color.
STATIC mp_obj_t machine_lcd_showChinese(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t index = mp_obj_get_int(args[3]);
    mp_int_t size = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    LCD_ShowChinese(x,y,index,size,color);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_showChinese_obj, 6, 6, machine_lcd_showChinese);

/// \method drawPoint()
/// draw point, in parameters x,y,color.
STATIC mp_obj_t machine_lcd_drawPoint(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);
    LCD_DrawPoint(x,y,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_drawPoint_obj, 4, 4, machine_lcd_drawPoint);

/// \method drawBigPoint()
/// draw big point, in parameters x,y,color.
STATIC mp_obj_t machine_lcd_drawBigPoint(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);
    LCD_DrawBigPoint(x,y,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_drawBigPoint_obj, 4, 4, machine_lcd_drawBigPoint);

/// \method fill()
/// draw fill area, in parameters x1,y1,x2,y2,color.
STATIC mp_obj_t machine_lcd_fill(size_t n_args, const mp_obj_t *args) {
    mp_int_t x1 = mp_obj_get_int(args[1]);
    mp_int_t y1 = mp_obj_get_int(args[2]);
    mp_int_t x2 = mp_obj_get_int(args[3]);
    mp_int_t y2 = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    LCD_Fill(x1,y1,x2,y2,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_fill_obj, 6, 6, machine_lcd_fill);

/// \method drawLine()
/// draw a line, in parameters x1,y1,x2,y2,color.
STATIC mp_obj_t machine_lcd_drawLine(size_t n_args, const mp_obj_t *args) {
    mp_int_t x1 = mp_obj_get_int(args[1]);
    mp_int_t y1 = mp_obj_get_int(args[2]);
    mp_int_t x2 = mp_obj_get_int(args[3]);
    mp_int_t y2 = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    LCD_DrawLine(x1,y1,x2,y2,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_drawLine_obj, 6, 6, machine_lcd_drawLine);

/// \method drawRectangle()
/// draw a rectangle, in parameters x1,y1,x2,y2,color.
STATIC mp_obj_t machine_lcd_drawRectangle(size_t n_args, const mp_obj_t *args) {
    mp_int_t x1 = mp_obj_get_int(args[1]);
    mp_int_t y1 = mp_obj_get_int(args[2]);
    mp_int_t x2 = mp_obj_get_int(args[3]);
    mp_int_t y2 = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    LCD_DrawRectangle(x1,y1,x2,y2,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_drawRectangle_obj, 6, 6,machine_lcd_drawRectangle);

/// \method drawCircle()
/// draw a Circle, in parameters x,y,rad,color.
STATIC mp_obj_t machine_lcd_drawCircle(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t r = mp_obj_get_int(args[3]);
    mp_int_t color = mp_obj_get_int(args[4]);
    LCD_DrawCircle(x,y,r,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_drawCircle_obj, 5, 5,machine_lcd_drawCircle);

/// \method showChar()
/// show a Char, in parameters x,y,char,mode,color.
STATIC mp_obj_t machine_lcd_showChar(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t c = mp_obj_get_int(args[3]);
    mp_int_t m = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    LCD_ShowChar(x,y,c,m,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_showChar_obj, 6, 6, machine_lcd_showChar);

/// \method showString()
/// show String, in parameters x,y,string,color.
STATIC mp_obj_t machine_lcd_showString(size_t n_args, const mp_obj_t *args) {
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
    mp_int_t color = mp_obj_get_int(args[4]);

    (void)self;

    LCD_ShowString(x,y,bufinfo.buf,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_showString_obj, 5, 5, machine_lcd_showString);

/// \method showNum()
/// show Num, in parameters x,y,Num,len,color.
STATIC mp_obj_t machine_lcd_showNum(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t n = mp_obj_get_int(args[3]);
    mp_int_t l = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    LCD_ShowNum(x,y,n,l,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_showNum_obj, 6, 6, machine_lcd_showNum);

/// \method showFloat()
/// show Float, in parameters x,y,Float,len,color.
STATIC mp_obj_t machine_lcd_showFloat(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_float_t f = mp_obj_get_float(args[3]);
    mp_int_t l = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    LCD_ShowFloat(x,y,f,l,color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_showFloat_obj, 6, 6, machine_lcd_showFloat);

/// \method showPicture()
/// show Picture, in parameters x1,y1
STATIC mp_obj_t machine_lcd_showPicture(size_t n_args, const mp_obj_t *args) {
    mp_int_t x1 = mp_obj_get_int(args[1]);
    mp_int_t y1 = mp_obj_get_int(args[2]);
    mp_int_t x2 = x1 + 39;
    mp_int_t y2 = y1 + 39;
    LCD_ShowPicture(x1,y1,x2,y2);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_showPicture_obj, 3, 3, machine_lcd_showPicture);

/// \method showLogo()
/// show Logo.
STATIC mp_obj_t machine_lcd_showLogo(mp_obj_t self_in) {
    machine_lcd_obj_t *self = self_in;
    (void)self;
    LCD_ShowLogo();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_lcd_showLogo_obj, machine_lcd_showLogo);

/// \method showBLE()
/// show BLE LOGO, in parameters x1,y1
STATIC mp_obj_t machine_lcd_showBLE(size_t n_args, const mp_obj_t *args) {
    mp_int_t x1 = mp_obj_get_int(args[1]);
    mp_int_t y1 = mp_obj_get_int(args[2]);
    mp_int_t x2 = x1 + 39;
    mp_int_t y2 = y1 + 39;
    LCD_ShowBLE(x1,y1,x2,y2);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_lcd_showBLE_obj, 3, 3, machine_lcd_showBLE);

/// \method showEnable()
/// show enable.
STATIC mp_obj_t machine_lcd_showEnable(mp_obj_t self_in) {
    machine_lcd_obj_t *self = self_in;
    (void)self;
    LCD_ShowEnable();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_lcd_showEnable_obj, machine_lcd_showEnable);

/// \method showDisable()
/// show disable.
STATIC mp_obj_t machine_lcd_showDisable(mp_obj_t self_in) {
    machine_lcd_obj_t *self = self_in;
    (void)self;
    LCD_ShowDisable();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_lcd_showDisable_obj, machine_lcd_showDisable);

STATIC const mp_rom_map_elem_t machine_lcd_locals_dict_table[] = {
    // instance methods
    // class methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_machine_lcd_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&mp_machine_lcd_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_setRotation), MP_ROM_PTR(&mp_machine_lcd_setRotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_fillScreen), MP_ROM_PTR(&mp_machine_lcd_fillScreen_obj) },
    { MP_ROM_QSTR(MP_QSTR_showChinese), MP_ROM_PTR(&mp_machine_lcd_showChinese_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawPoint), MP_ROM_PTR(&mp_machine_lcd_drawPoint_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawBigPoint), MP_ROM_PTR(&mp_machine_lcd_drawBigPoint_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&mp_machine_lcd_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawLine), MP_ROM_PTR(&mp_machine_lcd_drawLine_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawRectangle), MP_ROM_PTR(&mp_machine_lcd_drawRectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawCircle), MP_ROM_PTR(&mp_machine_lcd_drawCircle_obj) },
    { MP_ROM_QSTR(MP_QSTR_showChar), MP_ROM_PTR(&mp_machine_lcd_showChar_obj) },
    { MP_ROM_QSTR(MP_QSTR_showString), MP_ROM_PTR(&mp_machine_lcd_showString_obj) },
    { MP_ROM_QSTR(MP_QSTR_showNum), MP_ROM_PTR(&mp_machine_lcd_showNum_obj) },
    { MP_ROM_QSTR(MP_QSTR_showFloat), MP_ROM_PTR(&mp_machine_lcd_showFloat_obj) },
    { MP_ROM_QSTR(MP_QSTR_showPicture), MP_ROM_PTR(&mp_machine_lcd_showPicture_obj) },
    { MP_ROM_QSTR(MP_QSTR_showLogo), MP_ROM_PTR(&mp_machine_lcd_showLogo_obj) },
    { MP_ROM_QSTR(MP_QSTR_showBLE), MP_ROM_PTR(&mp_machine_lcd_showBLE_obj) },
    { MP_ROM_QSTR(MP_QSTR_showEnable), MP_ROM_PTR(&mp_machine_lcd_showEnable_obj) },
    { MP_ROM_QSTR(MP_QSTR_showDisable), MP_ROM_PTR(&mp_machine_lcd_showDisable_obj) },

    { MP_ROM_QSTR(MP_QSTR_WHITE),    MP_ROM_INT(LCD_WHITE) },
    { MP_ROM_QSTR(MP_QSTR_BLACK),     MP_ROM_INT(LCD_BLACK) },
    { MP_ROM_QSTR(MP_QSTR_BLUE),     MP_ROM_INT(LCD_BLUE) },
    { MP_ROM_QSTR(MP_QSTR_BRED),     MP_ROM_INT(LCD_BRED) },
    { MP_ROM_QSTR(MP_QSTR_GRED),     MP_ROM_INT(LCD_GRED) },
    { MP_ROM_QSTR(MP_QSTR_GBLUE),   MP_ROM_INT(LCD_GBLUE) },
    { MP_ROM_QSTR(MP_QSTR_RED),   MP_ROM_INT(LCD_RED) },
    { MP_ROM_QSTR(MP_QSTR_MAGENTA),   MP_ROM_INT(LCD_MAGENTA) },
    { MP_ROM_QSTR(MP_QSTR_GREEN),   MP_ROM_INT(LCD_GREEN) },
    { MP_ROM_QSTR(MP_QSTR_CYAN),   MP_ROM_INT(LCD_CYAN) },   
    { MP_ROM_QSTR(MP_QSTR_YELLOW),   MP_ROM_INT(LCD_YELLOW) },
    { MP_ROM_QSTR(MP_QSTR_BROWN),   MP_ROM_INT(LCD_BROWN) },
    { MP_ROM_QSTR(MP_QSTR_BRRED),   MP_ROM_INT(LCD_BRRED) },
    { MP_ROM_QSTR(MP_QSTR_GRAY),   MP_ROM_INT(LCD_GRAY) },
    { MP_ROM_QSTR(MP_QSTR_DARKBLUE),   MP_ROM_INT(LCD_DARKBLUE) },
    { MP_ROM_QSTR(MP_QSTR_LIGHTBLUE),   MP_ROM_INT(LCD_LIGHTBLUE) },
    { MP_ROM_QSTR(MP_QSTR_GRAYBLUE),   MP_ROM_INT(LCD_GRAYBLUE) },
    { MP_ROM_QSTR(MP_QSTR_IGHTGREEN),   MP_ROM_INT(LCD_IGHTGREEN) },
    { MP_ROM_QSTR(MP_QSTR_LGRAY),   MP_ROM_INT(LCD_LGRAY) },
    { MP_ROM_QSTR(MP_QSTR_LGRAYBLUE),   MP_ROM_INT(LCD_LGRAYBLUE) },
    { MP_ROM_QSTR(MP_QSTR_LBBLUE),   MP_ROM_INT(LCD_LBBLUE) },
};

STATIC MP_DEFINE_CONST_DICT(machine_lcd_locals_dict, machine_lcd_locals_dict_table);

const mp_obj_type_t machine_lcd_type = {
    { &mp_type_type },
    .name = MP_QSTR_LCD,
    .make_new = machine_lcd_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_lcd_locals_dict,
    .print = machine_lcd_print,
};

#endif // MICROPY_PY_MACHINE_LCD
