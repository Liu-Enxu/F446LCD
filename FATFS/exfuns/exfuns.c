#include "string.h"
#include "exfuns.h"
// #include "usart.h"
#include "FreeRTOS.h"

#define FILE_MAX_TYPE_NUM		6	// Number of file type groups
#define FILE_MAX_SUBT_NUM		13	// Maximum number of subtypes per group

// File type list
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			// BIN file
{"LRC"},			// LRC file
{"NES"},			// NES file
{"TXT","C","H"},	// Text files
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","AAC","WMA","WAV","MID","FLAC"},// Audio files
{"BMP","JPG","JPEG","GIF"},// Image files
};
/////////////////////////////// File system variables, used when malloc is enabled ///////////////////////////////
FATFS *fs[_VOLUMES];// Logical drive work areas.
FIL *file;  	  		// File object 1
FIL *ftemp;  	  		// File object 2
UINT br,bw;			// Bytes read/written
FILINFO fileinfo;	// File information
DIR dir;  			// Directory

u8 *fatbuf;			// SD card data buffer
///////////////////////////////////////////////////////////////////////////////////////
// Allocate memory for exfuns
// Return value: 0, success
// 1, failure
u8 exfuns_init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++){
		fs[i]=(FATFS*)pvPortMalloc(sizeof(FATFS));	// Allocate memory for logical drive i
		if(!fs[i])break;
	}
	file=(FIL*)pvPortMalloc(sizeof(FIL));		// Allocate memory for file
	ftemp=(FIL*)pvPortMalloc(sizeof(FIL));		// Allocate memory for ftemp
	fatbuf=(u8*)pvPortMalloc(512);				// Allocate memory for fatbuf
	if((i==_VOLUMES)&&file&&ftemp&&fatbuf)return 0;  // If any allocation fails, return failure.
	else return 1;
}

// Convert lowercase letters to uppercase; leave other characters unchanged.
u8 char_upper(u8 c)
{
	if(c<'A')return c;// Non-letter, unchanged.
	if(c>='a')return c-0x20;// Convert to uppercase.
	else return c;// Uppercase letter, unchanged.
}
// Determine file type from file name
// fname: file name
// Return value: 0XFF means the file type cannot be recognized.
//         Otherwise, the high nibble is the main type and the low nibble is the subtype.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';// File extension
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;// Offset to the end.
		fname++;
	}
	if(i==250)return 0XFF;// Bad string length.
	for(i=0;i<5;i++)// Get extension
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
	}
	strcpy((char *)tbuf,(const char*)attr);//copy
	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);// Convert to uppercase
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	// Compare main type
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)// Compare subtype
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;// No more members to compare.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)// Found
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;// Not found
}

// Get total and free disk capacity
// drv: drive number ("0:"/"1:")
// total: total capacity, in KB
// free: free capacity, in KB
// Return value: 0 on success; otherwise, error code
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
	u32 fre_clust=0, fre_sect=0, tot_sect=0;
	// Get free space information and free cluster count.
	res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
	if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	// Get total sectors
	    fre_sect=fre_clust*fs1->csize;			// Get free sectors
#if _MAX_SS!=512				  				// If sector size is not 512 bytes, convert to 512-byte sectors
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
			*total=tot_sect>>1;	// Unit: KB
			*free=fre_sect>>1;	// Unit: KB
 	}
	return res;
}	









