
/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, říjen 2017                                        */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro převod využijte zásobník (tStack),
** který byl implementován v rámci příkladu c202. Bez správného vyřešení
** příkladu c202 se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro převod infixového výrazu 
**                       na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar .... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;


/*
** Pomocná funkce untilLeftPar.
** Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka
** bude také odstraněna. Pokud je zásobník prázdný, provádění funkce se ukončí.
**
** Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
** znaků postExpr. Délka převedeného výrazu a též ukazatel na první volné
** místo, na které se má zapisovat, představuje parametr postLen.
**
** Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
** nadeklarovat a používat pomocnou proměnnou typu char.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {

    if (stackEmpty(s)) {       //overenie že zásobník nie je prázdny
        return;
    }

    char c;                    //pomocná premenná
    do {
        stackTop(s, &c);        //načítanie vrchného prvku zo zásobníka
        if (c != '(') {
            postExpr[(*postLen)++] = c;     //vrchný prvok => výstup
            stackPop(s);                    //odstránenie vrchného prvku
        }
        } while (c != '(' );    //cyklus beží až po ľavú zátvorku

    stackPop(s);                //odstránenie ľavej zátvorky z vrcholu zásobníka
    return;
}

/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je předán parametrem c po načtení znaku ze
** vstupního pole znaků.
**
** Dle priority předaného operátoru a případně priority operátoru na
** vrcholu zásobníku rozhodneme o dalším postupu. Délka převedeného 
** výrazu a taktéž ukazatel na první volné místo, do kterého se má zapisovat, 
** představuje parametr postLen, výstupním polem znaků je opět postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {
    char top;

    while(1) {
        if (!stackEmpty(s)){        //načítanie znaku pre ďaľšie porovnávanie
            stackTop(s, &top);
        }
        if (stackEmpty(s)) {        //prázdny zásobník, operátor => top
            stackPush(s, c);
            break;
        } else if (top == '(') {    //na vrchole ľavá zátvorka, operátor => top
            stackPush(s, c);
            break;
        } else {
            switch (c) {            //na vrchole je operátor

                case '-' :          //operátory s nižšou/rovnakou prioritou
                case '+' :
                    stackTop(s, &postExpr[(*postLen)++]);   //operátor z vrcholu => výstupný reťazec
                    stackPop(s);
                    break;

                case '/' :          //operátory s vyššou/rovnakou prioritou
                case '*' :
                    if ((top == '*') || (top == '/')){          //rovnaká priorita
                        stackTop(s, &postExpr[(*postLen)++]);   //operátor z vrcholu => výstupný reťazec
                        stackPop(s);
                    } else stackPush(s, c);                     //nižšia priorita, operátor => top
                    break;
            }
        }
        if (!stackEmpty(s)) {       //načítanie znaku z vrcholu pre ďaľšie porovnávanie
            stackTop(s, &top);
        }
    }
    return;
}

/* 
** Konverzní funkce infix2postfix.
** Čte infixový výraz ze vstupního řetězce infExpr a generuje
** odpovídající postfixový výraz do výstupního řetězce (postup převodu
** hledejte v přednáškách nebo ve studijní opoře). Paměť pro výstupní řetězec
** (o velikosti MAX_LEN) je třeba alokovat. Volající funkce, tedy
** příjemce konvertovaného řetězce, zajistí korektní uvolnění zde alokované
** paměti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
**    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
**    násobení má stejnou prioritu jako dělení. Priorita násobení je
**    větší než priorita sčítání. Všechny operátory jsou binární
**    (neuvažujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
**
** 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
**    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
**    chybné zadání výrazu).
**
** 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen 
**    ukončovacím znakem '='.
**
** 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník použijte zásobník znaků tStack implementovaný v příkladu c202. 
** Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
**
** Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
**
** Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
** char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
** nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
** ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
** řetězce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {

    int i = 0;                  //počítadlo
    char* postExpr;             //výstupný reťazec
    tStack* s;                  //zásobník
    unsigned postLen = 0;       //dĺžka výstupného reťazca
    if ((postExpr = (char *) malloc(MAX_LEN)) == NULL) {    //kontrola úspešného alokovania pamäti
        return NULL;
    }
    if ((s = malloc(sizeof(char *) * STACK_SIZE)) == NULL) {
        free(postExpr);
        return NULL;
    }
    stackInit(s);                  //inicializácia zásobníka

     do {                          //cyklus pre prevod
        if (( infExpr[i] >= '0' && infExpr[i] <= '9' ) || ( infExpr[i] >= 'a' && infExpr[i] <= 'z' ) || ( infExpr[i] >= 'A' && infExpr[i] <= 'Z' )) {
            postExpr[postLen++] = infExpr[i];           //načítaný operand sa zapíše na výstupný reťazec
        }

        switch (infExpr[i]) {       //načítaný operátor/zátvorka/koncový znak '='

            case '+':
            case '-':
            case '*':
            case '/':
                doOperation(s, infExpr[i], postExpr, &postLen);     //operátor predaý funkcii
                break;

            case '(':
                stackPush(s, infExpr[i]);                   //ľavá zátvorka => top
                break;

            case ')':
                untilLeftPar(s, postExpr, &postLen);        //pravá zátvorka zavolá funkciu pre odstránenie páru zátvoriek
                break;

            case '=':                                       //načítanie koncového znaku
                while (!stackEmpty(s)){                     //cyklus pre vyprázdnenie zásobníka
                    stackTop(s, &postExpr[postLen++]);
                    stackPop(s);
                }
                postExpr[postLen] = '=';                    //vloženie koncového znaku na koniec výstupného reťazca
                break;
        }
    } while (infExpr[i++] != '=');              //prevodný cyklus beží až po načítanie koncového znaku
    return postExpr;
}

/* Konec c204.c */
