#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// En-tête (Header)
typedef struct EFSHeader {
    char magic[3];       // Trois caractères identifiant le système de fichiers (EFS)
    uint32_t size_bloc;   // Taille d'un bloc de données
    uint32_t max_block;   // Nombre maximum de blocs de données
    uint32_t free_block;  // Nombre de blocs de données disponibles
    uint8_t version;      // Numéro de version du système de fichiers (actuellement 1)
} EFSHeader;

// Entrée (Entry)
typedef struct EFSEntry {
    uint32_t id;                     // Identifiant unique de l'entrée (0 pour le répertoire racine)
    uint32_t parent_id;              // Identifiant du parent de l'entrée (0 pour le répertoire racine)
    char name[256];                  // Nom de l'entrée
    uint32_t type;                   // Type de l'entrée (0 pour dossier, 1 pour fichier, 2 pour exécutable, 3 pour bibliothèque)
    uint32_t permissions;            // Autorisations définissant les droits d'accès
    uint64_t creation_time;          // Horodatage de la création de l'entrée
    uint64_t last_modification_time; // Horodatage de la dernière modification de l'entrée
    uint8_t is_delete;               // Indicateur binaire (0 pour non supprimé, 1 pour supprimé)
    uint32_t min_block;              // Pointeur vers le premier bloc de données associé à l'entrée
    uint32_t max_block;              // Pointeur vers le dernier bloc de données associé à l'entrée
} EFSEntry;

// Permissions
enum EFSPermissions {
    EFS_PERMISSIONS_NONE = 0,
    EFS_PERMISSIONS_EXECUTE = 1,
    EFS_PERMISSIONS_WRITE = 2,
    EFS_PERMISSIONS_READ = 4
};

enum DiskType {
    DISK_TYPE_NONE = 0,
    DISK_TYPE_MBR = 1,
    DISK_TYPE_GPT = 2,
};

struct FileSystem {
    EFSHeader header;
    EFSEntry *entry;
}; 

typedef struct {
    char *name;
    enum DiskType type;
} disk_t;

disk_t disk;

// Fonction pour convertir une chaîne en majuscules
void upper(char *str);

void EFSHelp(const char *program_name);

int formatEFSDisk(disk_t *_disk);

int readEFSDisk(disk_t *_disk);

// Fonction pour lire l'en-tête SFS depuis un fichier
int readEFSHeader(const char *filename, struct FileSystem *fs);

// Fonction pour écrire l'en-tête SFS dans un fichier
int writeEFSHeader(const char *filename, const struct FileSystem *fs);

int main(int argc, char *argv[]) {

    int opt;
    char *disk_filename = NULL;
    char *type = NULL;
    char *isFormat = NULL;
    
    while ((opt = getopt(argc, argv, ":d:t:f:h")) != -1) {
        if (opt == 'h' || opt == 'H'){
            EFSHelp(argv[0]);
            exit(EXIT_SUCCESS);
        }
        switch (opt) {
        case 'd':
            disk_filename = optarg;
            break;
        case 't':
            type = optarg;
            break;
        case 'f':
            isFormat = optarg;
            break;
        default:
            break;
        }
    }

    if(disk_filename == NULL)
        disk.name = "efs.img";
    else
        disk.name = disk_filename;
    
    if(type == NULL)
        disk.type = DISK_TYPE_NONE;
    else{
        upper(type);
        disk.type = strcmp(type, "GPT") == 0 ? DISK_TYPE_GPT : strcmp(type, "MBR") == 0 ? DISK_TYPE_MBR : DISK_TYPE_NONE;
        printf("Type: %s\n", type);
    }
    
    if(isFormat == NULL){
        isFormat = "FALSE";
    } else {
        upper(isFormat);
    }
    //printf("%s %s be formated!\n", disk.name, strcmp(isFormat, "TRUE") == 0 ? "will" : "will not");

    int result;
    if(strcmp(isFormat, "TRUE") == 0){
        result = formatEFSDisk(&disk);
        if (result == 0) {
            printf("En-tête écrit avec succès.\n");
        } else {
            printf("Erreur pendant l'écriture de l'en-tête!\n");
            return result;
        }
    } else {
        result = readEFSDisk(&disk);
        if (result != 0) {
            printf("Erreur pendant la lecture de l'en-tête!\n");
            return result;
        }
    }

    return 0;
}

int formatEFSDisk(disk_t *_disk){
    struct FileSystem fs;
    fs.header = (EFSHeader){
        .version = 1,
        .magic = "EFS",
        .free_block = 100,
        .max_block = 100,
        .size_bloc = 255,
    };

    fs.entry = malloc(sizeof(EFSEntry) * fs.header.max_block); // Allocate memory for all items

    EFSEntry rootDir = {
        .id = 0,
        .parent_id = 0,
        .name = "root",
        .min_block = 0,
        .max_block = 5,
        .permissions = EFS_PERMISSIONS_READ + EFS_PERMISSIONS_WRITE,
        .is_delete = 0,
        .type = 0,
    };

    fs.entry[0] = rootDir; // Assign the root directory item

    int result = writeEFSHeader(_disk->name, &fs);
    
    free(fs.entry); // Free allocated memory
    return result;
}

int readEFSDisk(disk_t *_disk){
    struct FileSystem readFs;
    int result = readEFSHeader(_disk->name, &readFs);
    if (result == 0) {
        // Utilisez les valeurs de l'en-tête comme nécessaire
        printf("Magic: %s\n", readFs.header.magic);
        printf("Version: %u\n", readFs.header.version);
        printf("Block_Size: %u\n", readFs.header.size_bloc);
        printf("Free Blocks: %u\n", readFs.header.free_block);
        printf("Total Blocks: %u\n", readFs.header.max_block);

        // Access the root directory item
        EFSEntry rootItem = readFs.entry[0];
        printf("Root Directory Name: %s\n", rootItem.name);
        printf("Read Permission: %s\n", rootItem.permissions & EFS_PERMISSIONS_READ ? "yes" : "no");
        printf("Write Permission: %s\n", rootItem.permissions & EFS_PERMISSIONS_WRITE ? "yes" : "no");
        printf("Execute Permission: %s\n", rootItem.permissions & EFS_PERMISSIONS_EXECUTE ? "yes" : "no");
    }

    free(readFs.entry); // Free allocated memory
    return result;
}

void EFSHelp(const char *program_name){
    printf("Usage: %s [-h] [-d {disk_filename}] [-t {type}] [-f {true/false}]\n", program_name);
    printf("Options:\n");
    printf("  -d  Specify disk filename\n");
    printf("  -t  Specify type (e.g., mbr, gpt)\n");
    printf("  -h  Display this help message\n");
    printf("  -f  Format the disk (e.g., true, false)\n");
}

// Fonction pour écrire l'en-tête SFS dans un fichier
int writeEFSHeader(const char *filename, const struct FileSystem *fs) {
    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        return -1; // Échec de l'ouverture du fichier
    }

    // Écrire l'en-tête dans le fichier
    size_t bytesWritten = fwrite(&fs->header, sizeof(EFSHeader), 1, file);

    if (bytesWritten != 1) {
        fprintf(stderr, "Erreur d'écriture de l'en-tête dans le fichier.\n");
        fclose(file);
        return -1; // Échec de l'écriture de l'en-tête
    }

    // Écrire les éléments dans le fichier
    size_t itemsWritten = fwrite(fs->entry, sizeof(EFSEntry), fs->header.max_block, file);

    fclose(file);

    if (itemsWritten != fs->header.max_block) {
        fprintf(stderr, "Erreur d'écriture des éléments dans le fichier.\n");
        return -1; // Échec de l'écriture des éléments
    }

    return 0; // Succès
}


// Fonction pour lire l'en-tête SFS depuis un fichier
int readEFSHeader(const char *filename, struct FileSystem *fs) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        return -1; // Échec de l'ouverture du fichier
    }

    // Lire l'en-tête depuis le fichier
    size_t bytesRead = fread(&fs->header, sizeof(EFSHeader), 1, file);

    // Allouer de la mémoire pour les éléments
    fs->entry = malloc(sizeof(EFSEntry) * fs->header.max_block);

    // Lire les éléments depuis le fichier
    size_t itemsRead = fread(fs->entry, sizeof(EFSEntry), fs->header.max_block, file);

    fclose(file);

    if (bytesRead != 1 || itemsRead != fs->header.max_block) {
        fprintf(stderr, "Erreur de lecture de l'en-tête du fichier.\n");
        return -1; // Échec de la lecture de l'en-tête
    }

    return 0; // Succès
}

// Fonction pour convertir une chaîne en majuscules
void upper(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}