//Berk Sunduri-180201145m,,, Emre Eren Kaya-210201006
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <math.h>
#include <iostream>
#include <sstream>
#define MAX_CHAR 256
#define MIN(x, y) (((x) < (y)) ? (x) : (y))//stackoverflow yardimiyla yapildi
//iki degeri karsilastiriyor hangisi kucukse onu returnluyor
struct SuffixNode//struct data for nodes
{
    struct SuffixNode *child[MAX_CHAR];

    struct SuffixNode *sufLink;

    int start;
    int *sufEnd;

    int indexSuffix;
};

typedef struct SuffixNode Node;

char text[256];
Node *root=NULL;

Node *lastNode= NULL;
Node *activeNode=NULL;
int f=0;

int activeKenar = -1;
int activeLen = 0;

int remainingSuffix = 0;
int leafEnd = -1;
int size=-1;
int *rootEnd = NULL;
int *splitEnd= NULL;
int inputSize = -1;

//calismayan graph fonksiyonu istenirse test edilebilir
/*using namespace std;

int printSuffix(int x,int y,int* array, int index, int total)
{
        if(index>=total)
            return NULL;


        ostringstream str1;
        str1<<array[index];
        string str2=str1.str();
        char* str=&str2[0u];

        setcolor(WHITE);

        circle(x,y,20);
        floodfill(x,y,WHITE);;

        outtextxy(x-2,y-3,str);

        setcolor(BLUE);

        int left=2*index+1;
        int right=2*index+2;

        printSuffix(x-y/(index+1),y+50,array,left,total);
        printSuffix(x+y/(index+1),y+50,array,right,total);

        if(left<total)
        {


            line(x,y,x-y/(index+1),y+50);

        }
        if(right<total)
        {

            line(x,y,x+y/(index+1),y+50);

        }
        return NULL;
}*/

//yeni node olusturmak
Node *newNode(int start,int *sufEnd)
{
    f++;
    Node *node=(Node*)malloc(sizeof(Node));
    for(int i=0;i<MAX_CHAR;i++)
    {
        node->child[i]=NULL;
    }

    node->sufLink=root;
    node->start=start;
    node->sufEnd=sufEnd;

    node->indexSuffix=-1;
    return node;
}
//node icin uzunluk degerini dondur
int edgeOfLength(Node *n)//stackoverflow yardimiyla yapildi
{
     if(n == root)
     {
        return 0;
     }
    return *(n->sufEnd)-(n->start)+1;
}
//aktif node u karsilastir daha kisaysa degistir
int down(Node *currNode)
{
    if (activeLen >= edgeOfLength(currNode))
    {
        activeKenar =
         (int)text[activeKenar+edgeOfLength(currNode)]-(int)' ';
        activeLen -= edgeOfLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}
//suffix malloc ve ukkonen kurallari
void extendSuffix(int pos)
{
    leafEnd = pos;
    remainingSuffix++;
    lastNode = NULL;

    while(remainingSuffix> 0) {

        if (activeLen == 0) {

            activeKenar = (int)text[pos]-(int)' ';
        }
        if (activeNode->child[activeKenar] == NULL)
        {
            activeNode->child[activeKenar] =
                                  newNode(pos, &leafEnd);


            if (lastNode != NULL)
            {
                lastNode->sufLink = activeNode;
                lastNode = NULL;
            }
        }

        else
        {

            Node *next = activeNode->child[activeKenar];
            if (down(next))//karsilastir degistir
            {

                continue;
            }

            if (text[next->start + activeLen] == text[pos])
            {
                if(lastNode != NULL && activeNode != root)
                {
                    lastNode->sufLink = activeNode;
                    lastNode = NULL;
                }


                activeLen++;

                break;
            }


            splitEnd = (int*) malloc(sizeof(int));
            *splitEnd = next->start + activeLen - 1;


            Node *split = newNode(next->start, splitEnd);
            activeNode->child[activeKenar] = split;


            split->child[(int)text[pos]-(int)' '] =
                                      newNode(pos, &leafEnd);
            next->start += activeLen;
            split->child[activeKenar] = next;


            if (lastNode != NULL)
            {

                lastNode->sufLink = split;
            }

            lastNode = split;
        }


        remainingSuffix--;
        if (activeNode == root && activeLen > 0)
        {
            activeLen--;
            activeKenar = (int)text[pos -
                            remainingSuffix + 1]-(int)' ';
        }


        else if (activeNode != root)
        {
            activeNode = activeNode->sufLink;
        }
    }
}


//sufTree printle
void printSufTree(int i,int j)
{
    for(int k=i;k<=j;k++)
    {
        printf("%c",text[k]);
    }
}
//DepthFirst Search manner
void setSuffixDFS(Node *n,int labelLen)
{
    if (n == NULL) return;

    if (n->start != -1)
    {
        printSufTree(n->start, *(n->sufEnd));
    }
    int leaf = 1;
    int i;
    for (i = 0; i < MAX_CHAR; i++)
    {
        if (n->child[i] != NULL)
        {
            if (leaf == 1 && n->start != -1)
                printf(" [%d]\n", n->indexSuffix);

            leaf = 0;
            setSuffixDFS(n->child[i],
                  labelLen + edgeOfLength(n->child[i]));
        }
    }
    if (leaf == 1)
    {
        n->indexSuffix = size - labelLen;
        printf(" [%d]\n", n->indexSuffix);
    }
}
//mallocla ayrilan yeri freele
void freeSuffix(Node *n)
{
    if(n==NULL)
    {
        return;
    }
    for(int i=0;i<MAX_CHAR;i++)
    {
        if(n->child[i]!=NULL)
        {
            freeSuffix(n->child[i]);
        }
    }
    if(n->indexSuffix==-1)
        {
            free(n->sufEnd);
        }
    free(n);
}
//ana fonksiyon
void buildSuffix()
{
    size = strlen(text);

    rootEnd=(int*)malloc(sizeof(int));
    *rootEnd=-1;

    root = newNode(-1,rootEnd);

    activeNode= root;

    for(int i=0;i<size;i++)
    {
        extendSuffix(i);
    }
    int labelLen=0;
    setSuffixDFS(root,labelLen);

    freeSuffix(root);
}
//kac defa cikti frekansini kontrol et
int countFreq(char *pat, char *txt)
{
    int M = strlen(pat);
    int N = strlen(txt);
    int res = 0;


    for (int i = 0; i <= N - M; i++)
    {
        int j;
        for (j = 0; j < M; j++)
            if (txt[i+j] != pat[j])
                break;
        if (j == M)
        {
           res++;
           j = 0;
        }
    }
    return res;
}
//en uzun substring bulma
void substring(char s[], char sub[], int p, int len)
{
 int c = 0;
 while (c < len) {
 sub[c] = s[p+c];
  c++;
}
 sub[c] = '\0';
}

//longest common prefix
int lcp(char s[], char t[],char a[])
{
 int n = MIN(strlen(s),strlen(t));
for(int i = 0; i < n; i++){
  if(s[i] != t[i]){
   substring(s,a,0,i);
    return 0;
 }
}
substring(s,a,0,n);
return 0;
}
//main func
int main()
{
   // int gd= DETECT,gm;
    //initgraph(&gd,&gm,"None");
    FILE *veri;
    veri=fopen("C:\\Users\\Berk\\Desktop\\veri.txt","r");
    char yazi[MAX_CHAR];
    char karsilastir[MAX_CHAR];
    fscanf(veri, "%[^\n]", yazi);
    strcpy(text,yazi);
    LOOP:
    int prob=0;
    printf("\n\nSuffix Tree Program:\n");
    printf("1.Veri Dosyasinda ki Suffix'i build ve printle.\n");
    printf("2.Gireceginiz substring dosyada ki stringte var mi?\n");
    printf("3.Tekrarlanan en uzun substring'i bul.\n");
    printf("0.Cikis\n");
    printf("Hangi islemi yapmak istersiniz:");
    scanf("%d",&prob);
    fflush(stdin);
    fflush(stdout);
    if(prob==1)
    {
        buildSuffix();
    }
    if(prob==2)
    {
        scanf("%s",&karsilastir);
        char *ret;
        ret=strstr(text,karsilastir);
        int defa=countFreq(karsilastir,text);
        if(ret)
        {
            printf("<%s>Substringi bulunuyor ve %d defa tekrarlaniyor",karsilastir,defa);
        }
        else
            printf("<%s>Substringi bulunmuyor",karsilastir);
    }
    if(prob==3)
    {
     char lrs[30]={0}, x[30], res[30], sub[30],sub1[30];
    int n=strlen(text);
    for(int i=0;i<n;i++)
    {
        for(int j=i+1;j<n;j++)
        {
            substring(text,sub,i,n);
            substring(text,sub1,j,n);
            lcp(sub,sub1,x);
            if(strlen(x) > strlen(lrs)) strncpy(lrs, x, strlen(x));
        }
    }
    printf("Tekrarlanan en uzun substring: %s",lrs);
    }
    if(prob==0)
    {
        return 0;
    }

    goto LOOP;
    /*int array[]={1,2,3,4,5,6,7,8,9,10};//calismayan graph istenirse test edilebilir
    printSuffix(300,100,array,0,f);
    getch();
    closegraph();
    */
    return 0;
}
