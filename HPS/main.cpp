#include <opencv2/opencv.hpp>
#include <iostream>
#include <cassert>
#include <cmath>
#include <fstream>
//#include <windows.h>
#include <time.h>
#include <dirent.h> 

//-------------Some include when it is working on the Nano-10 -----------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"
//-----------------------------------------------------------------------



using namespace cv;






#define HW_REGS_BASE (ALT_STM_OFST)
#define HW_REGS_SPAN (0x04000000)
#define HW_REGS_MASK (HW_REGS_SPAN-1)




#define TYPE_CONV1x1 0
#define TYPE_CONV3x3D 1
#define TYPE_AVG_POOLING 2
#define TYPE_BATCH_NORM 3


#define USE_RELU 1
#define LINEAR 0
#define USE_BN 1


void padding(int& input_size,int input_channel,int stride,int& input_address,int& output_address,int base_address);

int set_layer(int input_size, int input_channel, int output_channel, int type,int bn,uint32_t* kernel_address_list,char* filename1,char* filename2, char* filename3,int& kernel_address,int base_address,int& kernel_count,int& file_count);


void set_depth3x3(int input_size,int input_channel,int stride,uint32_t* kernel_address_list, char** filenames,int& kernel_address,int base_address, int& kernel_count, int& file_count);


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

void fpga(int input_size, int input_channel, int output_channel,int type, int stride,int relu,int bn,int input_address,int output_address,int kernel_address,void* send1,void* send2,void* send3,void* send4, void* receive)
{
	printf("input size %d, input address: %x\n output address: %x kernel_address: %x\n",input_size, input_address*4,output_address*4,kernel_address*4);
	printf("stride %d input channel %d, output channel: %d  type %d \n",stride,input_channel,output_channel, type);


	*((uint32_t *)send1) = 1+(1<<1)+(0<<2)+((stride-1)<<3)+(bn<<4)+(relu<<5)+(type<<6)+(output_channel<<10)+(input_channel<<21);

	*((uint32_t *)send2) = (input_address)+(input_size<<24);
	*((uint32_t *)send3) = output_address;
	*((uint32_t *)send4) = kernel_address;




	*((uint32_t *)send1) =  1+(1<<1)+(1<<2)+((stride-1)<<3)+(bn<<4)+(relu<<5)+(type<<6)+(output_channel<<10)+(input_channel<<21);
	
	int wait=0;
	while(wait!=1)
	{
		wait=*((uint32_t *)receive);
	}
	printf("DONE\n");
	*((uint32_t *)send1) =  1+(1<<1)+(0<<2)+((stride-1)<<3)+(bn<<4)+(relu<<5)+(type<<6)+(output_channel<<10)+(input_channel<<21);
		//int n;    
	//std::cin>>n;
}

void fpga_depth3x3(int& input_size,int input_channel, int stride, uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int& kernel_count,void* send1,void* send2,void* send3,void* send4, void* receive)
{
    padding(input_size,input_channel,stride,addr_ping,addr_pong,base_address) ;   
    //printf("(%d,%d)\n",input_size,input_size);
	//int n;    
//std::cin>>n;
    fpga(input_size,input_channel,input_channel,TYPE_CONV3x3D,stride,0,0,addr_ping,addr_pong,kernel_address_list[kernel_count],send1,send2,send3,send4,receive);
    input_size=(input_size-(2-stride)-1)/stride;
    int tmp=addr_ping;
    addr_ping=addr_pong;
    addr_pong=tmp;
    //printf("execute layer d %d\n",kernel_count);
    kernel_count+=1;
}

void fpga_point1x1(int& input_size,int input_channel,int output_channel,int stride, int relu, uint32_t* kernel_address_list,int base_address,int& addr_ping,int& addr_pong,int& kernel_count,void* send1,void* send2,void* send3,void* send4, void* receive)
{
    fpga(input_size,input_channel,output_channel,TYPE_CONV1x1,stride,relu,1,addr_ping,addr_pong,kernel_address_list[kernel_count],send1,send2,send3,send4,receive);
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
	printf("input size %d, input address: %x\n output address: %x \n",input_size, addr_ping*4,addr_pong*4);
	printf("input channel %d, type %d \n",input_channel, 2);

	int input_size2=input_size*input_size;
	for (int k=0;k<input_channel;k++)
	{
		int temp=0;
		for (int i=0;i<input_size;i++)
		{
		for(int j=0;j<input_size;j++)
		{
			
			temp+=*((uint32_t*)(base_address+(addr_ping+k*input_size2+i*input_size+j)*4));
	}
	}
		temp/=input_size2;
		*((uint32_t*)(base_address+(addr_pong+k)*4))=temp;
		
	}
        //fpga(input_size,input_channel,input_channel,TYPE_AVG_POOLING,1,0,0,addr_ping,addr_pong,kernel_address_list[kernel_count],send1,send2,send3,send4,receive);
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

    

    fpga_sep_3x3(input_size,3,96,2,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);


    fpga_sep_3x3(input_size,96,96,1,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
 
	
    fpga_sep_3x3(input_size,96,192,2,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    
	
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
 

    fpga_average_pooling(input_size,1024,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
    
    fpga_point1x1(input_size,1024,512,1,0,kernel_address_list,base_address,addr_ping,addr_pong,kernel_count,send1,send2,send3,send4,receive);
return;

}


float cos_distance(float* a,float* b)
{
	float absa=0;
	float absb=0;
	float dotab=0;
	for(int i=0;i<512;i++)
	{
		absa+=a[i]*a[i];
		absb+=b[i]*b[i];
		dotab+=a[i]*b[i];
	}
	absa=sqrt(absa);
	absb=sqrt(absb);
	
	float ret=0;
	ret=dotab/(absa*absb);
	return ret;

}


int main(int argc, char **argv)
{
	
	

	for(int i=0;i<argc;i++)
	{
		printf("%s\n",argv[i]);
	}
	
	int fd;
	DIR *d;
	void *virtual_base;
	void *base_address;

	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );
	base_address = mmap( NULL, 0x20000000, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, 0x20000000 );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	//uint32_t *SDRAM = (uint32_t*) malloc(0x1000000*sizeof(uint32_t));

	//uint32_t *SDRAM2 = (uint32_t*) malloc(0x1000000*sizeof(uint32_t));
//
//	for(int i=0;i<0x20000000/4;i++){
//		*(uint32_t*)(ddr3_base+i*4)=i;
//	}


	
	

	printf("DDR3 Address = %x\n", (uint32_t)base_address);

	

	cv::Mat image;
	//cv::Mat image_valid(1,160*160*3, CV_8UC1);
	//cv::Mat image_valid_r(1, 160 * 160 * 3, CV_8UC1);

	void* send1 = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + DATA_TYPE_BASE ) & ( unsigned long)( HW_REGS_MASK ) );   //abstract
	void* send2 = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + INPUT_ADDR_BASE ) & ( unsigned long)( HW_REGS_MASK ) );  //abstract
	void* send4 = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + WEIGHT_ADDR_BASE ) & ( unsigned long)( HW_REGS_MASK ) );   //abstract
	void* send3 = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + OUTPUT_ADDR_BASE ) & ( unsigned long)( HW_REGS_MASK ) );  //abstract
	void* receive = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + FINISH_SIGNAL_BASE ) & ( unsigned long)( HW_REGS_MASK ) );  //abstract

	
	*((uint32_t *)send1) = 1+(1<<1);

	
	
	cv::Mat mu,sigma;
	std::vector<cv::Mat> channels;
	double m,sig;

	



	int pyshic_addr=0x20000000;
	int expand_addr=0;
	int memoffset=0;


	

	char** filenames;
	filenames=new char*[105];
	uint32_t* kernel_address_list;
	kernel_address_list= new uint32_t[100];
	
	

	for(int i=0;i<106;i++)
	{
	    filenames[i]=new char[100];
	}
	
for(int i=0;i<106;i++)
{
	sprintf(filenames[i],"./weight160/count_%03d.hex\0",i);
	//printf("%d %s\n",i,filenames[i]);
}	

    int kernel_address=0x00;
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
    

    printf("%x\n",kernel_address*4);

    int input_address=kernel_address+0x00; //assign 0~80*80*96=614400 *4byte to input 
    printf("%x\n",input_address*4);
    int output_address=input_address+82*82*96;  //assign another 0~80*80*96=614400 *4byte to input 
    printf("%x\n",output_address*4);
    int protect_address=output_address+82*82*96;
    int addr_ping=input_address;
    int addr_pong=output_address;
	struct dirent *dir;
	d=opendir("./data/image/");
	std::vector<char*> file_name_list;
	while((dir=readdir(d))!=NULL)
	{
		if(strcmp(dir->d_name,".") && strcmp(dir->d_name,".."))
		{
			char* feature_file_name=new char[20];
			printf("%s\n",dir->d_name);
			feature_file_name=dir->d_name;
			for(int i=0;i<strlen(feature_file_name);i++)
			{
				if(feature_file_name[i]=='.')
				{
					feature_file_name[i]='\0';
				}
			}
			file_name_list.push_back(feature_file_name);
			
		}
		
		

	}
	

	for(int i=0;i<file_name_list.size();i++)
	{
		printf("%s\n",file_name_list[i]);
	}
	
	if(argc>1)
	{
		if(!strcmp(argv[1],"1"))
		{
		
		for(int n=0;n<file_name_list.size();n++)
		{
			char* imgname=new char[30];
			sprintf(imgname,"./data/image/%s.jpg",file_name_list[n]);
			printf("img %s\n",imgname);
			image=cv::imread(imgname);
			printf("aaasss");
			image.convertTo(image , CV_32F);	
			cv::meanStdDev(image, mu, sigma);	
			cv::split(image, channels);
			printf("asd\n");
			m=0;
			sig=0;
			for(int i = 0 ; i < channels.size() ; i++)
			{
				m = m+mu.at<double>(i,0);
				sig = sig + sigma.at<double>(i,0);
			}
		
			m /= 3.0;
			sig /= 3.0;
			printf("m=%f, sig=%f\n", m , sig);
		
			for(int i = 0 ; i < channels.size() ; i++)
			{
				channels[i] = (channels[i] - m) / sig;
			}
		
			cv::merge(channels, image);
			
			
			for (int k=0;k<3;k++)
			{
			for(int i=0;i<160;i++)
			{
			for(int j=0;j<160;j++)
			{
			*((uint32_t*)(base_address+(input_address+k*160*160+i*160+j)*4))=(int)(image.at<cv::Vec3f>(i,j)[k]*256.0*256.0); 
			}
			}
			}
			kernel_count=0;
			
			execute(kernel_address_list,(int)base_address, addr_ping,addr_pong,kernel_count,send1,send2,send3,send4, receive);
	
			FILE* f123;    
			char* featurename=new char[30];
			sprintf(featurename,"./data/features/%s",file_name_list[n]);
   			f123=fopen(featurename,"w");
		    int temp_data;
		    for(int i=0;i<512;i++)
		    {
		        temp_data=*((uint32_t*)(base_address+(addr_ping+i)*4));
			float fff=(float)temp_data;
			fff/=pow(2,16);
			printf("%d %f\n",temp_data,fff);
		        fprintf(f123,"%f\n",fff);        
		        
		        
		            
		    }
    		fclose(f123);
		
		}
		return 0;
		}
	}

	float** feature;
	feature=new float*[file_name_list.size()];
	for(int i=0;i<file_name_list.size();i++)
	{
		feature[i]=new float[512];
		char* feature_file_name=new char[30];
		sprintf(feature_file_name,"./data/features/%s",file_name_list[i]);
		printf("read %s\n",feature_file_name);
		FILE* readf;
		readf=fopen(feature_file_name,"r");
		//fscanf(readf,"%s",name[i]);
		for(int j=0;j<512;j++)
		{
			fscanf(readf,"%f",&feature[i][j]);
			//printf("%d %d %f\n",i,j,feature[i][j]);
		}
		fclose(readf);
	}
	
		


	kernel_count=0;


	cv::Mat frame0;
	cv::Mat frame0_ori;
	cv::Mat frame1;
	cv::Mat frame0_gray;
	cv::Mat frame0_sub_gray;
	cv::Mat frame1_gray;
	cv::VideoCapture cap0(0);
	cv::VideoCapture cap1(1);
	std::vector<cv::Point2f> curr_pts, curr_pts2;
	std::vector<uchar> status;
	std::vector<float> err;
	//cv::imshow("img2",image);
	cap0>>frame0;	
	cap1>>frame1;
	cv::resize(frame0,frame0,cv::Size(),1.0/3.0,1.0/3.0);
	cv::resize(frame1,frame1,cv::Size(),1.0/3.0,1.0/3.0);

	cv::Mat alert(160,160,CV_8UC3,cv::Scalar(255,255,255));
	cv::cvtColor(frame0,frame0_gray,cv::COLOR_BGR2GRAY);
	frame0_sub_gray=frame0_gray(cv::Rect(26,0,160,160));
	cv::waitKey(0);

	
	int face_count=0;
	while(1)
	{
		int face_flag=0;
		cap0>>frame0_ori;
		
		cap1>>frame1;
		cv::resize(frame0_ori,frame0_ori,cv::Size(),1.0/3.0,1.0/3.0);
		cv::resize(frame1,frame1,cv::Size(),1.0/3.0,1.0/3.0);
		
		frame0_ori=frame0_ori(cv::Rect(26,0,160,160));
		frame1=frame1(cv::Rect(53,0,160,160));
		frame0_ori.copyTo(frame0);
		cv::cvtColor(frame0,frame0_gray,cv::COLOR_BGR2GRAY);
		//frame0_sub_gray=frame0_gray(cv::Rect(26,0,160,160));
		//cv::goodFeaturesToTrack(frame0_gray,curr_pts,200,0.001,1);
		curr_pts.clear();		
		for(int i=0;i<10;i++){
			for(int j=0;j<10;j++)
			{
				Point2f k;
				k.x=30+10*j;
				k.y=30+10*i;
				curr_pts.push_back(k);
			}
		}
		/*
		for(int i=0;i<curr_pts.size();i++)
		{
			curr_pts[i].x+=26;
		}
		*/
		cv::cvtColor(frame1,frame1_gray,cv::COLOR_BGR2GRAY);
		
		cv::calcOpticalFlowPyrLK(frame0_gray,frame1_gray,curr_pts,curr_pts2,status,err, cv::Size(7,7),3,cv::TermCriteria(cv::TermCriteria::COUNT+cv::TermCriteria::EPS,3,1.0));
		auto pt1=curr_pts.begin();
		auto pt2=curr_pts2.begin();
		for(size_t k=0;k<status.size();k++)
		{
			if(status[k])
			{
				pt1++;
				pt2++;
			}
			else{
				pt1=curr_pts.erase(pt1);
				pt2=curr_pts2.erase(pt2);
			}
		}
		float err_total=0;
		
		for(int i=0;i<curr_pts2.size();i++)
		{
			cv::circle(frame0,cv::Point(curr_pts[i].x,curr_pts[i].y),2,cv::Scalar(0,0,255),-1);
			cv::circle(frame1,cv::Point(curr_pts2[i].x,curr_pts2[i].y),2,cv::Scalar(0,0,255),-1);
			err_total+=err[i];
		}
		err_total/=curr_pts.size();
		Mat i1,i2,i3;
		cv::resize(frame0,i1,cv::Size(),2.0,2.0);
		cv::resize(frame1,i2,cv::Size(),2.0,2.0);
		cv::resize(alert,i3,cv::Size(),2.0,2.0);
		cv::imshow("img0",i1);
		cv::imshow("img1",i2);
		cv::imshow("alert",i3);
		int ch=cv::waitKey(1)&0xFF;
		
		
		if(curr_pts2.size()>90 && err_total<28.0)
		{
			if(face_count<5)
			{
				face_count++;
			}
			else
			{
				
				face_flag=1;
			}
		}
		else
		{
			if(face_count>0)
			{
				face_count--;
			}
			else
			{
				*((uint32_t *)send1) = 1+(1<<1);
			}			
		}
		printf("\n%d %f %d\n\n",curr_pts2.size(),err_total,face_count);
		if(face_flag)
		{
			//cv::circle(alert,cv::Point(40,40),30,cv::Scalar(0,0,255),-1);

	frame0_ori.convertTo(image , CV_32F);
	
	//cv::Mat mu,sigma;
	cv::meanStdDev(image, mu, sigma);

	//std::vector<cv::Mat> channels;
	cv::split(image, channels);
	
	m=0;
	sig=0;
	for(int i = 0 ; i < channels.size() ; i++)
	{
		m = m+mu.at<double>(i,0);
		sig = sig + sigma.at<double>(i,0);
	}

	m /= 3.0;
	sig /= 3.0;
	printf("m=%f, sig=%f\n", m , sig);

	for(int i = 0 ; i < channels.size() ; i++)
	{
		channels[i] = (channels[i] - m) / sig;
	}

	cv::merge(channels, image);


			for (int k=0;k<3;k++)
{
	for(int i=0;i<160;i++)
	{
		for(int j=0;j<160;j++)
		{
		*((uint32_t*)(base_address+(input_address+k*160*160+i*160+j)*4))=(int)(image.at<cv::Vec3f>(i,j)[k]*256.0*256.0); 
		}
	}
}

		//continue;
		execute(kernel_address_list,(int)base_address, addr_ping,addr_pong,kernel_count,send1,send2,send3,send4, receive);
		float* exe_feature=new float[512];
		int temp_data2;
		    for(int i=0;i<512;i++)
		    {
		        temp_data2=*((uint32_t*)(base_address+(addr_ping+i)*4));
			float fff=(float)temp_data2;
			fff/=pow(2,16);
			//printf("%d %f\n",temp_data,fff);
		        exe_feature[i]=fff;
			     
		        
		        
		            
			}
			
		float max_sim=0.6;
		int maxname=-1;
		
		for(int i=0;i<file_name_list.size();i++)
		{
			float result=cos_distance(exe_feature,feature[i]);
			printf("%s %f",file_name_list[i],result);
			if( max_sim<result)
			{
				max_sim=result;
				maxname=i;
			}
		}
		
		if (maxname==-1)
		{
			alert=cv::Mat(160,160,CV_8UC3,cv::Scalar(255,255,255));
			cv::putText(alert,"UNKNOWN",cv::Point(20,40),cv::FONT_HERSHEY_PLAIN,2.0,cv::Scalar(0,0,0),1);
		}
		else
		{
			*((uint32_t *)send1) = 0+(0<<1);
			alert=cv::Mat(160,160,CV_8UC3,cv::Scalar(255,255,255));
			cv::putText(alert,file_name_list[maxname],cv::Point(20,40),cv::FONT_HERSHEY_PLAIN,2.0,cv::Scalar(0,0,0),1);
		}
		
					
		face_count=0;

		}
		else
		{
			//cv::circle(alert,cv::Point(40,40),30,cv::Scalar(0,0,0),-1);
		}

		if (ch==(int)'q')
		{
			break;
		}
		if(ch==(int)'w')
		{
			
			
			cv::imwrite("./data/temp.jpg",frame0_ori);
		}
	}
	return 0;

}



void padding(int& input_size,int input_channel,int stride,int& input_address,int& output_address,int base_address)
{
    int input_size2=input_size*input_size;
    int paddinput_size=(input_size+3-stride);
    int paddinput_size2=paddinput_size*paddinput_size;
    //output_address=input_address+input_size2*input_channel;
	//printf("addr %x %d\n",base_address,stride);
	//printf("padding %x to %x", input_address*4,output_address*4);
    if (stride==1)
    {
        for(int k=0;k<input_channel;k++)
        {
            for (int j=0;j<input_size+2;j++)
            {
                    *((uint32_t*)(base_address+(output_address+k*paddinput_size2+(0)*paddinput_size+(j))*4))=0;
            }


            for (int i=0;i<input_size;i++)
            {
                *((uint32_t*)(base_address+(output_address+k*paddinput_size2+(i+1)*paddinput_size+(0))*4))=0;
                for (int j=0;j<input_size;j++)
                {
                    *((uint32_t*)(base_address+(output_address+k*paddinput_size2+(i+1)*paddinput_size+(j+1))*4))=*((uint32_t*)(base_address+(input_address+k*input_size2+i*input_size+j)*4));
                }

                *((uint32_t*)(base_address+(output_address+k*paddinput_size2+(i+1)*paddinput_size+(input_size+1))*4))=0;
            }

            for (int j=0;j<input_size+2;j++)
            {
                    *((uint32_t*)(base_address+(output_address+k*paddinput_size2+(input_size+1)*paddinput_size+(j))*4))=0;
            }

        }
    }
    else if(stride==2)
    {
        
    
        for(int k=0;k<input_channel;k++)
        {
            for (int i=0;i<input_size;i++)
            {
                for (int j=0;j<input_size;j++)
                {
                    *((uint32_t*)(base_address+(output_address+k*paddinput_size2+(i)*paddinput_size+(j))*4))=*((uint32_t*)(base_address+(input_address+k*input_size2+i*input_size+j)*4));
                }

                *((uint32_t*)(base_address+(output_address+k*paddinput_size2+(i)*paddinput_size+(input_size))*4))=0;
            }

            for (int j=0;j<input_size+1;j++)
            {
                *((uint32_t*)(base_address+(output_address+k*paddinput_size2+(input_size)*paddinput_size+(j))*4))=0;
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
	//printf("%d %d\n",kernel_size,bn_size);
    //printf("%d save %s\n %s\n %s\n into %x\n",kernel_count,filename1,filename2,filename3, kernel_address);
    

    FILE* f;    
    f=fopen(filename1,"r");
    int data;
    for(int i=0;i<kernel_size;i++)
    {
        
        
        if(fscanf(f, "%x,", &data) == 1)
        {
            *((uint32_t*)(base_address+(kernel_address+i)*4))=data; 
            //printf("write %04x into %x\n",data,0x20000000+(kernel_address+i)*4);
            
        }
            
    }
    fclose(f);

    if (bn==1)
    {
        FILE* f2;    
        f2=fopen(filename2,"r");
        int data2=0;
	//printf("put bn gamma %x %s\n",(kernel_address+kernel_size)*4,filename2);
        for(int i=0;i<bn_size;i++)
        {
            
            
            if(fscanf(f2, "%x,", &data2) == 1)
            {
		
                *((uint32_t*)(base_address+(kernel_address+kernel_size+i)*4))=data2; 
		
                //printf("%04x\n",data);                
            }
		
                
        }
        fclose(f2);

        FILE* f3;    
        f3=fopen(filename3,"r");
        int data3;
	
        for(int i=0;i<bn_size;i++)
        {
            
            
            if(fscanf(f3, "%x,", &data3) == 1)
            {
                *((uint32_t*)(base_address+(kernel_address+kernel_size+bn_size+i)*4))=data3; 
                //printf("put bn beta\n");
		//printf("%04x\n",data);                
            }
                
        }
        fclose(f3);
    }
    kernel_address_list[kernel_count]=kernel_address;
    kernel_address+=(kernel_size+bn_size*2);
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

void set_depth3x3(int input_size,int input_channel,int stride,uint32_t* kernel_address_list, char** filenames,int& kernel_address,int base_address, int& kernel_count, int& file_count)
{
    set_layer(input_size,input_channel,input_channel,TYPE_CONV3x3D,0,kernel_address_list,filenames[file_count],NULL,NULL,kernel_address,base_address,kernel_count,file_count);

}



