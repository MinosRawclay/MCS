#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PLAYERS_MAX 4 //correspond aux règle de la bellote
#define NB_CARD_HAND 8
#define NB_CARD_DECK 32

#define STR_ROUGE_START printf("\033[31m")
#define STR_MAGENTA_START printf("\033[35m")
#define STR_VERT_START printf("\033[32m")
#define STR_BLEU_START printf("\033[34m")
#define STR_COLOR_END printf("\033[0m")

// ==================== DECLARATION ==============================================
enum card {
    H_AS, H_7, H_8, H_9, H_10, H_V, H_D, H_R,
    C_AS, C_7, C_8, C_9, C_10, C_V, C_D, C_R,
    P_AS, P_7, P_8, P_9, P_10, P_V, P_D, P_R,
    T_AS, T_7, T_8, T_9, T_10, T_V, T_D, T_R,NOTHING=-1
}card_t;
enum colorAtout {H,C,P,T};
enum equipe {EQUIPE1,EQUIPE2};
enum state {FINISH,PLAYING,WAITING};
typedef struct player
{
    //socket_t sd;
    int num;
    enum state s;
    int cards[NB_CARD_HAND];
}player_t;

typedef player_t* players_t[PLAYERS_MAX] ;

typedef struct pileCard
{
    enum card deck[NB_CARD_DECK];
    int lastcard;
}pileCard_t;

typedef enum card pli_t[PLAYERS_MAX];

const int value[9] = {11, 0, 0, 0, 10, 2, 3, 4,0};
const int valueAtout[9] = {11, 0, 0, 14, 10, 20, 3, 4,0};
const char *card_names[NB_CARD_DECK] = {
    "AS","7","8","9","10","V","D","R",
    "AS","7","8","9","10","V","D","R",
    "AS","7","8","9","10","V","D","R",
    "AS","7","8","9","10","V","D","R"
};

// ==================== GLOBALE VARIABLE ==================================================



// ==================== FONCTION DECLARATION ==============================================
//Communication
bool addPlayer(players_t players, int *nbPlayer);
void giveCard(players_t players, int playingPlayer);
void givePli(players_t players, pli_t pli);
bool askTakeAtout(players_t players, int player);
bool askTakeAtoutTurn2(players_t players, int player, enum colorAtout *c);

//Card
enum card dealCard(pileCard_t* pileDeck);
void resetCards(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2, players_t players);
int string_to_card(const char *s, enum card *out) ;
int point_of_gain(pileCard_t* pileEq1,pileCard_t* pileEq2, enum equipe eq,enum colorAtout c);
void cardShuffle(pileCard_t* pileDeck) ;
bool verifColor(char color);
int betterInPli(pli_t pli, enum colorAtout c);

//Game
void firstDeal(pileCard_t* deck, players_t players, int * startPlayer, pli_t pli);
int playerTurnAtout(int turn,int * startPlayer,enum colorAtout *c);
void secondDeal(pileCard_t* deck, players_t players,int* startPlayer,pli_t pli);
void thirdDeal(pileCard_t* deck, players_t players,int* startPlayer, pli_t pli, int playerTakeAtout);
bool turnDeal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorAtout * c);
int nextPlayingPlayer(int* startPlayer, int nbNextPlayer);
void turnNormal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorAtout * c);

//Affichage
void str_color(enum card card);
void afficherGainEq(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2);
void afficherEq(pileCard_t* pileEq);
void afficherDeck(pileCard_t* pileDeck);
void afficherPlayers(players_t players);
void afficherPli(pli_t pli);

// ==================== COMMUNICATION =====================================================
bool addPlayer(players_t players, int *nbPlayer){
    if (*nbPlayer>=4){return false;}

    players[*nbPlayer] = (player_t*) malloc(sizeof(player_t));
    if (&players[*nbPlayer] == NULL) {
        return false;
    }
    players[*nbPlayer]->num=*nbPlayer;
    players[*nbPlayer]->s = FINISH;
    for (int i = 0; i < NB_CARD_HAND; i++)
    {
        players[*nbPlayer]->cards[i]=NOTHING;
    }
    (*nbPlayer)++;
    return true;
}

//TODO --> transmet toutes les cartes du joueur au joueur
void giveCard(players_t players, int player){
    return;
}

//TODO --> transmett le pli au joueurs
void givePli(players_t players, pli_t pli){
    afficherPli(pli);
    afficherPlayers(players);
    return;
}

//TODO --> demander si tu veux accepter la carte du milieu
bool askTakeAtout(players_t players, int player){
    // if(player == 0){CLient INTERNE}
    printf("Tu prend l'atout T1 ? \n");
    printf("yes=1,no=0 :");
    char choice;
    scanf(" %c",&choice);
    if(choice == '1') return true; 
    return false;
}

//TODO --> chose atout color
bool askTakeAtoutTurn2(players_t players, int player, enum colorAtout *c){
    char color=0;
    char choice=0;
    printf("Tu prend l'atout T2 ? \n");
    printf("yes=1,no=0 :");
    scanf(" %c",&choice);
    
    if(choice == '0') return false; 
    do
    {
        printf("color (H,C,P,T) ? \n");
        scanf(" %c",&color);
    } while (!verifColor(color));
    
    *c=color;
    printf("Color : %c\n",color);
    return true;
}

enum card askCard(players_t players, int player){
    // if(player == 0){CLient INTERNE}
    printf("Tu met quel card ? \n");
    char choice;
    scanf(" %c",&choice);
    if(choice == '1') return true; 
    return false;
}

// ==================== CARD ==============================================================

enum card dealCard(pileCard_t* pileDeck){
    enum card c = pileDeck->deck[pileDeck->lastcard];
    pileDeck->deck[pileDeck->lastcard] = -1;
    pileDeck->lastcard++;
    return c;
}

void resetCards(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2, players_t players){
    for (int i = 0; i < 32; i++)
    {
        pileEq1->deck[i]=NOTHING;
        pileEq1->lastcard = 0;
        pileEq2->deck[i]=NOTHING;
        pileEq2->lastcard = 0;
        pileDeck->deck[i]=i;
        pileDeck->lastcard = 0;
    }
    pileDeck->lastcard = 0;
    for (int i = 0; i < PLAYERS_MAX; i++){
        for (int j = 0; j < NB_CARD_HAND; j++)
            {players[i]->cards[j]=NOTHING;}
        players[i]->s = FINISH; 
        //players[i]->num = i;   
    }
}

int string_to_card(const char *s, enum card *out) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(s, card_names[i]) == 0) {
            *out = (enum card)i;
            return 1;
        }
    }
    return 0;
}

int point_of_gain(pileCard_t* pileEq1,pileCard_t* pileEq2, enum equipe eq,enum colorAtout c){
    int points=0;
    int color=0;
    int numero=0;
    int i=0;
    enum card card;

    if (eq == EQUIPE1){
        while ((card=pileEq1->deck[i])!=NOTHING)
        {
            color = card/8;
            numero = card - color*8;
            points+=(color==c)?valueAtout[numero]:value[numero];
            //printf("color:%d,c:%d\n",color,c);
            //printf("i:%d--%s;temp:%d;point:%d\n",i,card_names[i],numero,points);
            //printf("val:%d;valA:%d\n",value[numero],valueAtout[numero]);
            i++;
        }
    }
    else
    {
       while ((card=pileEq2->deck[i])!=NOTHING)
        {
            color = card/8;
            numero = card - color*8;
            points+=(color==c)?valueAtout[numero]:value[numero];
            i++;
        } 
    }
    return points;
}

void cardShuffle(pileCard_t* pileDeck) {
    for (int i = NB_CARD_DECK - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        enum card temp = pileDeck->deck[j];
        pileDeck->deck[j] = pileDeck->deck[i];
        pileDeck->deck[i] = temp;
    }
    pileDeck->lastcard;
}

bool verifColor(char color){
    if(color == 'H' || color == 'C' || color == 'P' || color == 'T') {
        return true;}
    return false;
}

/**
 *	\fn			int betterInPli(enum colorAtout c)
 *	\brief		renvoit l'indice du joueur gagnant d'un pli
 *	\param 		
 *	\note		On trouve le meilleur du pli a partir de la  
 *	\result		paramètre sockEch modifié pour le mode DGRAM
 */
int betterInPli(pli_t pli, enum colorAtout c){

    int color, numero, bestVal, val, bestP, bestColor;
    enum card card, bestCard;
    
    bestCard = pli[0];
    bestP = 0;
    bestColor = bestCard/8;
    numero = bestCard - bestColor*8;
    bestVal = (bestColor==c)?valueAtout[numero]:value[numero];
    for (int i = 1; i < PLAYERS_MAX; i++)
    {
        card = pli[i];
        color = card/8;
        numero = card - color*8;
        if (color==c)//Card is Atout
        {        
            if (bestColor==c){// Both are Atout
                val = valueAtout[numero];
                if (val < bestVal) break; // new Card has a smaller value thant the old
                // Both are atout and of same value -> 7 of Atout & 8 of Atout, so the biggest is the best                
                if(val = bestVal && card<=bestCard) break; 
            }
            // old card isn't Atout
               
        }
        else//Card isn't Atout
        {
            if(bestColor==c)break;//old is a Atout
            val = value[numero];
            if (val < bestVal) break; // new Card has a smaller value thant the old
            // Both are atout and of same value -> 7 of Atout & 8 of Atout, so the biggest is the best                
            if(val = bestVal && card<=bestCard) break; 
        }
        bestColor = color;
        bestCard = card;
        bestP = i;
    }
    
}

void initPile(pileCard_t **pile)
{
    *pile = malloc(sizeof(pileCard_t));
    if (*pile == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}



// ==================== GAME ==============================================
void firstDeal(pileCard_t* deck, players_t players, int * startPlayer, pli_t pli){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        players[playingPlayer]->cards[0]=dealCard(deck);
        players[playingPlayer]->cards[1]=dealCard(deck);
        players[playingPlayer]->cards[2]=dealCard(deck);
        giveCard(players, playingPlayer);
        i++;
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
    givePli(players, pli);
}

int playerTurnAtout(int turn,int * startPlayer,enum colorAtout *c){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        if(turn == 1)
            {if(askTakeAtout(playingPlayer))return playingPlayer;}
        else
            {if(askTakeAtoutTurn2(playingPlayer,c))return playingPlayer;}
        i++;   
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
    return -1;
}

void secondDeal(pileCard_t* deck, players_t players,int* startPlayer,pli_t pli){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        players[playingPlayer]->cards[3]=dealCard(deck);
        players[playingPlayer]->cards[4]=dealCard(deck);
        giveCard(players, playingPlayer);  
        i++;
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
    pli[0]=dealCard(deck);
}

void thirdDeal(pileCard_t* deck, players_t players,int* startPlayer, pli_t pli, int playerTakeAtout){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        if(playingPlayer==playerTakeAtout){
            players[playingPlayer]->cards[5]=pli[0];
            pli[0]==NOTHING;
        }
        else players[playingPlayer]->cards[5]=dealCard(deck);
        players[playingPlayer]->cards[6]=dealCard(deck);
        players[playingPlayer]->cards[7]=dealCard(deck);
        giveCard(players, playingPlayer); 
        i++;   
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
}

bool turnDeal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorAtout * c){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    int p;
    resetCards(deck,pileEq1,pileEq2, players);
    cardShuffle(deck);
    firstDeal(deck,players,startPlayer,pli);
    secondDeal(deck,players,startPlayer,pli);
    givePli(players,pli);
    if((p = playerTurnAtout(1,startPlayer,c))==-1)
        if((p = playerTurnAtout(2,startPlayer,c))==-1)
            return false;
    thirdDeal(deck,players,startPlayer,pli,p);
    return true;
}

void turnNormal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorAtout * c){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
          
        i++;
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
}

int nextPlayingPlayer(int* startPlayer, int nbNextPlayer){
    return (*startPlayer+nbNextPlayer)%PLAYERS_MAX;
}



// ==================== AFFICHAGE =========================================
void str_color(enum card card){
    enum colorAtout color = card/8;
    //if (color == c){STR_VERT_START;return;}
    switch (color)
    {
    case H:
        STR_ROUGE_START;
        break;
    case C:
        STR_MAGENTA_START;
        break;
    case P:
        STR_BLEU_START;
        break;
    case T:
        STR_VERT_START;
        break;
    
    default:
        break;
    }
}

void afficherGainEq(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2){
    int j=0;
    enum card card=NOTHING;
    printf("Deck\n");
    afficherDeck(pileDeck);
    printf("Equipe1\n");
    afficherEq(pileEq1);
    printf("\nFin Eq1\n");
    printf("Equipe2\n");
    afficherEq(pileEq2);
    printf("\nFin Eq2\n");
}

void afficherEq(pileCard_t* pileEq){
    int j=0;
    enum card card=NOTHING;
    while ((card=pileEq->deck[j])!=NOTHING)
    {
        str_color(card);
        printf("-%s-",card_names[card]);
        STR_COLOR_END;
        j++;
    }
}

void afficherDeck(pileCard_t* pileDeck){
    for (int i = pileDeck->lastcard; i < NB_CARD_DECK; i++)
    {
        str_color(pileDeck->deck[i]);
        printf("-%s-",card_names[pileDeck->deck[i]]);
        STR_COLOR_END;
        i++;
    }
    
}

void afficherPlayers(players_t players){
    for (int i = 0; i < PLAYERS_MAX; i++)
    {
        printf("player n°%d : state=%d\n",players[i]->num,players[i]->s);
        for (int j = 0; j < NB_CARD_HAND; j++)
        {
            if(players[i]->cards[j]==NOTHING){
                printf("-o-");
            }
            else{
                str_color(players[i]->cards[j]);
                printf("-%s-",card_names[players[i]->cards[j]]);
                STR_COLOR_END;
            }
        }
        printf("\n");
    }
}

void afficherPli(pli_t pli){
    printf("Pli : ");
    for (int i = 0; i < 4; i++)
    {
        if (pli[i]==NOTHING)
            {printf("-o-");}
        else{
            str_color(pli[i]);
            printf("-%s-",card_names[pli[i]]);
            STR_COLOR_END;
        }  
    }
    printf("\n");
}
    
int main(int argc, char const *argv[])
{
    pileCard_t* deck;//les cartes existante dans le deck
    int lastDeal = 0;
    pileCard_t* gain_Eq1;
    pileCard_t* gain_Eq2;
    enum card pli[PLAYERS_MAX] = {NOTHING,NOTHING,NOTHING,NOTHING};
    player_t *players[PLAYERS_MAX];
    int startPlayer=0;
    enum colorAtout c;

    int nbPlayer=0;
    STR_ROUGE_START;
    printf("TEST\n");
    STR_COLOR_END;
    
    // =============================================================
    //création des 4 joueur - connection des clients
    while (nbPlayer < 4)
    {
        addPlayer(players,&nbPlayer);
    }
    afficherPlayers(players);
    
    // =============================================================
    
    printf("TESSETTSS555");
    getc(stdin);
    initPile(&gain_Eq1);
    initPile(&gain_Eq2);
    initPile(&deck);
    turnDeal(deck,gain_Eq1,gain_Eq2,players,&startPlayer,pli,&c);
    
    return 0;
}
