/*
*用KMP算法实现字符串匹配搜索方法
*该程序实现的功能是搜索本目录下的所有文件的内容是否与给定的
*字符串匹配，如果匹配，则输出文件名：包含该字符串的行
*待搜索的目标串搜索指针移动位数 = 已匹配的字符数 - 对应部分匹配值
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define KEYWORD_MAX_LENGTH 100      //设定搜索串的最大长度

int kmp_table[KEYWORD_MAX_LENGTH];  //为搜索串建立kmp表
char prefix_stack[KEYWORD_MAX_LENGTH]; //前缀表达式栈
char suffix_stack[KEYWORD_MAX_LENGTH]; //后缀表达式栈
int keyword_length = 0;  //搜索串的长度
int record_position[KEYWORD_MAX_LENGTH]; //记录与关键字串匹配源串中的位置

/*
*GetMatchValue:获得字符串src的部分匹配值
*/
int GetMatchValue(char *src)
{
    int value = 0;
    int src_len = strlen(src);
    char *begin = src;    //初始化指向字符串第一个字符
    char *end = src + (src_len - 1);  //初始化指向字符串最后一个字符
    int i = 0;
    for(i=0;i<(src_len-1);i++)
    {
        prefix_stack[i] = *begin;
        suffix_stack[i] = *end;
        begin++;
        end--;
    }
    char *p = prefix_stack;
    char *q = suffix_stack + (src_len - 2);  //指向栈中最后一个元素
    int flag = 0;   //用一个标志位来确定后缀栈中到最后一个元素都与前缀栈中的符号匹配
    while(q >= suffix_stack)
    {
        if(*p == *q)
        {
            value++;
            p++;
            flag=1;
        }
        else {
            flag = 0;
        }
        q--;
    }
    if(flag == 0) value = 0;
    return value;
}

/*
*创建搜索字符串的KMP表
*/
int Create_KMP_Table(char *str,int *table)
{
    int i;
    char *dst;
    keyword_length = strlen(str);
    for(i=0;i<keyword_length;i++)
    {
        if(i == 0) {
            table[i] = 0;   //第一个字符无前缀和后缀，所以为0
        }
        else {
            dst = (char*)malloc((i+2));
            if(dst == NULL)
            {
                printf("malloc space error!\n");
                return EXIT_FAILURE;
            }
            strncpy(dst,str,(i+1));   //匹配str的前(i+1)个字符
            dst[i+1] = '\0';    //注意字符串要以'/0'结尾
            table[i] = GetMatchValue(dst); 
            free((void*)dst);    
        }
    }
    return EXIT_SUCCESS;
}

//打印搜索字符串对应的KMP表
void Table_Print(char *str,int *table)
{
    int i;
    char c = *str;
    while(c != '\0')
    {
        printf("%-4c",c);        //左对齐输出搜索字符串中的字符
        c = *++str;
    }
    printf("\n");
    for(i=0;i<keyword_length;i++)
    {
        printf("%-4d",table[i]); //左对齐输出每个字符对应的部分匹配值
    }
    printf("\n");
}

//在目标串dst_str中搜索关键子串search_str,打印出关键字串的位置信息,返回与关键字串匹配的数目
int Search_Keyword(char *dst_str,char *search_str)
{
    char *p = dst_str;
    char *q = search_str;
    char *temp;

    //创建关键字串的KMP表    
    Create_KMP_Table(search_str,kmp_table);
    
    int count = 0;  //记录现在已经匹配的数目
    int k = 0;     //记录与关键字串匹配的字串的数目
    int move = 0;  //当字符串不匹配时，搜索指针移动的位数    

    while(*p != '\0')   //直到搜索到目标串的最后一个字符为止
    {
        temp = p;
        while(*q != '\0')
        {
            if(*q == *temp)
            {
                count++;
                temp++;
                q++;
            }
            else break;
        }
        
        if(count == 0)
            p++;
        else {
            if(count == keyword_length)
            {
                record_position[k++] = (temp-dst_str)-(keyword_length);
            }
            move = count - kmp_table[count-1];
            p += move;
        }

        count = 0;
        q = search_str;
    }
    return k;
}


int main(int argc,char **argv)
{
    char *search_str = argv[1];
    //char dst_str[] = "hello woshijpf woshijpf woshij woshijp woshijpf";
    char dst_str[] = "ADD--UNIQUE constraint failed: ProtectGroupTable.PNo";
    
    printf("Please input serach string and dst_string\n");
    if(search_str == NULL)
    {
        printf("Please input search string\n");
        return EXIT_FAILURE;
    }

    if(dst_str == NULL)
    {
        printf("Please input dst_string\n");
        return EXIT_FAILURE;
    }
    
    int result = Search_Keyword(dst_str,search_str);  //放回搜索到的结果的数目
    Table_Print(search_str,kmp_table);
    printf("%s\n",dst_str);         //输出待搜索的目标串
    if(result == 0)
    {
        printf("Sorry!Don't find the string %s\n",search_str);
        return EXIT_SUCCESS;
    }
    else {
        int i,j,num;
        int before = 0;
        for(i=0;i<result;i++)
        {
            num = record_position[i] - before;    //打印搜索串在目标串中的位置
            before = record_position[i]+1;
            for(j=1;j<=num;j++)
                printf(" ");
            printf("*");
        }
        printf("\n");
    }
    
    return EXIT_SUCCESS;
}
