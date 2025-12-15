# Quizz Net

## Groupe A
- Lina LETHOOR
- Sebastian CARON

## Compiler et lancer le server

### Compiler

### Install openssl

```bash
sudo apt install libssl-dev
```

Dans le dossier `server` : 

```bash
make
```

Permet de compiler le serveur.

```bash
make fr0 
```

Recompile et lance le serveur sur le port 9504 avec le nom `les_12_coups`.

### Lancer le serveur

```bash
./server 'nom du serveur' port
```

- 'nom du serveur' correspond au nom du serveur
- port correpsond au port sur lequel écoutera les connexiosn TCP


Exemple :
```bash
./server 'les_12_coups' 9504
```

## Lancer les tests unitaires

```bash
export PYTHONPATH=$PYTHONPATH:`pwd`
python3 ./tests/run_tests.py
```

### Arguments :

- `--local` ne compile pas, ne lance pas, n'arrete pas le server. (utile si déjà lancé)

Tests Specifiques :
- `-udp` 
- `-invalid_ep`
- `-register`
- `-login`

Par défaut : lance tous les tests.

### Exemples : 

```bash
python3 ./tests/run_tests.py --local -login
``` 
Lance le test login.

```bash
python3 ./tests/run_tests.py -udp -register
```
Compile et démarre le serveur, lance les tests udp et register et arrête le server.

