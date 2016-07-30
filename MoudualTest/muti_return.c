#include "stdio.h"
#include "string.h"
#include "stdlib.h"

char **global_point = NULL;


static int callback(void *NotUsed, int argc, char **argv, char **az)
{
	
	unsigned int len = 0;
	int i,j = 0;

	char **p_first = NULL;


	if(argv == NULL || argc == 0 )
	{
		return -1;
	}


	/*分配内存*/
	p_first = (char**)malloc(sizeof(char*)*argc);
	if(p_first != NULL)
	{
		for(j=0; j<argc; j++)
		{
                   p_first[j] = (char*)malloc(sizeof(char)*strlen(argv[j])+1);
		   memset(p_first[j], 0, sizeof(char)*strlen(argv[j])+1);
		}	
	}

   //将argv中的数据缓存到分配好的内存中.

	for(i=0; i<argc; i++)
	{
		strncpy(p_first[i], argv[i],strlen(argv[i]));
	}

	//将而极指针传给全局而极指针，供主程序调用.
	global_point = p_first;
	
	return 0;

}	

extern int return_main(char ***return_point)

{
	int ret = 0;
        char *argv[3] = {"abchkihsaikhkkfsakjnhjfsa", "456", "789233344"};
        int argc=3;
	ret = callback(NULL, argc, argv, NULL);
	
	if(ret == 0)
	{
		*return_point = global_point;
              
	}else
        {
                return -1;
         }	
	global_point = NULL;
	return argc;
}
	
