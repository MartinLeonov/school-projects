/**
 * Projekt c. 2 - Santa Claus Problem 
 * Autor: Roman Blanco, xblanc01@stud.fit.vutbr.cz
 * Datum: 27.3.2013
 * Soubor: santa.c
 *
 * Popis:
 * Implementace modifikovaneho synchronizacniho problemu "Santa Claus 
 * Problem" :
 *
 * Skritci vyrabeji hracky. Pokud skritek narazi na problem, jde za 
 * Santou s zadosti o pomoc. Pote, co se skritkovi dostane pomoci od 
 * Santy, vraci se skritek zpet do prace. Jakmile pocet navstev skritka
 * u Santy dosahne predem stanoveneho poctu, skritek odjede na 
 * dovolenou. Jakmile Santa pomohl poslednimu skritkovi 
 * (vsichni jsou na dovolene), ukonci se procesy skritku a proces Santy.
 * Skritci vyrabejici hracky jsou aktivni skřítci, skritci na dovolene
 * jsou odpocivajici skritci.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>

// chybove konstanty (+help)
enum tecodes {
    EOK = 0,
    ECLWRONGCNT,
    ECLNOTNUM,
    ECLWRONGVAL,
    EFOPEN,
    EFORK,
    ESEMAPHORE,
    ESHM,
    ESHMCLEAR,
    HELPMSG,
};

const char const *ecode[] = {
    [EOK] =         "Vse je OK\n",
    [ECLWRONGCNT] = "Chybny pocet zadanych parametru\n"
    "Pro napovedu spustte program s parametrem -h nebo --help\n",
    [ECLNOTNUM] =   "Jeden ze zadanych parametru neni cislo\n"
    "Pro napovedu spustte program s parametrem -h nebo --help\n",
    [ECLWRONGVAL] = "Chybna hodnota jednoho z parametru\n"
    "Pro napovedu spustte program s parametrem -h nebo --help\n",
    [EFOPEN] =      "Chyba pri otevirani souboru pro vystup\n",
    [EFORK] =       "Fork procesu se nepovedl\n",
    [ESEMAPHORE] =  "Chyba pri vytvareni semaforu\n",
    [ESHM] =        "Nepodarilo se alokovat sdilenou pamet\n",
    [ESHMCLEAR] =   "Nepodarilo se odalokovat sdilenou pamet\n",
    [HELPMSG] =
    "IOS: Projekt 2 - Santa Claus Problem\n"
    "AUTOR:     Roman Blanco, xblanc01@stud.fit.vutbr.cz\n"
    "POPIS:     Implementace modifikovaneho synchronizacniho problemu\n"
    "POUZITI:   ./santa [C] [E] [H] [S]\n"
    "           - Vystup programu se zapise do souboru ./santa.out\n\n"
    "POPIS PARAMETRU:\n\n"
    " - Vsechny parametry jsou celociselne hodnoty\n"
    " - Casove udaje parametru [H] a [S] jsou dany v miliseknudach\n\n"
    " [C]  - pocet navstev u Santy, nez skritek muze na dovolenou\n"
    " [E]  - pocatecni pocet pracujicich skritku po spusteni\n"
    " [H]  - maximalni doba prace skritka, nez se vyskytne problem\n"
    " [S]  - maximalni doba pomoci, kterou dostane skritek od Santy\n\n"
    " [C]  >  0\n"
    " [E]  >  0\n"
    " [H]  >= 0\n"
    " [S]  >= 0\n\n"
    "PRIKLAD:   ./santa 20 20 5 5\n"
    "           ./santa 10 15 7 7\n",
};

// struktura pro parametry
typedef struct parameters {
    long visitCount;        // pocet navstev pred dovolenou     [C]
    long elfCount;          // pocatecni pocet skritku          [E]
    long maxProcessT;       // maximalni doba vyroby hracek     [H]
    long maxHelpT;          // maximalni doba pomoci            [S]
} Tparam;

// soubor s vystupem procesu
FILE *outFile = NULL;

// deklarovani semaforu
sem_t *elfSem, *santaSem, *cntASem, *blckSem, *exitSem, *incIDSem;

// makro k zavreni semaforu
#define CLEAN_SEM \
    do {\
        sem_close(santaSem);\
        sem_close(elfSem);\
        sem_close(cntASem);\
        sem_close(blckSem);\
        sem_close(exitSem);\
        sem_close(incIDSem);\
        fclose(outFile); }\
    while (0)

// ukazatele na sdilenou pamet
int *procNum , *numberElves, *waitingElves, *elfID = NULL;

// navratove hodnoty pro promenne ve sdilene pameti
int SHMprocNum, SHMnumberElves, SHMwaitingElves, SHMelfID = 0;

int getParameters(int argc, char *argv[], Tparam *parameter);
int openFile(FILE **outFile);
void santaWork(Tparam parameter);
void elfWork(Tparam parameter);
int incCounter(int *counter);
int allocMem(void);
int clearSources(void);
void killElves(int sighandler);

int main(int argc, char *argv[]) {
    Tparam parameter;
    pid_t santaPID;
    pid_t elfPID;
    int errnum;
    
    // odchyceni signalu
    signal(SIGTERM, killElves);
    signal(SIGINT, killElves);

    // zajisteni generovani nahodnych cisel
    srand(time(0));

    // nacteni parametru
    errnum = getParameters(argc, argv, &parameter);
    if (errnum != EOK) {
        if (errnum == HELPMSG) {
            fprintf(stdout, "%s", ecode[errnum]);
            return 0;
        }
        fprintf(stderr, "%s", ecode[errnum]);
        return 1;
    }

    // nacteni souboru pro vystup
    errnum = openFile(&outFile);
    if (errnum != EOK) {
        fprintf(stderr, "%s", ecode[errnum]);
        return 2;
    }

    // nastaveni bufferu, kvuli zapisu do souboru
    setbuf(outFile, NULL);

    // alokovani pameti
    errnum = allocMem();
    if (errnum != EOK) {
        fprintf(stderr, "%s", ecode[errnum]);
        return 2;
    }

    // definovani citacu (ve sdilene pameti)
    *procNum = 1;
    *numberElves = parameter.elfCount;
    *waitingElves = 0;
    *elfID = 1;

    // vytvoreni semaforu (pozn.: v /dev/shm)
    elfSem = sem_open("/elfSemf", O_CREAT | O_EXCL, 0666, 0);
    santaSem = sem_open("/santaSemf", O_CREAT | O_EXCL, 0666, 0);
    cntASem = sem_open("/countASemf", O_CREAT | O_EXCL, 0666, 1);
    blckSem = sem_open("/blockSemf", O_CREAT | O_EXCL, 0666, 1);
    exitSem = sem_open("/exitSemf", O_CREAT | O_EXCL, 0666, 0);
    incIDSem= sem_open("/incIDSemf", O_CREAT | O_EXCL, 0666, 1);
    // kontrola vytvoreni semaforu
    if(elfSem == SEM_FAILED 
    || santaSem == SEM_FAILED 
    || cntASem == SEM_FAILED 
    || blckSem == SEM_FAILED 
    || exitSem == SEM_FAILED 
    || incIDSem == SEM_FAILED) 
    {   // chyba pri vytvareni semaforu
        fprintf(stderr, "%s", ecode[ESEMAPHORE]);
        // vycisteni zdroju
        errnum = clearSources();
        if (errnum != EOK) {
            fprintf(stderr, "%s", ecode[errnum]);
            return 2;
        }
        return 2;
    }

  
    // vytvoreni procesu
    santaPID = fork();
    if (santaPID == -1) {
        // fork se nepovedl
        fprintf(stderr, "%s", ecode[EFORK]);
        errnum = clearSources();
        if (errnum != EOK) {
            fprintf(stderr, "%s", ecode[errnum]);
            return 2;
        }
        exit(2);
    } else if (santaPID == 0) {
        // child  -> proces santy
        santaWork(parameter);
    } else {
        // parent -> spoustim skritky
        for (int i = 0; i < parameter.elfCount; i++) {
            elfPID = fork();
            if (elfPID == -1) {
                // fork se nepovedl
                fprintf(stderr, "%s", ecode[EFORK]);
                // ukonceni Santy
                kill(santaPID, SIGTERM);
                // ukonceni skritku
                errnum = clearSources();
                if (errnum != EOK) {
                    fprintf(stderr, "%s", ecode[errnum]);
                    return 2;
                }
                killElves(0);
                exit(2);
            } else if (elfPID == 0) {
                // child  -> proces skritka
                elfWork(parameter);
            } else {
                // parent -> pokracovani v hlavnim procesu
            }
        }
        // parent -> pokracovani v hlavnim procesu
        // cekam na skonceni child procesu santy
        int statusSanta;
        wait(&statusSanta);
    }

    // uvolneni alokovane pameti
    errnum = clearSources();
    if (errnum != EOK) {
        fprintf(stderr, "%s", ecode[errnum]);
        return 2;
    }

    return 0; 
}

/**
 * Funkce: getParameters
 * ---------------------
 * Zpracuje a overi parametry zadane pri spusteni programu
 * Spravne pouziti parametru je popsane v napovede k programu
 */

int getParameters(int argc, char *argv[], Tparam *parameter) {
    char *testParam;
    
    if (argc == 2) {
        if ((strcmp(argv[1], "-h") == 0) ||
            (strcmp(argv[1], "--help") == 0)) {
            return HELPMSG;
        } else {
            return ECLWRONGCNT;
        }
    } else if (argc == 5) {
        // nacteni parametru do promennych
        parameter->visitCount = strtol(argv[1], &testParam, 10);
        if (testParam[0] != '\0') { return ECLNOTNUM; }
        parameter->elfCount = strtol(argv[2], &testParam, 10);
        if (testParam[0] != '\0') { return ECLNOTNUM; }
        parameter->maxProcessT = strtol(argv[3], &testParam, 10);
        if (testParam[0] != '\0') { return ECLNOTNUM; }
        parameter->maxHelpT = strtol(argv[4], &testParam, 10);
        if (testParam[0] != '\0') { return ECLNOTNUM; }
        // kontrola hodnot parametru   
        if ((parameter->visitCount <= 0) || 
            (parameter->elfCount <= 0)) {
            return ECLWRONGVAL;
        } else if ((parameter->maxProcessT < 0) || 
                   (parameter->maxHelpT < 0)) {
            return ECLWRONGVAL;
        } else {
            return EOK;
        }
    } else {
        return ECLWRONGCNT;
    }
}

/**
 * Funkce: openFile
 * ----------------
 * Vytvori soubor, do ktereho budou zapisovany hlaseni, ktere
 * tisknou Santa a skritci
 */
 
int openFile(FILE **outFile) {
    *outFile = fopen("santa.out", "w");
    if (*outFile == NULL) {
        return EFOPEN;
    } else {
        return EOK;
    }
}

/**
 * Funkce: santaWork
 * -----------------
 * Po obdrzeni pozadavku o pomoc od skritka overi pocet aktivnich
 * skritku a skritku cekajicich na obsluhu. Pokud jsou nejaci cekajici a
 * Santa muze provest obsluhu, Santa jim pomuze (prace Santy je 
 * simulovana jako uspani na nahodnou dobu v intervalu <0, [S]> ).
 * Pokud jsou jsou vsichni skritci odpocivajici, Santa se ukonci
 */
 
void santaWork(Tparam parameter) {
    // aby nedoslo k situaci, ze se vice procesu snazi zapisovat do
    // souboru najednou, pred kazdym print uzamknu semafor,
    // po print opet odemknu (cntASem)
    sem_wait(cntASem);
    fprintf(outFile, "%d: santa: started\n",
      incCounter(procNum));
    sem_post(cntASem);
    while (1) {
        sem_wait(santaSem); // Santa spi
        //------------------// Santa se vzbudi (skritek odemkl semafor)
        sem_wait(cntASem);
        fprintf(outFile, "%d: santa: checked state: %d: %d\n",
          incCounter(procNum), *numberElves, *waitingElves);
        sem_post(cntASem);
        if (*numberElves != 0) {
            sem_wait(cntASem);
            fprintf(outFile, "%d: santa: can help\n",
              incCounter(procNum));
            sem_post(cntASem);
            // "prace Santy" = obslouzeni skritku
            if (parameter.maxHelpT != 0) {
                usleep(rand()%(parameter.maxHelpT)*1000);
            }
            // ulozim kolik skritku mam pustit (kolik bylo obslouzeno)
            // do nesdilene promenne
            int toRelease = *waitingElves;
            for (int j = 0; j < toRelease; j++) {
                sem_post(elfSem);
            }
        } else {
            // vsichni skritci odpocivaji, odemkne semafor pro ukonceni
            for (int i = 0; i < parameter.elfCount; i++) {
                sem_post(exitSem);
            }
            sem_wait(cntASem);
            fprintf(outFile, "%d: santa: finished\n",
              incCounter(procNum));
            sem_post(cntASem);
            // uzavreni semaforu
            CLEAN_SEM;
            exit(0);
        }
    }
}

/**
 * Funkce: elfWork
 * ---------------
 * Po spusteni se skritek pousti do prace, simulovanou jako uspani na
 * nahodny cas v intervalu <0, [H]>. 
 * Pri vyskytnuti problemu zada Santu o pomoc. Pokud je pocet aktivnich
 * skritku >3, za Santou muze jit pouze skupina 3 skritku. Pri 3 a mene
 * mohou chodit k Santovi jednotlive.
 * Pocet navstiveni Santy pred odchodem na dovolenou je dany parametrem
 * [C].
 * Po skonceni prace skritek odchazi na dovolenou; ve chvili kdy jsou
 * vsichni skritci na dovolene se proces skritka ukonci
 */

void elfWork(Tparam parameter) {
    // ID skritka
    sem_wait(incIDSem);
    int i = incCounter(elfID);
    sem_post(incIDSem);

    sem_wait(cntASem);
    fprintf(outFile, "%d: elf: %d: started\n",
      incCounter(procNum), i);
    sem_post(cntASem);
    for (int c = 0; c < parameter.visitCount; c++) {
        // "prace skritka"
        if (parameter.maxProcessT != 0) {
            usleep(random()%(parameter.maxProcessT)*1000);
        }
        sem_wait(cntASem);
        fprintf(outFile, "%d: elf: %d: needed help\n",
          incCounter(procNum), i);
        sem_post(cntASem);
        if (*numberElves > 3) {
            // pokud nejsou posledni 3 skritci
            sem_wait(blckSem);
            // brani, aby cekalo u Santy vice skritku nez 3
        }
        sem_wait(cntASem);
        fprintf(outFile, "%d: elf: %d: asked for help\n",
          incCounter(procNum), i);
        sem_post(cntASem);
        sem_wait(cntASem);
        (*waitingElves)++;
        sem_post(cntASem);
        if (*numberElves > 3) {
            if (*waitingElves == 3) {
                // pocet cekajicich skritku == 3, muzou vzbudit Santu
                sem_post(santaSem);
            } else { 
                // musi cekat tri na vzbuzeni Santy
                // odemkne semafor, aby mohl se pripojil dalsi
                sem_post(blckSem);
            }
        } else {
            // pocet aktivnich skritku <= 3, muze vzbudit Santu i 1
            sem_post(santaSem);
        }
        // skritci cekaji, az jim Santa umozni vstup
        sem_wait(elfSem);
        sem_wait(cntASem);
        fprintf(outFile, "%d: elf: %d: got help\n",
          incCounter(procNum), i);
        sem_post(cntASem);
        sem_wait(cntASem);
        (*waitingElves)--;
        sem_post(cntASem);
        if (*waitingElves == 0) {
            // od Santy odesel posledni ze skupiny
            // otevre semafor pro dalsi cekajici
            sem_post(blckSem);
        }
    }
    sem_wait(cntASem);
    fprintf(outFile, "%d: elf: %d: got a vacation\n",
      incCounter(procNum), i);
    sem_post(cntASem);
    sem_wait(cntASem);
    (*numberElves)--;
    sem_post(cntASem);
    if ((*numberElves == 0) && (*waitingElves == 0)) {
        // posledni skritek dokoncil praci -> vzbudi Santu
        sem_post(santaSem);
    }
    sem_wait(exitSem); // skritek je na dovolene, ceka na ukonceni
    //-----------------// Santa odemkl semafor, skritek se ukonci
    sem_wait(cntASem);
    fprintf(outFile, "%d: elf: %d: finished\n",
      incCounter(procNum), i);
    sem_post(cntASem);
    // uzavreni semaforu
    CLEAN_SEM;
    exit(0);
}

/**
 * Funkce: incCounter
 * -------------------
 * Je volana pokazde, kdyz Santa nebo skritek tiskne hlaseni.
 * Zajistuje chronologicke cislovani akci (hlaseni).
 * Funkce take prirazuje jednotlivym procesum skritku ID postupne
 * podle poradi spusteni (interval je dany parametrem [E]: <1, [E]>)
 */
 
int incCounter(int *counter) {
    int incCounter = 0;

    incCounter = *counter;
    (*counter)++;
    return incCounter;
}

/**
 * Funkce: allocMem
 * ----------------
 * Alokuje misto ve sdilene pameti pro promenne, se kterymi
 * pracuji proces Santy a procesy skritku
 */

int allocMem(void) {
    // alokovani sdilene pameti pro procNum (citac procesu)
    if((SHMprocNum = shmget(IPC_PRIVATE, sizeof(int),
     IPC_CREAT | IPC_EXCL | 0666)) == -1)
    { return ESHM; }
     
    if((procNum = (int *)shmat(SHMprocNum, NULL, 0)) == NULL)
    { return ESHM; }
    
    // alokovani sdilene pameti pro numberElves (pocet aktivnich elfu)
    if((SHMnumberElves = shmget(IPC_PRIVATE, sizeof(int),
     IPC_CREAT | IPC_EXCL | 0666)) == -1) 
    { return ESHM; }
     
    if((numberElves = (int *)shmat(SHMnumberElves, NULL, 0)) == NULL)
    { return ESHM; }
    
    // alokovani sdilene pameti pro numberElves (pocet cekajicich elfu)
    if((SHMwaitingElves = shmget(IPC_PRIVATE, sizeof(int),
     IPC_CREAT | IPC_EXCL | 0666)) == -1)
    { return ESHM; }
    
    if((waitingElves = (int *)shmat(SHMwaitingElves, NULL, 0)) == NULL)
    { return ESHM; }
    
    // alokovani sdilene pameti pro elfID (identifikacni cislo elfa)
    if((SHMelfID = shmget(IPC_PRIVATE, sizeof(int),
     IPC_CREAT | IPC_EXCL | 0666)) == -1)
    { return ESHM; }
    
    if((elfID = (int *)shmat(SHMelfID, NULL, 0)) == NULL)
    { return ESHM; }
    
    return EOK;
}

/**
 * Funkce: clearSources
 * --------------------
 * Vycisti a overi uspesne uvolneni pameti alokovane programem,
 * uzavre soubor slouzici k zapisu akci, a vycisti pamet po semaforech 
 */

int clearSources(void) {
    // uvolneni semaforu
    sem_unlink("/elfSemf");
    sem_unlink("/santaSemf");
    sem_unlink("/countASemf");
    sem_unlink("/blockSemf");
    sem_unlink("/exitSemf");
    sem_unlink("/incIDSemf");

    // odalokovani sdilene pameti
    if(shmctl(SHMprocNum, IPC_RMID, NULL) == -1)
        { return ESHMCLEAR;}
    if(shmctl(SHMnumberElves, IPC_RMID, NULL) == -1)
        { return ESHMCLEAR; }
    if(shmctl(SHMwaitingElves, IPC_RMID, NULL) == -1)
        { return ESHMCLEAR; }
    if(shmctl(SHMelfID, IPC_RMID, NULL) == -1)
        { return ESHMCLEAR; }

    // uzavreni souboru pro zapis
    fclose(outFile);

    return EOK;
}

/**
 * Funkce: killElves
 * -----------------
 * V pripade preruseni ukonci bezici procesy skritku a vycisti
 * pridelenou pamet
 */
 
void killElves(int sighandler) {
    // aby prekladac nehlasil unused
    (void)sighandler;
    // ukonceni skritku
    kill(getpid(), SIGTERM);
    clearSources();
    exit(2);
}
