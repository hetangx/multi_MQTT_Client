#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char getFigure(char* topicName, char* payload)
{
    char p[10] = { '0','1','2','3','4','5','6','7','8','9' };
    char* pfig = NULL;
    for (int i = 0; i < 10; i++)
    {
        if ((pfig = strrchr(topicName, p[i])) != NULL)
        {
            break;
        }
    }

    char a = 0;
        a = *pfig;
         printf("a: %c\n", a);

    return a;
}

void catPubtopic(char** p, char* maintopic)
{   
    int lenp = strlen(maintopic) + 5;
    char* p1 = strstr(maintopic, "/up");
    if (p1)
    {
        strncpy_s(*p, lenp , maintopic, strlen(maintopic) - strlen(p1));
        strcat_s(*p, lenp, "/retrun");
    }

}

int main()
{
    char* test = "plat/gateway/460110142949436/pkg/4/up";
    char* payload = "asdfqwer";
    char fig = getFigure(test, payload);
    printf("%c\n", fig);

    char* pubtopic = (char*)malloc(strlen(test) + 5);
    if (pubtopic)
    {
        catPubtopic(&pubtopic, test);
        printf("pub topic: %s\n", pubtopic);
    }
    free (pubtopic);
    return 0;
}