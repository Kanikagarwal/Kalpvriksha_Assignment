#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Players_data.h"

#define MAX_TEAMS 10
#define MAX_PLAYERS 50
#define MAX_NAME 50

// This enum decides that the player has which role in the team
enum { BATSMAN = 1, BOWLER = 2, ALL_ROUNDER = 3 };
// Structure for player
typedef struct PlayerNode {
    int id;
    char name[MAX_NAME + 1];
    char teamName[MAX_NAME + 1];
    int role;
    int runs;
    float average;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerNode *next;
} PlayerNode;

// Structure for team and each role has its own linked list
typedef struct {
    int id;
    char name[MAX_NAME + 1];
    int totalPlayers;
    float strikeRateSum;
    int battingPlayers;
    //  Each role has its own linked list.
    PlayerNode *players[3];
} Team;

typedef struct { PlayerNode *player; } HeapItem;

Team teamsList[MAX_TEAMS];
int totalTeams = 0;

/* Function declarations */
void initializeTeams();
void loadPlayers();
int readInt(const char *message, int min, int max);
float readFloat(const char *message, float min, float max);
void readName(const char *message, char name[]);
int getRole(const char *roleName);
const char *roleToString(int role);
float calculateIndex(int role, float average, float strikeRate, int wickets, float economyRate);
Team *findTeamById(int id);
Team *findTeamByName(const char *name);
PlayerNode *createPlayer(int id, const char *name, const char *teamName, int role, int runs, float average, float strikeRate, int wickets, float economyRate);
void insertPlayer(Team *team, PlayerNode *newPlayer);
int playerIdAlreadyUsed(int id);
float getAverageStrikeRate(const Team *team);
void addPlayer();
void displayTeamPlayers();
void displayTeamsByStrikeRate();
void displayTopKPlayers();
void displayPlayersByRole();
void freeMemory();
void printPlayerHeading();
void printPlayer(const PlayerNode *player);
void pushHeap(HeapItem heap[], int *size, PlayerNode *player);
PlayerNode *popHeap(HeapItem heap[], int *size);
int hasHigherIndex(PlayerNode *first, PlayerNode *second);

// Function for reading the integer input in the entire program
int readInt(const char *message, int min, int max)
{
    char line[100], *end;
    long value;
    while (1) {
        printf("%s", message);
        if (fgets(line, sizeof(line), stdin) == NULL) exit(0);
        value = strtol(line, &end, 10);
        while (isspace((unsigned char)*end)) end++;
        if (end == line || *end != '\0' || value < min || value > max)
            printf("Please enter a valid number between %d and %d.\n", min, max);
        else return (int)value;
    }
}
// Function for reading the floating input in the entire program
float readFloat(const char *message, float min, float max)
{
    char line[100], *end;
    float value;
    while (1) {
        printf("%s", message);
        if (fgets(line, sizeof(line), stdin) == NULL) exit(0);
        value = strtof(line, &end);
        while (isspace((unsigned char)*end)) end++;
        if (end == line || *end != '\0' || value < min || value > max)
            printf("Please enter a valid value between %.2f and %.2f.\n", min, max);
        else return value;
    }
}
// Function for reading a string
void readName(const char *message, char name[])
{
    char line[100];
    size_t length;
    while (1) {
        printf("%s", message);
        if (fgets(line, sizeof(line), stdin) == NULL) exit(0);
        line[strcspn(line, "\n")] = '\0';
        length = strlen(line);
        if (length == 0 || length > MAX_NAME)
            printf("Name should contain 1 to %d characters.\n", MAX_NAME);
        else { strcpy(name, line); return; }
    }
}
// A helper function for converting the role to printable string
const char *roleToString(int role)
{
    if (role == BATSMAN) return "Batsman";
    if (role == BOWLER) return "Bowler";
    return "All-rounder";
}
// A helper function for converting the user input into program specified manner
int getRole(const char *roleName)
{
    if (strcmp(roleName, "Batsman") == 0) return BATSMAN;
    if (strcmp(roleName, "Bowler") == 0) return BOWLER;
    if (strcmp(roleName, "All-rounder") == 0) return ALL_ROUNDER;
    return 0;
}

// Calculating performance index accoridng to the formula given for the roles
float calculateIndex(int role, float average, float strikeRate, int wickets, float economyRate)
{
    float battingScore = average * strikeRate / 100.0f;
    if (role == BATSMAN) return battingScore;
    if (role == BOWLER) return wickets * 2.0f + 100.0f - economyRate;
    return battingScore + wickets * 2.0f;
}
// Initializing team variables
void initializeTeams()
{
    int i, j;
    totalTeams = teamCount;
    if (totalTeams > MAX_TEAMS) totalTeams = MAX_TEAMS;
    for (i = 0; i < totalTeams; i++) {
        teamsList[i].id = i + 1;
        strcpy(teamsList[i].name, teams[i]);
        teamsList[i].totalPlayers = 0;
        teamsList[i].strikeRateSum = 0;
        teamsList[i].battingPlayers = 0;
        for (j = 0; j < 3; j++) teamsList[i].players[j] = NULL;
    }
}
// Finding the team with a given ID
Team *findTeamById(int id)
{
    int low = 0, high = totalTeams - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (teamsList[mid].id == id) return &teamsList[mid];
        if (teamsList[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return NULL;
}
// Finding a team with the given Name
Team *findTeamByName(const char *name)
{
    int i;
    for (i = 0; i < totalTeams; i++)
        if (strcmp(teamsList[i].name, name) == 0) return &teamsList[i];
    return NULL;
}
// Getting the Average Strike Rate
float getAverageStrikeRate(const Team *team)
{
    if (team->battingPlayers == 0) return 0;
    return team->strikeRateSum / team->battingPlayers;
}

// Creatung a new player and allocating memory by making a new Node
PlayerNode *createPlayer(int id, const char *name, const char *teamName, int role, int runs, float average, float strikeRate, int wickets, float economyRate)
{
    PlayerNode *newPlayer = malloc(sizeof(PlayerNode));
    if (newPlayer == NULL) { printf("Memory allocation failed.\n"); return NULL; }
    newPlayer->id = id;
    strcpy(newPlayer->name, name);
    strcpy(newPlayer->teamName, teamName);
    newPlayer->role = role;
    newPlayer->runs = runs;
    newPlayer->average = average;
    newPlayer->strikeRate = strikeRate;
    newPlayer->wickets = wickets;
    newPlayer->economyRate = economyRate;
    newPlayer->performanceIndex = calculateIndex(role, average, strikeRate, wickets, economyRate);
    newPlayer->next = NULL;
    return newPlayer;
}
// Inserting the player into the team
void insertPlayer(Team *team, PlayerNode *newPlayer)
{
    int position = newPlayer->role - 1;
    // This gives the actual address of the pointer as we want to modify it
    PlayerNode **head = &team->players[position], *current;
    if (*head == NULL || newPlayer->performanceIndex > (*head)->performanceIndex) {
        newPlayer->next = *head;
        *head = newPlayer;
    } else {
        current = *head;
        while (current->next != NULL && current->next->performanceIndex >= newPlayer->performanceIndex)
            current = current->next;
        newPlayer->next = current->next;
        current->next = newPlayer;
    }
    team->totalPlayers++;
    if (newPlayer->role == BATSMAN || newPlayer->role == ALL_ROUNDER) {
        team->strikeRateSum += newPlayer->strikeRate;
        team->battingPlayers++;
    }
}

int playerIdAlreadyUsed(int id)
{
    int i, j;
    PlayerNode *current;
    for (i = 0; i < totalTeams; i++)
        for (j = 0; j < 3; j++)
            for (current = teamsList[i].players[j]; current != NULL; current = current->next)
                if (current->id == id) return 1;
    return 0;
}

void loadPlayers()
{
    int i, loaded = 0;
    for (i = 0; i < playerCount; i++) {
        Team *team = findTeamByName(players[i].team);
        PlayerNode *newPlayer;
        int role = getRole(players[i].role);
        if (team == NULL || role == 0) continue;
        if (team->totalPlayers >= MAX_PLAYERS) {
            printf("Player limit reached for %s. Some records were skipped.\n", team->name);
            continue;
        }
        newPlayer = createPlayer(players[i].id, players[i].name, players[i].team, role, players[i].totalRuns, players[i].battingAverage, players[i].strikeRate, players[i].wickets, players[i].economyRate);
        if (newPlayer != NULL) { insertPlayer(team, newPlayer); loaded++; }
    }
    printf("%d initial players loaded successfully.\n", loaded);
}

void printPlayerHeading()
{
    printf("\n%-6s %-24s %-13s %8s %8s %8s %7s %7s %12s\n", "ID", "Name", "Role", "Runs", "Average", "SR", "Wickets", "Economy", "Index");
    printf("---------------------------------------------------------------------------------------------------------\n");
}

void printPlayer(const PlayerNode *player)
{
    printf("%d %s %s %d %.2f %.2f %d %.2f %.2f\n", player->id, player->name, roleToString(player->role), player->runs, player->average, player->strikeRate, player->wickets, player->economyRate, player->performanceIndex);
}

//Display all players of a selected team. 
void displayTeamPlayers()
{
    int id, i;
    Team *team;
    PlayerNode *current;
    id = readInt("Enter Team ID: ", 1, totalTeams);
    team = findTeamById(id);
    if (team == NULL) { printf("Team not found.\n"); return; }
    printf("\nPlayers of %s\n", team->name);
    printPlayerHeading();
    for (i = 0; i < 3; i++)
        for (current = team->players[i]; current != NULL; current = current->next)
            printPlayer(current);
    printf("\nTotal players: %d\n", team->totalPlayers);
    printf("Average batting strike rate: %.2f\n", getAverageStrikeRate(team));
}
// Comparing the teams on the basis of Average strike rate (if firstTeam has less strike rate then 1 will be  returned else -1 will be returned)
int compareTeams(const void *a, const void *b)
{
    Team *first = *(Team **)a, *second = *(Team **)b;
    float firstAverage = getAverageStrikeRate(first), secondAverage = getAverageStrikeRate(second);
    if (firstAverage < secondAverage) return 1;
    if (firstAverage > secondAverage) return -1;
    return 0;
}

//Display teams in descending order of average batting strike rate
void displayTeamsByStrikeRate()
{
    Team *sortedTeams[MAX_TEAMS];
    int i;
    for (i = 0; i < totalTeams; i++) sortedTeams[i] = &teamsList[i];
    qsort(sortedTeams, totalTeams, sizeof(Team *), compareTeams);
    printf("\nTeams sorted by average batting strike rate\n");
    printf("%-8s %-22s %-15s %-15s\n", "Team ID", "Team Name", "Average SR", "Total Players");
    printf("--------------------------------------------------------------\n");
    for (i = 0; i < totalTeams; i++)
        printf("%-8d %-22s %-15.2f %-15d\n", sortedTeams[i]->id, sortedTeams[i]->name, getAverageStrikeRate(sortedTeams[i]), sortedTeams[i]->totalPlayers);
}

// Show the first K players from an already sorted list
void displayTopKPlayers()
{
    int id, role, k, count = 0;
    Team *team;
    PlayerNode *current;
    id = readInt("Enter Team ID: ", 1, totalTeams);
    team = findTeamById(id);
    if (team == NULL) { printf("Team not found.\n"); return; }
    role = readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    k = readInt("Enter K: ", 1, MAX_PLAYERS);
    current = team->players[role - 1];
    printf("\nTop %d %s players of %s\n", k, roleToString(role), team->name);
    printPlayerHeading();
    while (current != NULL && count < k) {
        printPlayer(current);
        current = current->next;
        count++;
    }
    if (count == 0) printf("No players found for this role.\n");
    else if (count < k) printf("Only %d player(s) are available.\n", count);
}
// Returning 1-0 on the basis of which player has higher performance index
int hasHigherIndex(PlayerNode *first, PlayerNode *second)
{
    return first->performanceIndex > second->performanceIndex;
}
// The heap is built for showing players of all teams of specific role according to their performance index in descending order in O(NlogN) time complexity
void pushHeap(HeapItem heap[], int *size, PlayerNode *player)
{
    int child;
    (*size)++;
    child = *size;
    heap[child].player = player;
    while (child > 1 && hasHigherIndex(heap[child].player, heap[child / 2].player)) {
        HeapItem temp = heap[child];
        heap[child] = heap[child / 2];
        heap[child / 2] = temp;
        child /= 2;
    }
}

PlayerNode *popHeap(HeapItem heap[], int *size)
{
    PlayerNode *answer = heap[1].player;
    int parent = 1;
    heap[1] = heap[*size];
    (*size)--;
    while (1) {
        int left = parent * 2, right = left + 1, largest = parent;
        if (left <= *size && hasHigherIndex(heap[left].player, heap[largest].player)) largest = left;
        if (right <= *size && hasHigherIndex(heap[right].player, heap[largest].player)) largest = right;
        if (largest == parent) break;
        {
            HeapItem temp = heap[parent];
            heap[parent] = heap[largest];
            heap[largest] = temp;
        }
        parent = largest;
    }
    return answer;
}

/*
   Take the first player from every team's selected role list.
   After printing one player, add the next player from that same list.
*/
void displayPlayersByRole()
{
    int role, i, heapSize = 0;
    HeapItem heap[MAX_TEAMS + 1];
    role = readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    for (i = 0; i < totalTeams; i++)
        if (teamsList[i].players[role - 1] != NULL)
            pushHeap(heap, &heapSize, teamsList[i].players[role - 1]);
    printf("\nAll %s players sorted by Performance Index\n", roleToString(role));
    printf("%s %s %s %s %s %s %s %s %s %s\n", "ID", "Name", "Team", "Role", "Runs", "Average", "SR", "Wickets", "Economy", "Index");
    printf("--------------------------------------------------------------------------------------------------------------------------------\n");
    while (heapSize > 0) {
        PlayerNode *player = popHeap(heap, &heapSize);
        printf("%d %s %s %s %d %f %f %d %f %f\n", player->id, player->name, player->teamName, roleToString(player->role), player->runs, player->average, player->strikeRate, player->wickets, player->economyRate, player->performanceIndex);
        if (player->next != NULL) pushHeap(heap, &heapSize, player->next);
    }
}
// Function for adding the player 
void addPlayer()
{
    int teamId, id, role, runs, wickets;
    float average, strikeRate, economyRate;
    char name[MAX_NAME + 1];
    Team *team;
    PlayerNode *newPlayer;
    teamId = readInt("Enter Team ID: ", 1, totalTeams);
    team = findTeamById(teamId);
    if (team == NULL) { printf("Team not found.\n"); return; }
    if (team->totalPlayers >= MAX_PLAYERS) { printf("This team already has the maximum number of players.\n"); return; }
    id = readInt("Enter Player ID (1-1000): ", 1, 1000);
    if (playerIdAlreadyUsed(id)) { printf("This Player ID is already in use.\n"); return; }
    readName("Enter player name: ", name);
    role = readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    runs = readInt("Total runs: ", 0, 100000);
    average = readFloat("Batting average: ", 0, 1000);
    strikeRate = readFloat("Strike rate: ", 0, 1000);
    wickets = readInt("Wickets: ", 0, 1000);
    economyRate = readFloat("Economy rate: ", 0, 100);
    newPlayer = createPlayer(id, name, team->name, role, runs, average, strikeRate, wickets, economyRate);
    if (newPlayer == NULL) return;
    insertPlayer(team, newPlayer);
    printf("Player added successfully.\n");
}
// Function for freeing up the memory
void freeMemory()
{
    int i, j;
    for (i = 0; i < totalTeams; i++)
        for (j = 0; j < 3; j++) {
            PlayerNode *current = teamsList[i].players[j];
            while (current != NULL) {
                PlayerNode *temp = current;
                current = current->next;
                free(temp);
            }
            teamsList[i].players[j] = NULL;
        }
}

int main()
{
    int choice;
    initializeTeams();
    loadPlayers();
    do {
        printf("\n===============================================\n");
        printf("       ICC ODI PLAYER PERFORMANCE ANALYZER\n");
        printf("===============================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Top K Players of a Team by Role\n");
        printf("5. Display All Players of a Selected Role\n");
        printf("6. Exit\n");
        printf("===============================================\n");
        choice = readInt("Enter your choice: ", 1, 6);
        switch (choice) {
            case 1: addPlayer(); break;
            case 2: displayTeamPlayers(); break;
            case 3: displayTeamsByStrikeRate(); break;
            case 4: displayTopKPlayers(); break;
            case 5: displayPlayersByRole(); break;
            case 6: freeMemory(); printf("Memory released. Exiting program.\n"); break;
        }
    } while (choice != 6);
    return 0;
}
