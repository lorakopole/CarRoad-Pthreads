//Program w języku c, zajmujący się obsługą wątków poprzez bibliotekę pthreads
//Etap 2 - sygnalizacja świetla - na skrzyzowaniach torów znajduje się sygnalizacja świetlna. Gdy światło jest zielone, samochód może przejechać przez skrzyżowanie. Jeśli jest czerwone, zatrzymuje się do czasu zmiany światła na zielone

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <vector>
using namespace std;

#define NUMBER_OF_CARS 10 //liczba aut na torze nr 2
#define NUMBER_OF_LOOPS 5 //liczba petli dla aut na torze nr1, +2 
#define LIGHTS_TIME 3000000 //liczba mikrosekund pomiędzy zmianami świateł
struct car //struktura auta
{
    char name;
    int x;
    int y;
    int loop;
    int velocity;
    int delay;
};

struct args //struktura przechowująca indeks auta i wskaźnik na światła
{
    int number;
    bool* lights_pointers;
};


pthread_t threads[NUMBER_OF_CARS];
struct args thread_args1[NUMBER_OF_CARS]; //tablica przechowująca indeksy aut i wskaźnik na światła (droga 1)


vector<car> road1;
vector<car> road2;
bool loop = true; //boolean odpowiedzialny za petle aut

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; //mutex dla skrzyżowania 1
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; //mutex dla skrzyżowania 2
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER; //mutex dla skrzyżowania 3
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER; //mutex dla skrzyżowania 4


pthread_cond_t cond[4];

void* printRoad(void* argument) //druk drogi
{
    bool* lights;
    lights = ((bool *)argument);

    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_GREEN);

    int x1=9;
    int y1=0;
    int x2=20;
    int y2=29;

    int x3=0;
    int y3=9;
    int x4=29;
    int y4=20;
    timeout(0);
    while(getch()!='x') //#działa dopóki nie kliknie się x
    {
    erase();

    //drukowanie torów

    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
    
    mvhline(y1+2, x1+2, 0, x2-x1-3);
    mvhline(y2-2, x1+2, 0, x2-x1-3);
    mvvline(y1+2, x1+2, 0, y2-y1-3);
    mvvline(y1+2, x2-2, 0, y2-y1-3);
    mvaddch(y1+2, x1+2, ACS_ULCORNER);
    mvaddch(y2-2, x1+2, ACS_LLCORNER);
    mvaddch(y1+2, x2-2, ACS_URCORNER);
    mvaddch(y2-2, x2-2, ACS_LRCORNER);
    
    
    mvhline(y3, x3, 0, x4-x3);
    mvhline(y4, x3, 0, x4-x3);
    mvvline(y3, x3, 0, y4-y3);
    mvvline(y3, x4, 0, y4-y3);
    mvaddch(y3, x3, ACS_ULCORNER);
    mvaddch(y4, x3, ACS_LLCORNER);
    mvaddch(y3, x4, ACS_URCORNER);
    mvaddch(y4, x4, ACS_LRCORNER);
    
    mvhline(y3+2, x3+2, 0, x4-x3-3);
    mvhline(y4-2, x3+2, 0, x4-x3-3);
    mvvline(y3+2, x3+2, 0, y4-y3-3);
    mvvline(y3+2, x4-2, 0, y4-y3-3);
    mvaddch(y3+2, x3+2, ACS_ULCORNER);
    mvaddch(y4-2, x3+2, ACS_LLCORNER);
    mvaddch(y3+2, x4-2, ACS_URCORNER);
    mvaddch(y4-2, x4-2, ACS_LRCORNER);
    
    //drukowanie aut

    for(int i=0;i<3;i++) 
    {
        move(road2.at(i).y,road2.at(i).x);
        addch(road2.at(i).name);
    }
    for(int i=0;i<road1.size();i++) 
    {
        if(road1.at(i).loop !=NUMBER_OF_LOOPS && road1.at(i).loop !=0) 
        {
            move(road1.at(i).y,road1.at(i).x);
            addch(road1.at(i).name);            
        }
    }

    //drukorwanie swiatel
    pthread_mutex_lock(&mutex1);
    if(*(lights) == false) //skrzyzowanie nr 1
    {
        attron(COLOR_PAIR(1));
        mvaddch(19,9,'N');
        attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(2));
        mvaddch(20,10,'Y');
        attroff(COLOR_PAIR(2));
    }
    else
    {
        attron(COLOR_PAIR(2));
        mvaddch(19,9,'Y');
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(1));
        mvaddch(20,10,'N');
        attroff(COLOR_PAIR(1));
    }
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_lock(&mutex2);
    if(*(lights+1) == false) //skrzyzowanie nr 2
    {
        attron(COLOR_PAIR(1));
        mvaddch(19,18,'N');
        attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(2));
        mvaddch(18,19,'Y');
        attroff(COLOR_PAIR(2));

    }
    else
    {
        attron(COLOR_PAIR(2));
        mvaddch(19,18,'Y');
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(1));
        mvaddch(18,19,'N');
        attroff(COLOR_PAIR(1));
    }
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_lock(&mutex3);

    if(*(lights+2) == false) //skrzyzowanie nr 3
    {
        attron(COLOR_PAIR(1));
        mvaddch(10,20,'N');
        attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(2));
        mvaddch(9,19,'Y');
        attroff(COLOR_PAIR(2));
    }
    else
    {
       attron(COLOR_PAIR(2));
        mvaddch(10,20,'Y');
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(1));
        mvaddch(9,19,'N');
        attroff(COLOR_PAIR(1));
    }
    pthread_mutex_unlock(&mutex3);
    pthread_mutex_lock(&mutex4);


    if(*(lights+3) == false) //skrzyzowanie nr 4
    {
        attron(COLOR_PAIR(1));
        mvaddch(10,11,'N');
        attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(2));
        mvaddch(11,10,'Y');
        attroff(COLOR_PAIR(2));
    }
    else
    {
        attron(COLOR_PAIR(2));
        mvaddch(10,11,'Y');
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(1));
        mvaddch(11,10,'N');
        attroff(COLOR_PAIR(1));
    }
    pthread_mutex_unlock(&mutex4);

    usleep(10);
    refresh();
    }
    loop = false;
    pthread_exit(0);
}

void createRoad2() //tworzenie drogi
{
    
    car temp;
    int Xpos;
    int Ypos;
    char name = 64;
    int speed = rand()%200 + 20;
    for(int j=0; j<3; j++)
    {
        Xpos = rand()%2;
        if(Xpos==0) Xpos=10;
        else Xpos=19;

        Ypos = (rand()%28) + 1;
        for(int i=0; i<road2.size();i++) //wybieranie pozycji Y dla wektora
        {
            if(road2.at(i).y == Ypos)
            {
            Ypos = (rand()%28) + 1;
            i=0;  
            }
        }
        name++;
        temp = {name,Xpos,Ypos,0,speed,0};
        road2.push_back(temp);
    }
}

void* MoveRoad2Cars(void* argument) //poruszanie aut
{
    bool* lights;
    int i;
    struct args *CarArgs;

    CarArgs = (struct args*) argument;
    lights = CarArgs->lights_pointers;
    i = CarArgs->number;

    while(loop)
    {
        if(road2.at(i).x<19 && road2.at(i).y == 1) //pozioma na gorze, 
        {
            road2.at(i).x++;
        }
        else if(road2.at(i).x == 19 && (road2.at(i).y >= 1 && road2.at(i).y <28)) //pionowa z prawej, skrzyzowanie 3 i 2
        {
            if(road2.at(i).y==8) //obsługa skrzyzowania nr 3
            {
                pthread_mutex_lock(&mutex3);
                while(*(lights+2) != false) pthread_cond_wait(&cond[2], &mutex3);
                road2.at(i).y++;
                pthread_mutex_unlock(&mutex3);
            }
            else if(road2.at(i).y==17) //obsługa skrzyzowania nr 2
            {
                pthread_mutex_lock(&mutex2);
                while(*(lights+1) != false) pthread_cond_wait(&cond[1], &mutex2);
                road2.at(i).y++;
                pthread_mutex_unlock(&mutex2);
            }
            else
            {
                road2.at(i).y++;
            }
        }
        else if(road2.at(i).x <= 19 && road2.at(i).x > 10 && road2.at(i).y ==28) //pozioma na dole
        {
            road2.at(i).x--;
        }
        else if(road2.at(i).x==10 && road2.at(i).y <=28) //pionowa z lewej, sprzyzowanie 1 i 4
        {
            if(road2.at(i).y==21) //obsługa skrzyzowania nr 1
            {
                pthread_mutex_lock(&mutex1);
                while(*(lights) != false) pthread_cond_wait(&cond[0], &mutex1);
                road2.at(i).y--;
                pthread_mutex_unlock(&mutex1);
            }
            else if(road2.at(i).y==12) //obsługa skrzyzowania nr 4
            {
                pthread_mutex_lock(&mutex4);
                while(*(lights+3) != false) pthread_cond_wait(&cond[3], &mutex4);
                road2.at(i).y--;
                pthread_mutex_unlock(&mutex4);
            }
            else
            {
                road2.at(i).y--;
            }

        }
        usleep(road2.at(0).velocity*1000);
    }
    pthread_exit(0);
}

car makeCar(int i) //tworzenie auta o indeksie i
{
    car temp;
    int Xpos=1;
    int Ypos=10;
    char name = 97 + i;
    int speed;
    speed = rand()%1000 + 100;
    temp = {name,Xpos,Ypos,0,speed,rand()%20000};
    return temp;
}
void createCars() //tworzenie aut
{
    car temp;
    for(int j=0; j<NUMBER_OF_CARS; j++)
    {
        temp = makeCar(j);
        road1.insert(road1.begin() + j, temp);
    }
}

void* moveCar(void* argument) //wątek poruszania się auta
{
    bool* lights;
    int i;
    struct args *CarArgs;

    usleep(10);

    CarArgs = (struct args*) argument;
    lights = CarArgs->lights_pointers;
    i = CarArgs->number;
    napms(road1.at(i).delay);
    road1.at(i).loop+=1;
    while(loop)
    {
        if(road1.at(i).y<19 && road1.at(i).x == 1) //pionowa z lewej
        {
            road1.at(i).y++;
        }
        else if(road1.at(i).y == 19 && (road1.at(i).x >= 1 && road1.at(i).x <28)) //pozioma na dole, skrzyzowania 1 i 2
        {
            if(road1.at(i).x == 8) //obsługa skrzyżowania nr 1
            {
                pthread_mutex_lock(&mutex1);
                while(*(lights) != true) pthread_cond_wait(&cond[0], &mutex1);
                road1.at(i).x++;
                pthread_mutex_unlock(&mutex1);
            }
            else if(road1.at(i).x == 17) //obsługa skrzyżowania nr 2
            {
                pthread_mutex_lock(&mutex2);
                while(*(lights+1) != true) pthread_cond_wait(&cond[1], &mutex2);
                road1.at(i).x++;
                pthread_mutex_unlock(&mutex2);
            }
            else 
            {
                road1.at(i).x++;
            }
        }
        else if(road1.at(i).y <= 19 && road1.at(i).y > 10 && road1.at(i).x ==28) //pionowa z prawej
        {
            road1.at(i).y--;
        }
        else if(road1.at(i).y==10 && road1.at(i).x <=28) //pozioma na gorze, skrzyzowania 3 i 4
        {
            if(road1.at(i).x == 21) //obsługa skrzyżowania nr 3
            {
                pthread_mutex_lock(&mutex3);
                while(*(lights+2) != true) pthread_cond_wait(&cond[2], &mutex3);
                road1.at(i).x--;
                pthread_mutex_unlock(&mutex3);
            }
            else if(road1.at(i).x == 12) //obsługa skrzyżowania nr 4
            {
                pthread_mutex_lock(&mutex4);
                while(*(lights+3) != true) pthread_cond_wait(&cond[3], &mutex4);
                road1.at(i).x--;
                pthread_mutex_unlock(&mutex4);
            }
            else 
            {
                road1.at(i).x--;
            }
        }
        usleep(road1.at(i).velocity*100);
        if(road1.at(i).x == 1 && road1.at(i).y == 19) road1.at(i).loop+=1;
        if(road1.at(i).loop == NUMBER_OF_LOOPS) break;  
    }
    pthread_exit(0);
}

void* checkIfFinish(void* argument) //wątek sprawdzajacy czy auto skonczylo okrążenia zeby dodac kolejne auto
{
    bool* lights;
    lights = ((bool *)argument);
    car temp;
    while(loop)
    {
    for(int i=0; i<NUMBER_OF_CARS;i++)
    {
        if(road1.at(i).loop == NUMBER_OF_LOOPS)
        {
            usleep(100);
            temp = makeCar(i);
            if(i==0) road1.erase(road1.begin());
            else road1.erase(road1.begin()+i);
            road1.insert(road1.begin() + i, temp);
            thread_args1[i].lights_pointers = lights;
            thread_args1[i].number = i;

            int result = pthread_create(&threads[i], NULL, moveCar,(void *) &thread_args1[i]);
            pthread_detach(threads[i]);
            if(result)
            {
                printf("MAIN: Thread %d was not created!\n", i);
            }
        }
    }
    }
    pthread_exit(0);
}


void* LightsChanger(void* argument) //funkcja odpowiedzialna za zmienianie świateł
{

    bool* lights;
    int i;
    struct args *Args;

    Args = (struct args*) argument;
    lights = Args->lights_pointers;
    i = Args->number;

    while(loop)
    {
        
        *(lights) = true;
        pthread_cond_broadcast(&cond[i]);
        usleep(LIGHTS_TIME);
        
        *(lights) = false;
        pthread_cond_broadcast(&cond[i]);
        usleep(LIGHTS_TIME);

    }
    pthread_exit(0);
}




int main()
{
    srand(time(NULL));
    initscr();
    curs_set(0);
    start_color();

    cond[0] = PTHREAD_COND_INITIALIZER;
    cond[1] = PTHREAD_COND_INITIALIZER;
    cond[2] = PTHREAD_COND_INITIALIZER;
    cond[3] = PTHREAD_COND_INITIALIZER;

    struct args crossing_args[4]; //tablica przechowująca indeksy swiatel i wskaznik na swiatlo
    struct args thread_args2[3]; //tablica przechowująca indeksy aut i wskaźnik na światła (droga 2)

    bool lights[4]; 
    *(lights) = true;
    *(lights+1) = true;
    *(lights+2) = true;
    *(lights+3) = true;

    for(int i=0; i<4; i++)
    {
        crossing_args[i].number = i;
        crossing_args[i].lights_pointers = &lights[i];
    }
    
    pthread_t crossing1;
    pthread_t crossing2; 
    pthread_t crossing3; 
    pthread_t crossing4; 
    int lc1 = pthread_create(&crossing1,NULL,LightsChanger, &crossing_args[0]);
    usleep(80000);
    int lc2 = pthread_create(&crossing2,NULL,LightsChanger, &crossing_args[1]);
    usleep(80000);
    int lc3 = pthread_create(&crossing3,NULL,LightsChanger, &crossing_args[2]);
    usleep(80000);
    int lc4 = pthread_create(&crossing4,NULL,LightsChanger, &crossing_args[3]);


    createRoad2();


    for(int i=0; i<3; i++)
    {
        thread_args2[i].number = i;
        thread_args2[i].lights_pointers = lights;
    }

    pthread_t road2Thread1;
    pthread_t road2Thread2;
    pthread_t road2Thread3;
    int c1r2 = pthread_create(&road2Thread1, NULL, MoveRoad2Cars, (void *) &thread_args2[0]);
    int c2r2 = pthread_create(&road2Thread2, NULL, MoveRoad2Cars, (void *) &thread_args2[1]);
    int c3r2 = pthread_create(&road2Thread3, NULL, MoveRoad2Cars, (void *) &thread_args2[2]);

    createCars();

    for(int i=0; i<NUMBER_OF_CARS;i++) 
    {
        thread_args1[i].lights_pointers = lights;
        thread_args1[i].number = i;
	
        int result = pthread_create(&threads[i], NULL, moveCar,(void *) &thread_args1[i]);
        if(result){
            printf("MAIN: Thread %d was not created!\n", i);
        }
    }
    pthread_t roadDraw;
    int rd = pthread_create(&roadDraw, NULL, printRoad, &lights);
    pthread_t CheckForFinish;
    int cff = pthread_create(&CheckForFinish, NULL, checkIfFinish, &lights);


    pthread_join(roadDraw,NULL); //wątek z obsługą przycisku wyłączenia programu
    endwin();
    //cout<<"zakonczono rysowanie"<<endl;

    pthread_join(CheckForFinish,NULL);
    //cout<<"zakonczono sprawdzanie czy auta skonczyly tor "<<endl;

    //procedura odblokowywania potencjalnie zablokowanych wątków
    // *(lights) = true;
    // *(lights+1) = true;
    // *(lights+2) = true;
    // *(lights+3) = true;
    // for(int i=0; i<4; i++) pthread_cond_broadcast(&cond[i]);

         for(int i=0; i<NUMBER_OF_CARS;i++) 
    {
        pthread_join(threads[i],NULL);
        //cout<<"zakonczono auto "<<i<<"na drodze 1"<<endl;
    }
   
    //cout<<"zakonczono droge 1"<<endl;
    pthread_join(road2Thread1,NULL);
    pthread_join(road2Thread2,NULL);
    pthread_join(road2Thread3,NULL);
    //cout<<"zakonczono droge 2"<<endl;
    pthread_join(crossing1, NULL);
    //cout<<"zakonczono skrzyzowanie 1"<<endl;
    pthread_join(crossing2, NULL);
    //cout<<"zakonczono skrzyzowanie 2"<<endl;
    pthread_join(crossing3, NULL);
    //cout<<"zakonczono skrzyzowanie 3"<<endl;
    pthread_join(crossing4, NULL);
    //cout<<"zakonczono skrzyzowanie 4"<<endl;


    
    road1.clear();
    road2.clear();
    return 0;
}

