import os

def find_bad_byte():
    print("--- Recherche de l'octet 0xA0 (Latin-1 NBSP) ---")
    found = False
    
    # On parcourt tous les dossiers et fichiers
    for root, dirs, files in os.walk("."):
        if "__pycache__" in root or ".git" in root:
            continue # On ignore les dossiers poubelles

        for file in files:
            # On cherche dans les .py mais aussi les .json ou .txt si tu en as
            if file.endswith((".py", ".json", ".txt")):
                path = os.path.join(root, file)
                
                try:
                    # IMPORTANT : On ouvre en mode BINAIRE ('rb')
                    with open(path, 'rb') as f:
                        content = f.read()
                        
                    # On cherche l'octet exact b'\xa0'
                    if b'\xa0' in content:
                        print(f"⚠️TROUVÉ DANS : {path}")
                        
                        # On essaie de trouver la ligne (approximatif en binaire)
                        lines = content.split(b'\n')
                        for i, line in enumerate(lines):
                            if b'\xa0' in line:
                                print(f"   -> Ligne {i+1} (approx): {line}")
                        found = True

                except Exception as e:
                    print(f"Erreur lecture {path}: {e}")

    if not found:
        print("--- Aucun octet 0xA0 trouvé. ---")
        print("Si l'erreur persiste, vérifie tes encodages de fichiers (Save as UTF-8).")

if __name__ == "__main__":
    find_bad_byte()