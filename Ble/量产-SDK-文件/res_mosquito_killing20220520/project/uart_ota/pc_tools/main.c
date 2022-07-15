#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char  uint8_t;
typedef unsigned int  uint32_t;
// Image header for SUOTA
typedef struct
{
    uint8_t signature[2];
    uint8_t validflag;      // Set to STATUS_VALID_IMAGE at the end of the image update
    uint8_t imageid;        // it is used to determine which image is the newest
    uint32_t code_size;     // Image size
    uint32_t CRC ;          // Image CRC
    uint8_t version[16];    // Vertion of the image
    uint32_t timestamp;
    uint8_t encryption;
    uint8_t reserved[31];
}image_header_t;

static unsigned char buffer[1024];
int main(int argc, char * argv[])
{
	FILE * fp_in;
	FILE * fp_out;
	uint32_t  code_size;
	uint32_t  size; 
	uint32_t  i;
	uint32_t  checksum = 0;
	printf("################# OTA TOOL #######################################\n");
	printf("           Version:   1.0 \n");
	printf("           Date:      %s  \n", __TIME__); 
	printf("           Copyright (C) Radiawave 2020-2025\n" ); 
	printf("           description: convert the app bin file to OTA format \n");
	printf("#################################################################\n");
	printf("\n\n");
	if(argc < 3)
	{
		 printf("please use command: \n%s  input_bin_file  output_file \n", argv[0]);
		 return 0;
	}
	fp_in = fopen(argv[1], "rb");
	if(fp_in == NULL)
	{
		printf("Can not open the file %s \n", argv[1]);
		return 0;
	}
	
	fp_out = fopen(argv[2], "wb");
	if(fp_out == NULL)
	{
		printf("Error, can not create file %s \n", argv[2]);
		fclose(fp_in);
		return 0;
	}
	
	
	image_header_t header;
	memset(&header, 0xFF, sizeof(header));
	header.signature[0] = 0x70;
	header.signature[1] = 0x51;
	
	header.validflag = 0xAA;
	header.imageid = 0x00;
	
	fseek(fp_in, 0, SEEK_END);
	code_size = ftell(fp_in);
	
	fseek(fp_in, 0, SEEK_SET);
	header.code_size = code_size;
	header.CRC = 0x0000;
	strcpy(header.version, "Radiawave \n");
	header.timestamp = 0x00;
	header.encryption = 0x00;
	
	fwrite(&header, 1, sizeof(header), fp_out);
	
	while(!feof(fp_in))
	{
		size = fread(buffer, 1, 1024, fp_in);
		for(i = 0; i < size; i++)
		{
			checksum += buffer[i];
		}
		fwrite(buffer, 1, size, fp_out);
	}
	
	header.CRC = checksum;
	fseek(fp_out, 0, SEEK_SET);
	fwrite(&header, 1, sizeof(header), fp_out);
	fclose(fp_in);
	fclose(fp_out);
	printf("success checksum = 0x%08x .......\n", checksum );
	
	
    return 1;
}