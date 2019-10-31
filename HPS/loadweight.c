#include <stdio.h>
#include <dirent.h> 
#include <stdlib.h>
#include <stdint.h>

struct layer
{
    char* filename;
    int inputsize;
    int inputchannel;
    int outputchannel;
    int type;
    int relu;
    int stride;

    uint16_t* inputaddress;
    uint16_t* outputaddress;
    uint16_t* kerneladdress;



};

#define TYPE_CONV1x1 0
#define TYPE_CONV3x3D 1
#define TYPE_AVG_POOLING 2
#define TYPE_BATCH_NORM 3


#define USE_RELU 1
#define LINEAR 0
#define USE_BN 1

void padding(int& input_size,int input_channel,int stride,int& input_address,int& output_address,int base_address)
{
    int input_size2=input_size*input_size;
    int paddinput_size=(input_size+3-stride);
    int paddinput_size2=paddinput_size*paddinput_size;
    //output_address=input_address+input_size2*input_channel;
    if (stride==1)
    {
        for(int k=0;k<input_channel;k++)
        {
            for (int j=0;j<input_size+2;j++)
            {
                    *((uint32_t*)base_address+output_address+k*paddinput_size2+(0)*paddinput_size+(j))=0;
            }


            for (int i=0;i<input_size;i++)
            {
                *((uint32_t*)base_address+output_address+k*paddinput_size2+(i+1)*paddinput_size+(0))=0;
                for (int j=0;j<input_size;j++)
                {
                    *((uint32_t*)base_address+output_address+k*paddinput_size2+(i+1)*paddinput_size+(j+1))=*((uint32_t*)base_address+input_address+k*input_size2+i*input_size+j);
                }

                *((uint32_t*)base_address+output_address+k*paddinput_size2+(i+1)*paddinput_size+(input_size+1))=0;
            }

            for (int j=0;j<input_size+2;j++)
            {
                    *((uint32_t*)base_address+output_address+k*paddinput_size2+(input_size+1)*paddinput_size+(j))=0;
            }

        }
    }
    else if(stride==2)
    {
        if (stride==1)
    
        for(int k=0;k<input_channel;k++)
        {
            for (int i=0;i<input_size;i++)
            {
                for (int j=0;j<input_size;j++)
                {
                    *((uint32_t*)base_address+output_address+k*paddinput_size2+(i)*paddinput_size+(j))=*((uint32_t*)base_address+input_address+k*input_size2+i*input_size+j);
                }

                *((uint32_t*)base_address+output_address+k*paddinput_size2+(i)*paddinput_size+(input_size))=0;
            }

            for (int j=0;j<input_size+1;j++)
            {
                *((uint32_t*)base_address+output_address+k*paddinput_size2+(input_size)*paddinput_size+(j))=0;
            }

                
        }
    
    }

    int tmp=input_address;
    input_address=output_address;
    output_address=tmp;
    input_size=paddinput_size;
}


int set_layer(int input_size, int input_channel, int output_channel, int type,int bn,uint32_t* kernel_address_list,char* filename1,char* filename2, char* filename3,int& kernel_address,int base_address,int& kernel_count,int& file_count)
{
    int bn_size=0;
    int kernel_size=0;

    if(type==TYPE_CONV1x1)
    {
        if (bn==1)
        {
            kernel_size=input_channel*output_channel;
            bn_size=output_channel;
        }
        else if(bn==0)
        {
            kernel_size=input_channel*output_channel;
            bn_size=0;
        }

    }else if(type==TYPE_CONV3x3D)
    {
        kernel_size=3*3*input_channel;
        bn_size;
    }


    printf("%d save %s\n %s\n %s\n into %x\n",kernel_count,filename1,filename2,filename3, kernel_address);
    

    FILE* f;    
    f=fopen(filename1,"r");
    int data;
    for(int i=0;i<kernel_size;i++)
    {
        
        
        while(fscanf(f, "%x,", &data) == 1)
        {
            *((uint32_t*)base_address+kernel_address+i)=data; 
            //printf("%04x\n",data);
            
        }
            
    }
    fclose(f);

    if (bn==1)
    {
        FILE* f2;    
        f2=fopen(filename2,"r");
        int data2;
        for(int i=0;i<bn_size;i++)
        {
            
            
            while(fscanf(f, "%x,", &data) == 1)
            {
                *((uint32_t*)base_address+kernel_address+kernel_size+i)=data; 
                //printf("%04x\n",data);                
            }
                
        }
        fclose(f2);

        FILE* f3;    
        f3=fopen(filename3,"r");
        int data3;
        for(int i=0;i<bn_size;i++)
        {
            
            
            while(fscanf(f, "%x,", &data) == 1)
            {
                *((uint32_t*)base_address+kernel_address+kernel_size+bn_size+i)=data; 
                //printf("%04x\n",data);                
            }
                
        }
        fclose(f3);
    }
    kernel_address_list[kernel_count]=kernel_address;
    kernel_address+=kernel_size+bn_size*2;
    kernel_count+=1;

    if (bn==1)
    {
        file_count+=3;
    }
    else
    {
        file_count+=1;
    }
}

//input_size and stride doesn't matter when assign kernel weight, so just make it unchanged.
//these function inherent from previous design which consider feature map into memory control.

//int set_layer(int input_size, int input_channel, int output_channel, int type,int bn,int** kernel_address_list,char* filename1,char* filename2, char* filename3,int& kernel_address,int& base_address)
void set_depth3x3(int input_size,int input_channel,int stride,uint32_t* kernel_address_list, char** filenames,int& kernel_address,int base_address, int& kernel_count, int& file_count)
{
    set_layer(input_size,input_channel,input_channel,TYPE_CONV3x3D,0,kernel_address_list,filenames[file_count],NULL,NULL,kernel_address,base_address,kernel_count,file_count);

}

void set_point1x1(int input_size,int input_channel,int output_channel, uint32_t* kernel_address_list, char** filenames,int& kernel_address,int base_address, int& kernel_count, int& file_count)
{
    set_layer(input_size,input_channel,output_channel,TYPE_CONV1x1,1,kernel_address_list,filenames[file_count],filenames[file_count+1],filenames[file_count+2],kernel_address,base_address,kernel_count, file_count); 
}

void set_sep_conv2d(int input_size,int input_channel,int output_channel,int stride,uint32_t* kernel_address_list, char** filenames,int& kernel_address,int base_address, int& kernel_count, int& file_count)
{
    set_depth3x3(input_size, input_channel,stride, kernel_address_list,filenames,kernel_address,base_address,kernel_count,file_count);
    set_point1x1(input_size,input_channel,output_channel,kernel_address_list,filenames,kernel_address,base_address,kernel_count,file_count);
}

void set_fire_module(int input_size,int input_channel,int squeeze_channel,int expand_channel,uint32_t* kernel_address_list, char** filenames,int& kernel_address,int base_address, int& kernel_count, int& file_count)
{
    set_point1x1(input_size,input_channel,squeeze_channel,kernel_address_list,filenames,kernel_address,base_address,kernel_count,file_count);

    set_point1x1(input_size,squeeze_channel,expand_channel,kernel_address_list,filenames,kernel_address,base_address,kernel_count,file_count);
    set_sep_conv2d(input_size,squeeze_channel,expand_channel,1,kernel_address_list,filenames,kernel_address,base_address,kernel_count,file_count);

}

void fpga(int input_size, int inputchannel, int output_channel,int type, int stride,int relu,int input_address,int output_address,int kernel_address,void* send1,void* send2,void* send3,void* send4, void* receive)
{

}

void fpga_depth3x3(int& input_size,int input_channel, int stride, uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int& kernel_count,void* send1,void* send2,void* send3,void* send4, void* receive)
{
    padding(input_size,input_channel,stride,addr_ping,addr_pong,base_address) ;   
    printf("(%d,%d)\n",input_size,input_size);
    fpga(input_size,input_channel,input_channel,TYPE_CONV3x3D,stride,0,addr_ping,addr_pong,kernel_address_list[kernel_count],send1,send2,send3,send4,receive);
    input_size=(input_size-(2-stride)-1)/stride;
    int tmp=addr_ping;
    addr_ping=addr_pong;
    addr_pong=tmp;
    printf("execute layer d %d\n",kernel_count);
    kernel_count+=1;
}

void fpga_point1x1(int& input_size,int input_channel,int output_channel,int stride, int relu, uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int& kernel_count,void* send1,void* send2,void* send3,void* send4, void* receive)
{
    fpga(input_size,input_channel,output_channel,TYPE_CONV1x1,stride,relu,addr_ping,addr_pong,kernel_address_list[kernel_count],send1,send2,send3,send4,receive);
    int tmp=addr_ping;
    addr_ping=addr_pong;
    addr_pong=tmp;
    printf("execute layer p %d\n",kernel_count);
    kernel_count+=1;
}

void fpga_sep_3x3(int& input_size,int input_channel,int output_channel, int stride, uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int& kernel_count
    ,void* send1,void* send2,void* send3,void* send4, void* receive)
{
    fpga_depth3x3(input_size,input_channel,stride,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    fpga_point1x1(input_size,input_channel,output_channel,1,1,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
}


void fpga_fire_module(int& input_size,int input_channel,int squeeze_channel,int expand_channel, uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int& kernel_count
    ,void* send1,void* send2,void* send3,void* send4, void* receive)
{
    //printf("%x %x\n",addr_ping,addr_pong);
    fpga_point1x1(input_size,input_channel,squeeze_channel,1,1,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    //printf("%x %x\n",addr_ping,addr_pong);
    int tmp_addr=addr_ping; //preserve for conv3x3
    
    fpga_point1x1(input_size,squeeze_channel,expand_channel,1,1,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    //printf("after expand %x: after squeeze %x\n",addr_ping,addr_pong);
    addr_ping+=input_size*input_size*expand_channel;
    //addr_ping+=input_size*input_size*expand_channel;
    int tmp=addr_ping;
    addr_ping=addr_pong;
    addr_pong=tmp;
    //printf("%x %x\n",addr_ping,addr_pong);

    fpga_depth3x3(input_size,input_channel,1,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    //printf("%x %x\n",addr_ping,addr_pong);



    fpga_point1x1(input_size,squeeze_channel,expand_channel,1,1,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    //wait_concat=
    //printf("%x %x\n",addr_ping,addr_pong);
    addr_ping-=input_size*input_size*expand_channel;
    //printf("%x %x\n",addr_ping,addr_pong);

}

void fpga_average_pooling(int& input_size,int input_channel,uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int& kernel_count
    ,void* send1,void* send2,void* send3,void* send4, void* receive)
    {
        fpga(input_size,input_channel,input_channel,TYPE_AVG_POOLING,1,0,addr_ping,addr_pong,kernel_address_list[kernel_count],send1,send2,send3,send4,receive);
        input_size=1;
        int tmp=addr_ping;
        addr_ping=addr_pong;
        addr_pong=tmp;
    }

void read_image(int input_size,int base_address,int addr_ping)
{
    //cvreadimage
    //cvcameraread
}

void execute(uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int kernel_count,void* send1,void* send2,void* send3,void* send4, void* receive)
{
    int input_size=160;

    read_image(input_size,base_address,addr_ping);

    fpga_sep_3x3(input_size,3,96,2,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    printf("(%d,%d)\n",input_size,input_size);
    fpga_sep_3x3(input_size,96,96,1,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    printf("(%d,%d)\n",input_size,input_size);
    fpga_sep_3x3(input_size,96,192,2,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    printf("(%d,%d)\n",input_size,input_size);

    fpga_fire_module(input_size,192,16,64,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    fpga_fire_module(input_size,128,16,64,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    fpga_fire_module(input_size,128,32,128,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);

    fpga_sep_3x3(input_size,256,128,2,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);

    fpga_fire_module(input_size,128,32,128,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    fpga_fire_module(input_size,256,48,192,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    fpga_fire_module(input_size,384,48,192,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    fpga_fire_module(input_size,384,64,256,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);

    fpga_sep_3x3(input_size,512,256,2,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);

    fpga_fire_module(input_size,256,64,256,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);

    fpga_point1x1(input_size,512,1024,1,0,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    printf("(%d,%d)\n",input_size,input_size);
    fpga_average_pooling(input_size,1024,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    printf("(%d,%d)\n",input_size,input_size);
    fpga_point1x1(input_size,1024,512,1,0,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);

    /*
    set_sep_conv2d(160,3,96,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_sep_conv2d(80,96,96,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_sep_conv2d(80,96,192,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_fire_module(40,192,16,64,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(40,128,16,64,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(40,128,32,128,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_sep_conv2d(40,256,128,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_fire_module(20,128,32,128,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(20,256,48,192,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(20,384,48,192,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(20,384,64,256,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_sep_conv2d(20,512,256,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_fire_module(10,256,64,256,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_point1x1(10,512,1024,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    //avg-pooling doesnt need setting
    set_point1x1(1,1024,512,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    */

}



int main()
{

    DIR *d;
    FILE *f;
    uint32_t* base_address;

    base_address=new uint32_t [0x10000000];
    
    int output_offset=0;



    char c;
    uint32_t data;

    //Mat img;
    //img=cv::imread("PATHOFIMAGE");
    uint32_t* kernel_address_list;
    kernel_address_list= new uint32_t[100];
    struct dirent *dir;
    d=opendir(".\\weight160");
    dir = readdir(d);
    dir = readdir(d);


    char** filenames;
    filenames=new char*[105];
    for(int i=0;i<105;i++)
    {
        filenames[i]=new char[100];
    }






    if (d) 
    {
        int count=0;
        while (((dir = readdir(d)) != NULL) ) {
                 
            sprintf(filenames[count],".\\Weight160\\%s\0",dir->d_name);
            printf("%d %s\n",count,filenames[count]);

            count+=1;
            /*
            f=fopen(filename,"r");
            while(fscanf(f, "%x,", &data) == 1)
            {
                printf("%04x\n",data);
                
            }

            break;
            */

        } 

        closedir(d);
    }

    //print(count==0)
    int kernel_address=0;
    int kernel_count=0;
    int file_count=0;
    //void set_sep_conv2d(int input_size,int input_channel,int output_channel,int stride,uint32_t* kernel_address_list, char** filenames,int& kernel_address,int base_address, int& kernel_count, int& file_count)
    set_sep_conv2d(160,3,96,2,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_sep_conv2d(80,96,96,1,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_sep_conv2d(80,96,192,2,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_fire_module(40,192,16,64,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(40,128,16,64,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(40,128,32,128,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_sep_conv2d(40,256,128,2,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_fire_module(20,128,32,128,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(20,256,48,192,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(20,384,48,192,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_fire_module(20,384,64,256,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_sep_conv2d(20,512,256,2,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    set_fire_module(10,256,64,256,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    set_point1x1(10,512,1024,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);
    //avg-pooling doesnt need setting
    set_point1x1(1,1024,512,kernel_address_list,filenames,kernel_address,(uint32_t)base_address,kernel_count,file_count);

    printf("%x\n",kernel_address);
    


    int input_address=kernel_address+0x00; //assign 0~80*80*96=614400 *4byte to input 
    
    int output_address=input_address+160*160*3;  //assign another 0~80*80*96=614400 *4byte to input 

    int protect_address=output_address+160*160*3;
    kernel_count=0;
    //void execute(uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int kernel_count,void* send1,void* send2,void* send3,void* send4, void* receive)
    execute(kernel_address_list,(uint32_t)base_address,input_address,output_address,kernel_count,NULL,NULL,NULL,NULL,NULL);


    /*
    sep_conv2d(160,3,96,2,1,filenames,kernel_count,input_address,output_address,kernel_address,next_input_address,ddr3,0,0);
    sep_conv2d(80,96,96,1,1,filenames,kernel_count,input_address,output_address,kernel_address,next_input_address,ddr3,0,0);
    sep_conv2d(80,96,192,2,1,filenames,kernel_count,input_address,output_address,kernel_address,next_input_address,ddr3,0,0);
    fire_module(40,192,16,64,1,1,filenames,kernel_count,input_address,output_address,kernel_address,next_input_address,ddr3);
    //fire_module(40,128,16,64,1,1,filenames,kernel_count,input_address,output_address,kernel_address,next_input_address,ddr3);
    printf("%x,%x,%x,%x",input_address,output_address,kernel_address,next_input_address);
*/
    return 0;
}