#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define PURPLE  "\033[1;35m"
#define RESET   "\033[0m"

struct Utilizator {
    char username[50];
    char parola[50];
    int logat;
};

struct Canal {
    char nume[100];
    float rating;
    int numar_voturi;
    int numar_abonati;
};

struct Utilizator utilizator_curent = {"", "", 0};

void curatare_ecran() {
    system("cls");
}

void pauza() {
    printf("\nApasati Enter pentru a continua...");
    while (getchar() != '\n');
    getchar();
}

void creare_fisiere() {
    FILE *f_utilizatori = fopen("utilizatori.txt", "a");
    if (f_utilizatori != NULL) {
        fclose(f_utilizatori);
    }

    FILE *f_canale = fopen("canale.txt", "a");
    if (f_canale != NULL) {
        fclose(f_canale);
    }

    FILE *f_abonamente = fopen("abonamente.txt", "a");
    if (f_abonamente != NULL) {
        fclose(f_abonamente);
    }

    // Adaugam fișierul pentru a stoca voturile individuale
    FILE *f_voturi = fopen("voturi.txt", "a");
    if (f_voturi != NULL) {
        fclose(f_voturi);
    }
}

// Validare nume utilizator
int validare_username(char* username) {
    if (strlen(username) < 3 || strlen(username) > 20) {
        printf(RED "\nNume utilizator invalid! Trebuie sa contina intre 3 si 20 caractere.\n" RESET);
        return 0;
    }

    // Verificam daca username-ul exista deja
    FILE *file = fopen("utilizatori.txt", "r");
    if (file != NULL) {
        char linie[100];
        char user_existent[50];
        char parola_temp[50];

        while (fgets(linie, sizeof(linie), file)) {
            sscanf(linie, "%[^;];%s", user_existent, parola_temp);
            if (strcmp(user_existent, username) == 0) {
                fclose(file);
                printf(RED "\nNume utilizator existent! Alegeti alt nume.\n" RESET);
                return 0;
            }
        }
        fclose(file);
    }

    return 1;
}

// Validare parola
int validare_parola(char* parola) {
    if (strlen(parola) < 4) {
        printf(RED "\nParola trebuie sa aiba minim 4 caractere!\n" RESET);
        return 0;
    }
    return 1;
}

// Validare rating
int validare_rating(int rating) {
    if (rating < 1 || rating > 5) {
        printf(RED "\nRating invalid! Trebuie sa fie intre 1 si 5.\n" RESET);
        return 0;
    }
    return 1;
}

// Verificare daca utilizatorul este logat
int este_logat() {
    if (utilizator_curent.logat == 0) {
        printf(RED "\nTrebuie sa fiti logat pentru a efectua aceasta actiune!\n" RESET);
        return 0;
    }
    return 1;
}

// Functii pentru gestionarea utilizatorilor
void inregistrare_utilizator(char* username, char* parola) {
    FILE *file = fopen("utilizatori.txt", "a");
    if (file == NULL) {
        printf(RED "Eroare la deschiderea fisierului pentru scriere.\n" RESET);
        return;
    }

    fprintf(file, "%s;%s\n", username, parola);
    fclose(file);

    printf(GREEN "\nUtilizator inregistrat cu succes!\n" RESET);
}

int logare_utilizator(char* username, char* parola) {
    FILE *file = fopen("utilizatori.txt", "r");
    if (file == NULL) {
        printf(RED "Eroare la deschiderea fisierului utilizatori.\n" RESET);
        return 0;
    }

    char linie[100];
    char user_temp[50];
    char parola_temp[50];

    while (fgets(linie, sizeof(linie), file)) {
        sscanf(linie, "%[^;];%s", user_temp, parola_temp);
        if (strcmp(user_temp, username) == 0 && strcmp(parola_temp, parola) == 0) {
            fclose(file);
            strcpy(utilizator_curent.username, username);
            strcpy(utilizator_curent.parola, parola);
            utilizator_curent.logat = 1;

            printf(GREEN "\nAutentificare reusita! Bine ati venit, " YELLOW "%s" RESET "!\n", username);
            return 1;
        }
    }

    fclose(file);
    printf(RED "\nCredentiale incorecte!\n" RESET);
    return 0;
}

void delogare_utilizator() {
    strcpy(utilizator_curent.username, "");
    strcpy(utilizator_curent.parola, "");
    utilizator_curent.logat = 0;

    printf(GREEN "Ati fost delogat cu succes.\n" RESET);
}

// Functii pentru gestionarea canalelor
int cauta_canal(char* nume_canal) {
    FILE *file = fopen("canale.txt", "r");
    if (file == NULL) {
        return 0;
    }

    char linie[200];
    int nr;
    char nume[100];
    float rating;
    int voturi;

    while (fgets(linie, sizeof(linie), file)) {
        if (sscanf(linie, "%d. %[^-] - Rating: %f/5 (Voturi: %d)", &nr, nume, &rating, &voturi) >= 3) {
            // Eliminare spatii de la final
            size_t len = strlen(nume);
            while (len > 0 && (nume[len - 1] == ' ' || nume[len - 1] == '\t')) {
                nume[len - 1] = '\0';
                len--;
            }

            if (strcmp(nume, nume_canal) == 0) {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

int este_abonat(char* nume_canal) {
    if (!este_logat()) {
        return 0;
    }

    FILE *file = fopen("abonamente.txt", "r");
    if (file == NULL) {
        return 0;
    }

    char linie[200];
    char username[50];
    char canal[100];

    while (fgets(linie, sizeof(linie), file)) {
        if (sscanf(linie, "%[^;];%[^\n]", username, canal) == 2) {
            if (strcmp(username, utilizator_curent.username) == 0 && strcmp(canal, nume_canal) == 0) {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

void abonare_canal(char* nume_canal) {
    if (!este_logat()) {
        return;
    }

    if (este_abonat(nume_canal)) {
        printf(YELLOW "Sunteti deja abonat la acest canal!\n" RESET);
        return;
    }

    // Verificam daca canalul exista
    if (!cauta_canal(nume_canal)) {
        printf(RED "Canalul %s nu exista in baza de date!\n" RESET, nume_canal);
        return;
    }

    FILE *file = fopen("abonamente.txt", "a");
    if (file == NULL) {
        printf(RED "Eroare la deschiderea fisierului abonamente.\n" RESET);
        return;
    }

    fprintf(file, "%s;%s\n", utilizator_curent.username, nume_canal);
    fclose(file);

    printf(GREEN "V-ati abonat cu succes la canalul " YELLOW "%s" RESET "!\n", nume_canal);
}

void dezabonare_canal(char* nume_canal) {
    if (!este_logat()) {
        return;
    }

    if (!este_abonat(nume_canal)) {
        printf(YELLOW "Nu sunteti abonat la acest canal!\n" RESET);
        return;
    }

    FILE *file = fopen("abonamente.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (file == NULL || temp == NULL) {
        printf(RED "Eroare la deschiderea fisierelor.\n" RESET);
        return;
    }

    char linie[200];
    char username[50];
    char canal[100];

    while (fgets(linie, sizeof(linie), file)) {
        if (sscanf(linie, "%[^;];%[^\n]", username, canal) == 2) {
            if (!(strcmp(username, utilizator_curent.username) == 0 && strcmp(canal, nume_canal) == 0)) {
                fprintf(temp, "%s", linie);
            }
        }
    }

    fclose(file);
    fclose(temp);

    remove("abonamente.txt");
    rename("temp.txt", "abonamente.txt");

    printf(GREEN "V-ati dezabonat cu succes de la canalul " YELLOW "%s" RESET "!\n", nume_canal);
}

// Verifica daca un utilizator a votat deja pentru un canal
int a_votat_deja(char* username, char* nume_canal) {
    FILE *file = fopen("voturi.txt", "r");
    if (file == NULL) {
        return 0;
    }

    char linie[200];
    char user_curent[50];
    char canal[100];
    int rating;

    while (fgets(linie, sizeof(linie), file)) {
        if (sscanf(linie, "%[^;];%[^;];%d", user_curent, canal, &rating) == 3) {
            if (strcmp(user_curent, username) == 0 && strcmp(canal, nume_canal) == 0) {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

// Inregistreaza un vot în fișierul de voturi
void inregistreaza_vot(char* username, char* nume_canal, int rating) {
    FILE *file;

    // Verificam daca utilizatorul a votat deja
    if (a_votat_deja(username, nume_canal)) {
        // Modificam votul existent
        FILE *original = fopen("voturi.txt", "r");
        FILE *temp = fopen("temp_voturi.txt", "w");

        if (original == NULL || temp == NULL) {
            printf(RED "Eroare la deschiderea fisierelor.\n" RESET);
            return;
        }

        char linie[200];
        char user_curent[50];
        char canal[100];
        int rating_vechi;

        while (fgets(linie, sizeof(linie), original)) {
            if (sscanf(linie, "%[^;];%[^;];%d", user_curent, canal, &rating_vechi) == 3) {
                if (strcmp(user_curent, username) == 0 && strcmp(canal, nume_canal) == 0) {
                    // Inlocuim cu noul rating
                    fprintf(temp, "%s;%s;%d\n", username, nume_canal, rating);
                } else {
                    // Pastram linia neschimbata
                    fprintf(temp, "%s", linie);
                }
            } else {
                fprintf(temp, "%s", linie);
            }
        }

        fclose(original);
        fclose(temp);

        remove("voturi.txt");
        rename("temp_voturi.txt", "voturi.txt");
    } else {
        // Adaugam un vot nou
        file = fopen("voturi.txt", "a");
        if (file == NULL) {
            printf(RED "Eroare la deschiderea fisierului voturi.\n" RESET);
            return;
        }

        fprintf(file, "%s;%s;%d\n", username, nume_canal, rating);
        fclose(file);
    }
}

// Calculeaza rating-ul unui canal pe baza tuturor voturilor
void actualizeaza_rating_canal(char* nume_canal) {
    FILE *file = fopen("voturi.txt", "r");
    if (file == NULL) {
        return;
    }

    float suma_rating = 0;
    int numar_voturi = 0;
    char linie[200];
    char username[50];
    char canal[100];
    int rating;

    // Calculam suma ratingurilor și numarul de voturi
    while (fgets(linie, sizeof(linie), file)) {
        if (sscanf(linie, "%[^;];%[^;];%d", username, canal, &rating) == 3) {
            if (strcmp(canal, nume_canal) == 0) {
                suma_rating += rating;
                numar_voturi++;
            }
        }
    }

    fclose(file);

    // Actualizam informatiile în fișierul canale.txt
    FILE *canale = fopen("canale.txt", "r");
    FILE *temp = fopen("temp_canale.txt", "w");

    if (canale == NULL || temp == NULL) {
        printf(RED "Eroare la deschiderea fisierelor.\n" RESET);
        return;
    }

    char linie_canal[200];
    int nr;
    char nume[100];
    float rating_vechi;
    int voturi_vechi;

    while (fgets(linie_canal, sizeof(linie_canal), canale)) {
        if (sscanf(linie_canal, "%d. %[^-] - Rating: %f/5 (Voturi: %d)", &nr, nume, &rating_vechi, &voturi_vechi) >= 3) {
            // Eliminare spatii de la final
            size_t len = strlen(nume);
            while (len > 0 && (nume[len - 1] == ' ' || nume[len - 1] == '\t')) {
                nume[len - 1] = '\0';
                len--;
            }

            if (strcmp(nume, nume_canal) == 0) {
                // Calculam media daca exista voturi
                float media = (numar_voturi > 0) ? suma_rating / numar_voturi : 0;
                fprintf(temp, "%d. %s - Rating: %.1f/5 (Voturi: %d)\n", nr, nume, media, numar_voturi);
            } else {
                fprintf(temp, "%s", linie_canal);
            }
        } else {
            fprintf(temp, "%s", linie_canal);
        }
    }

    fclose(canale);
    fclose(temp);

    remove("canale.txt");
    rename("temp_canale.txt", "canale.txt");
}

void asigneaza_rating(char* nume_canal, int rating) {
    if (!este_logat()) {
        return;
    }

    if (!este_abonat(nume_canal)) {
        printf(YELLOW "Trebuie sa fiti abonat la acest canal pentru a asigna un rating!\n" RESET);
        return;
    }

    // Verificam daca canalul exista
    if (!cauta_canal(nume_canal)) {
        printf(RED "Canalul %s nu exista in baza de date!\n" RESET, nume_canal);
        return;
    }

    // Inregistram votul utilizatorului
    inregistreaza_vot(utilizator_curent.username, nume_canal, rating);

    // Actualizam rating-ul canalului
    actualizeaza_rating_canal(nume_canal);

    printf(GREEN "Ati atribuit rating-ul " PURPLE "%d" RESET " canalului " YELLOW "%s" RESET "!\n", rating, nume_canal);
}

// Functii pentru meniuri
void meniu_inregistrare() {
    int opt;
    do {
        curatare_ecran();
        printf(BLUE "=== Meniu Inregistrare ===\n" RESET);
        printf("1. Introduceti datele pentru inregistrare\n");
        printf("2. Revenire la meniul principal\n");
        printf("Selectati o optiune: ");
        scanf("%d", &opt);
        getchar();

        if (opt == 1) {
            curatare_ecran();
            printf(BLUE "=== Inregistrare Utilizator ===\n" RESET);
            char username[50];
            char parola[50];

            do {
                printf("Nume utilizator (3-20 caractere): ");
                scanf("%s", username);
            } while (!validare_username(username));

            do {
                printf("Parola (minim 4 caractere): ");
                scanf("%s", parola);
            } while (!validare_parola(parola));

            inregistrare_utilizator(username, parola);
            pauza();
        }
    } while (opt != 2);

    curatare_ecran();
}

void meniu_conectare() {
    int opt;
    do {
        curatare_ecran();
        printf(BLUE "=== Meniu Logare ===\n" RESET);
        printf("1. Introduceti credentialele pentru logare\n");
        printf("2. Revenire la meniul principal\n");
        printf("Selectati o optiune: ");
        scanf("%d", &opt);
        getchar();

        if (opt == 1) {
            curatare_ecran();
            printf(BLUE "=== Credentiale Logare ===\n" RESET);
            char username[50];
            char parola[50];

            printf("Nume utilizator: ");
            scanf("%s", username);
            printf("Parola: ");
            scanf("%s", parola);

            logare_utilizator(username, parola);
            pauza();
        }
    } while (opt != 2);

    curatare_ecran();
}

void meniu_canal() {
    if (!este_logat()) {
        pauza();
        curatare_ecran();
        return;
    }

    int opt;
    do {
        curatare_ecran();
        printf(BLUE "=== Meniu Canale ===\n" RESET);
        printf("1. Cauta canal\n");
        printf("2. Aboneaza-te / Dezaboneaza-te\n");
        printf("3. Asigneaza rating canalului\n");
        printf("4. Revenire la meniul principal\n");
        printf("Selectati o optiune: ");
        scanf("%d", &opt);
        getchar();

        switch (opt) {
        case 1:
            curatare_ecran();
            printf(BLUE "=== Cautare canal ===\n" RESET);
            printf("Introduceti numele canalului pe care doriti sa-l cautati: ");
            char canal_cautat[50];
            scanf("%49s", canal_cautat);
            getchar();

            if (cauta_canal(canal_cautat)) {
                printf(GREEN "\nCanalul " YELLOW "%s" GREEN " a fost gasit!\n" RESET, canal_cautat);

                if (este_abonat(canal_cautat)) {
                    printf(BLUE "Sunteti abonat la acest canal.\n" RESET);

                    // Verificam daca utilizatorul a votat deja pentru acest canal
                    if (a_votat_deja(utilizator_curent.username, canal_cautat)) {
                        printf(PURPLE "Ati acordat deja un rating acestui canal.\n" RESET);
                    }
                } else {
                    printf("Nu sunteti abonat la acest canal.\n");
                }
            } else {
                printf(RED "\nCanalul " YELLOW "%s" RED " nu a fost gasit!\n" RESET, canal_cautat);
            }
            pauza();
            break;

        case 2:
            curatare_ecran();
            printf(BLUE "=== Abonare/Dezabonare Canal ===\n" RESET);
            printf("1. Abonare la canal\n");
            printf("2. Dezabonare de la canal\n");
            printf("Selectati o optiune: ");
            int sub_opt;
            scanf("%d", &sub_opt);
            getchar();

            if (sub_opt == 1 || sub_opt == 2) {
                printf("Introduceti numele canalului: ");
                char nume_canal[50];
                scanf("%49s", nume_canal);
                getchar();

                if (sub_opt == 1) {
                    abonare_canal(nume_canal);
                } else {
                    dezabonare_canal(nume_canal);
                }
            } else {
                printf(RED "Optiune invalida!\n" RESET);
            }
            pauza();
            break;

        case 3:
            curatare_ecran();
            printf(BLUE "=== Asignare Rating Canal ===\n" RESET);
            printf("Introduceti numele canalului: ");
            char nume_canal[50];
            scanf("%49s", nume_canal);
            getchar();

            int rating;
            do {
                printf("Introduceti un rating pentru canal (1-5): ");
                scanf("%d", &rating);
                getchar();
            } while (!validare_rating(rating));

            asigneaza_rating(nume_canal, rating);
            pauza();
            break;

        case 4:
            curatare_ecran();
            break;

        default:
            printf(RED "Optiune invalida! Reincercati.\n" RESET);
            pauza();
            break;
        }
    } while (opt != 4);

    curatare_ecran();
}

// Functie pentru compararea canalelor dupa rating (pentru qsort)
int compara_canale_rating(const void *a, const void *b) {
    const struct Canal *canalA = (const struct Canal *)a;
    const struct Canal *canalB = (const struct Canal *)b;

    // Comparare descrescatoare dupa rating
    if (canalB->rating > canalA->rating) return 1;
    if (canalB->rating < canalA->rating) return -1;

    // In caz de egalitate la rating, sortam dupa numarul de voturi (descrescator)
    return canalB->numar_voturi - canalA->numar_voturi;
}

void meniu_top25() {
    curatare_ecran();
    printf(BLUE "=== Top 25 Canale ===\n" RESET);

    // Citire canale din fisier
    FILE *file = fopen("canale.txt", "r");
    if (file == NULL) {
        printf(RED "Nu exista canale inregistrate!\n" RESET);
        pauza();
        curatare_ecran();
        return;
    }

    // Citire si sortare canale
    struct Canal canale[100];
    int numar_canale = 0;

    char linie[200];
    while (fgets(linie, sizeof(linie), file) && numar_canale < 100) {
        int nr;
        char nume[100];
        float rating;
        int voturi = 0;

        if (sscanf(linie, "%d. %[^-] - Rating: %f/5 (Voturi: %d)", &nr, nume, &rating, &voturi) >= 3) {
            // Eliminare spatii de la final
            size_t len = strlen(nume);
            while (len > 0 && (nume[len - 1] == ' ' || nume[len - 1] == '\t')) {
                nume[len - 1] = '\0';
                len--;
            }

            strcpy(canale[numar_canale].nume, nume);
            canale[numar_canale].rating = rating;
            canale[numar_canale].numar_voturi = voturi;
            numar_canale++;
        }
    }
    fclose(file);

    // Sortare canale dupa rating și numar de voturi
    qsort(canale, numar_canale, sizeof(struct Canal), compara_canale_rating);

    // Afisare top 25 canale
    int limita = (numar_canale < 25) ? numar_canale : 25;

    if (limita == 0) {
        printf(YELLOW "Nu exista canale inregistrate!\n" RESET);
    } else {
        for (int i = 0; i < limita; i++) {
            printf("%d. ", i + 1);
            printf(YELLOW "%s" RESET, canale[i].nume);
            printf(" | Rating: ");
            printf(PURPLE "%.1f/5" RESET, canale[i].rating);
            printf(" (" BLUE "%d voturi" RESET ")\n", canale[i].numar_voturi);
        }
    }

    pauza();
    curatare_ecran();
}

int main() {
    // Pentru a activa codurile ANSI în consola Windows
    system("color");

    // Creare fisiere necesare daca nu exista
    creare_fisiere();

    int option;
    do {
        printf(BLUE "=== Sistem de Streaming Online ===\n" RESET);

        // Afisare stare utilizator
        if (utilizator_curent.logat) {
            printf("Utilizator conectat: ");
            printf(YELLOW "%s\n\n" RESET, utilizator_curent.username);
        } else {
            printf("Niciun utilizator conectat\n\n");
        }

        printf("1. Inregistrare\n");
        printf("2. Logare\n");
        printf("3. Delogare\n");
        printf("4. Meniu Canale\n");
        printf("5. Afisare Top 25 Canale\n");
        printf("6. Iesire\n");
        printf("Selectati o optiune: ");
        scanf("%d", &option);
        getchar();

        switch(option) {
        case 1:
            meniu_inregistrare();
            break;
        case 2:
            meniu_conectare();
            break;
        case 3:
            curatare_ecran();
            if (utilizator_curent.logat) {
                delogare_utilizator();
            } else {
                printf(YELLOW "Nu sunteti conectat!\n" RESET);
            }
            pauza();
            curatare_ecran();
            break;
        case 4:
            meniu_canal();
            break;
        case 5:
            meniu_top25();
            break;
        case 6:
            curatare_ecran();
            printf(GREEN "La revedere!\n" RESET);
            break;
        default:
            curatare_ecran();
            printf(RED "Optiune invalida! Reincercati.\n" RESET);
            pauza();
            curatare_ecran();
            break;
        }
    } while(option != 6);

    return 0;
}
