#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int return_main(char ***return_point);
int main()
{
	unsigned int i = 0;
	int ret = 0;
	
	char **p_return = NULL;

	
	ret = return_main(&p_return);  
		
	if(ret >= 0)
	{
		printf("get_data success\n");

	}
	else
	{
		printf("failure!\n");
	    return -1;
	}
	printf("%p\n", p_return);

	
	if(p_return != NULL)
	{
		for(i=0; i<ret; i++)
			printf("p_return[%d] = %s\n", i, p_return[i]);
	}

	
	if(p_return != NULL)
	{
		for(i=0; i<ret; i++)
		{
			if(p_return[i] != NULL)
			{
				free(p_return[i]);
				p_return[i] = NULL;
			}
		}

		free(p_return);
		p_return = NULL;
	}

	return 0;

}
	
