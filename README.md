# Documentation du Euterpe FileSystem (EFS)

## Table des matières

- [Introduction](#introduction)
  - [Principes Fondamentaux](#principes-fondamentaux)
- [Tools](#tools)
  - [Compilation](#compilation)
    - [Windows](#windows)
    - [Linux](#linux)
  - [Utilisation](#utilisation)
- [En-tête (Header)](#en-tête-header)
- [Entrées (Entries)](#entrées-entries)
- [Permissions](#permissions)
- [Exemples](#exemples)
  - [Exemple en C](#exemple-en-c)
  - [Exemple en C++](#exemple-en-cpp)
  - [Exemple en C#](#exemple-en-csharp)
  - [Exemple en Rust](#exemple-en-rust)
  - [Exemple en Java](#exemple-en-java)
- [Footeur](#footeur)

## Introduction

Le Euterpe FileSystem (EFS) est un système de fichiers axé sur la facilité d'utilisation et la simplicité. Il offre une structure de stockage de données organisée et efficace, adaptée à la gestion des fichiers et des répertoires. Cette documentation fournit des détails sur la structure du EFS, notamment les en-têtes (headers), les entrées (entries), les offsets et les autorisations (permissions).

### Principes Fondamentaux

Avant de plonger dans les détails techniques, comprenons les principes fondamentaux qui guident la conception du EFS.

## Tools

### Compilation

#### Windows

1) Télécharge [W64DevKit](https://github.com/skeeto/w64devkit/releases/tag/v1.21.0)
2) Extrait le dans un dossier et ajoute Path\To\w64devkit\bin dans les variables d'environnement
3) Lance un invité de commande et écrit ```sh gcc efs.c -o efs.exe```

#### Linux

```sh
gcc efs.c -o efs
```

### Utilisation

```sh
Usage: ./efs [-h] [-d {disk_filename}] [-t {type}]
Options:
  -d  Specify disk filename
  -t  Specify type (e.g., mbr, gpt)
  -h  Display this help message
```

## En-tête (Header)

Chaque système de fichiers Euterpe commence par un en-tête spécifique qui contient des informations essentielles pour la gestion des données. L'en-tête est structuré comme suit :

- `magic` (0-2): Trois caractères identifiant le système de fichiers (EFS).
- `size_block` (3-6): Taille d'un bloc de données.
- `max_block` (7-10): Nombre maximum de blocs de données.
- `free_block` (11-14): Nombre de blocs de données disponibles.
- `version` (15-15): Numéro de version du système de fichiers (actuellement 1).

## Entrées (Entries)

Chaque fichier ou répertoire dans le système de fichiers Euterpe est représenté par une entrée. Les entrées sont organisées en une liste, chaque entrée occupant une plage spécifique dans le système de fichiers. Chaque entrée a la même structure que décrite ci-dessous :

- `id` (0-3): Identifiant unique de l'entrée (0 pour le répertoire racine).
- `parent_id` (4-7): Identifiant du parent de l'entrée (0 pour le répertoire racine).
- `name` (8-263): Nom de l'entrée (maximum 256 caractères).
- `type` (264-267): Type de l'entrée (0 pour dossier, 1 pour fichier, 2 pour exécutable, 3 pour bibliothèque).
- `permissions` (268-271): Autorisations définissant les droits d'accès (voir la section sur les permissions).
- `creation_time` (272-279): Horodatage de la création de l'entrée.
- `last_modification_time` (280-287): Horodatage de la dernière modification de l'entrée.
- `is_delete` (288-288): Indicateur binaire (0 pour non supprimé, 1 pour supprimé).
- `min_block` (289-292): Pointeur vers le premier bloc de données associé à l'entrée.
- `max_block` (293-296): Pointeur vers le dernier bloc de données associé à l'entrée.

Les offsets indiquent les positions des différents champs dans une entrée ou un en-tête. Ils sont exprimés en octets à partir du début de la structure.

## Permissions

Les autorisations définissent les droits d'accès pour chaque entrée. Les valeurs possibles sont :

- `7` (rwx): Lecture, écriture et exécution.
- `6` (rw-): Lecture et écriture.
- `5` (r-x): Lecture et exécution.
- `4` (r--): Lecture seule.
- `3` (-wx): Écriture et exécution.
- `2` (-w-): Écriture seule.
- `1` (--x): Exécution seule.
- `0` (---): Aucune permission.

Les autorisations sont définies en utilisant des combinaisons de ces valeurs, par exemple, `rw-` indique lecture et écriture, `r-x` indique lecture et exécution, etc.

## Exemples

Voici quelques Exemples de structures pour EFS

### Exemple en C

Voici des exemples de structures en C correspondant à la documentation :

```c

// En-tête (Header)
struct EFSHeader {
    char magic[3];       // Trois caractères identifiant le système de fichiers (EFS)
    uint32_t size_bloc;   // Taille d'un bloc de données
    uint32_t max_block;   // Nombre maximum de blocs de données
    uint32_t free_block;  // Nombre de blocs de données disponibles
    uint8_t version;      // Numéro de version du système de fichiers (actuellement 1)
};

// Entrée (Entry)
struct EFSEntry {
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
};

// Permissions
enum EFSPermissions {
    EFS_PERMISSIONS_NONE = 0,
    EFS_PERMISSIONS_EXECUTE = 1,
    EFS_PERMISSIONS_WRITE = 2,
    EFS_PERMISSIONS_READ = 4
};
```

Ces exemples de structures en C peuvent être adaptés en fonction des besoins spécifiques de votre implémentation.

### Exemple en CPP

```cpp
// En-tête (Header)
struct EFSHeader {
    char magic[3];      // Trois caractères identifiant le système de fichiers (EFS)
    uint32_t sizeBloc;   // Taille d'un bloc de données
    uint32_t maxBlock;   // Nombre maximum de blocs de données
    uint32_t freeBlock;  // Nombre de blocs de données disponibles
    uint8_t version;     // Numéro de version du système de fichiers (actuellement 1)
};

// Entrée (Entry)
struct EFSEntry {
    uint32_t id;                     // Identifiant unique de l'entrée (0 pour le répertoire racine)
    uint32_t parentId;               // Identifiant du parent de l'entrée (0 pour le répertoire racine)
    std::string name;                // Nom de l'entrée
    uint32_t type;                   // Type de l'entrée (0 pour dossier, 1 pour fichier, 2 pour exécutable, 3 pour bibliothèque)
    uint32_t permissions;            // Autorisations définissant les droits d'accès
    uint64_t creationTime;           // Horodatage de la création de l'entrée
    uint64_t lastModificationTime;   // Horodatage de la dernière modification de l'entrée
    bool isDelete;                   // Indicateur binaire (false pour non supprimé, true pour supprimé)
    uint32_t minBlock;               // Pointeur vers le premier bloc de données associé à l'entrée
    uint32_t maxBlock;               // Pointeur vers le dernier bloc de données associé à l'entrée
};

// Permissions
enum class EFSPermissions : uint32_t {
    NONE = 0,
    EXECUTE = 1,
    WRITE = 2,
    READ = 4
};
```

Ces exemples de structures en C++ peuvent être adaptés en fonction des besoins spécifiques de votre implémentation.

### Exemple en Csharp

```cs
// En-tête (Header)
class EFSHeader
{
    public char[] Magic { get; set; }      // Trois caractères identifiant le système de fichiers (EFS)
    public int SizeBloc { get; set; }      // Taille d'un bloc de données
    public int MaxBlock { get; set; }      // Nombre maximum de blocs de données
    public int FreeBlock { get; set; }     // Nombre de blocs de données disponibles
    public byte Version { get; set; }      // Numéro de version du système de fichiers (actuellement 1)
}

// Entrée (Entry)
class EFSEntry
{
    public int Id { get; set; }                           // Identifiant unique de l'entrée (0 pour le répertoire racine)
    public int ParentId { get; set; }                     // Identifiant du parent de l'entrée (0 pour le répertoire racine)
    public string Name { get; set; }                      // Nom de l'entrée
    public int Type { get; set; }                         // Type de l'entrée (0 pour dossier, 1 pour fichier, 2 pour exécutable, 3 pour bibliothèque)
    public int Permissions { get; set; }                  // Autorisations définissant les droits d'accès
    public DateTime CreationTime { get; set; }            // Horodatage de la création de l'entrée
    public DateTime LastModificationTime { get; set; }    // Horodatage de la dernière modification de l'entrée
    public bool IsDelete { get; set; }                    // Indicateur binaire (false pour non supprimé, true pour supprimé)
    public int MinBlock { get; set; }                     // Pointeur vers le premier bloc de données associé à l'entrée
    public int MaxBlock { get; set; }                     // Pointeur vers le dernier bloc de données associé à l'entrée
}

// Permissions
enum EFSPermissions
{
    NONE = 0,
    EXECUTE = 1,
    WRITE = 2,
    READ = 4
}
```

Ces exemples de structures en C# peuvent être adaptés en fonction des besoins spécifiques de votre implémentation.

### Exemple en Rust

```rust
// En-tête (Header)
struct EFSHeader {
    magic: [char; 3],      // Trois caractères identifiant le système de fichiers (EFS)
    size_bloc: u32,        // Taille d'un bloc de données
    max_block: u32,        // Nombre maximum de blocs de données
    free_block: u32,       // Nombre de blocs de données disponibles
    version: u8,           // Numéro de version du système de fichiers (actuellement 1)
}

// Entrée (Entry)
struct EFSEntry {
    id: u32,                          // Identifiant unique de l'entrée (0 pour le répertoire racine)
    parent_id: u32,                   // Identifiant du parent de l'entrée (0 pour le répertoire racine)
    name: String,                     // Nom de l'entrée
    r#type: u32,                      // Type de l'entrée (0 pour dossier, 1 pour fichier, 2 pour exécutable, 3 pour bibliothèque)
    permissions: u32,                 // Autorisations définissant les droits d'accès
    creation_time: SystemTime,        // Horodatage de la création de l'entrée
    last_modification_time: SystemTime, // Horodatage de la dernière modification de l'entrée
    is_delete: bool,                  // Indicateur binaire (false pour non supprimé, true pour supprimé)
    min_block: u32,                   // Pointeur vers le premier bloc de données associé à l'entrée
    max_block: u32,                   // Pointeur vers le dernier bloc de données associé à l'entrée
}

// Permissions
enum EFSPermissions {
    NONE = 0,
    EXECUTE = 1,
    WRITE = 2,
    READ = 4
}
```

Ces exemples de structures en Rust peuvent être adaptés en fonction des besoins spécifiques de votre implémentation.

### Exemple en Java

```java
// En-tête (Header)
class EFSHeader {
    char[] magic;       // Trois caractères identifiant le système de fichiers (EFS)
    int sizeBloc;       // Taille d'un bloc de données
    int maxBlock;       // Nombre maximum de blocs de données
    int freeBlock;      // Nombre de blocs de données disponibles
    byte version;       // Numéro de version du système de fichiers (actuellement 1)
}

// Entrée (Entry)
class EFSEntry {
    int id;                           // Identifiant unique de l'entrée (0 pour le répertoire racine)
    int parentId;                     // Identifiant du parent de l'entrée (0 pour le répertoire racine)
    String name;                      // Nom de l'entrée
    int type;                         // Type de l'entrée (0 pour dossier, 1 pour fichier, 2 pour exécutable, 3 pour bibliothèque)
    int permissions;                  // Autorisations définissant les droits d'accès
    Date creationTime;                // Horodatage de la création de l'entrée
    Date lastModificationTime;        // Horodatage de la dernière modification de l'entrée
    boolean isDelete;                 // Indicateur binaire (false pour non supprimé, true pour supprimé)
    int minBlock;                     // Pointeur vers le premier bloc de données associé à l'entrée
    int maxBlock;                     // Pointeur vers le dernier bloc de données associé à l'entrée
}

// Permissions
enum EFSPermissions {
    NONE(0),
    EXECUTE(1),
    WRITE(2),
    READ(4);

    private final int value;

    EFSPermissions(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
```

Ces exemples de structures en Java peuvent être adaptés en fonction des besoins spécifiques de votre implémentation.

# Footeur

Cette documentation fournit une vue d'ensemble du Euterpe FileSystem et de ses composants. Pour une utilisation plus détaillée, référez-vous aux spécifications techniques du système de fichiers Euterpe.
