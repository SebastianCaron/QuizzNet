# Quizz Net

> Lina LETHOOR & Sebastian CARON


## Lancer les tests unitaires

```bash
export PYTHONPATH=$PYTHONPATH:`pwd`
```

### Lancer des tests en local (le server doit être lancé)

```bash
python3 ./tests/run_tests.py --local
```

### Lancer des tests de A à Z (comme sur l'action github)

> ie: va effectuer la compilation, le lancement et l'arret.

```bash
python3 ./tests/run_tests.py
```