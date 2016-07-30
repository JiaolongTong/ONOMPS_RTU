#include "stdio.h"
#include "string.h"
#include "stdlib.h"

char **global_point = NULL;


static int callback(void *NotUsed, int argc, char **argv, char **az)
{
	
	unsigned int length,len = 0;
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
                        p_first[j] = (char*)malloc(sizeof(char)*strlen(argv[j]));
                        memset(p_first[j], 0, strlen(argv[j])+1);          
		}	 
	}

        //将argv中的数据缓存到分配好的内存中.

	for(i=0; i<argc; i++)
	{
		strcpy(p_first[i], argv[i]);
	}

	//将而极指针传给全局而极指针，供主程序调用.
	global_point = p_first;

	return 0;

}	


int main()
{
	unsigned int i = 0;

	char *argv[3] = {"123545345453643656563454353jfdjfljfdsijgoidghjdsigjo;gjdioahgjlgj", "456", "789233344"};

	
	callback(NULL, 3, argv, 0);

	//主程序调用global_point, 使用数据.
	if(global_point != NULL)
	{
		for(i=0; i<3; i++)

		printf("global_point[i] = %s\n", global_point[i]);
	}

	//释放内存.
	for(i=0; i<3; i++)
	{
		free(global_point[i]);
	}


	free(global_point);

        *global_point = NULL;

	return 0;

}
	

