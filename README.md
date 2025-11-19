# Quizz Net

## Groupe A
- Lina LETHOOR
- Sebastian CARON


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
