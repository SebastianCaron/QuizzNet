import time
import subprocess
import os
import signal
from typing import Optional

class ServerMaker:    
    def __init__(self, server_path: str = "./server", server_name: str = "les_12_coups", tcp_port: str = "9504"):
        self.server_path = server_path.rstrip('/')
        self.server_name = server_name
        self.tcp_port = tcp_port
        self.server_process: Optional[subprocess.Popen] = None
        self.udp_port = 5555
    
    def start_server(self) -> bool:
        try:
            server_binary = os.path.join(self.server_path, "server")
            
            print(f"Compilation du serveur...")
            result = subprocess.run(["make", "aspi", "all"], cwd=f"{self.server_path}", capture_output=True, text=True)
            if result.returncode != 0:
                print(f"Erreur de compilation: {result.stderr}")
                return False
            
            if not os.path.exists(server_binary):
                print(f"Erreur: le binaire {server_binary} n'existe pas après compilation")
                return False
            
            self.server_process = subprocess.Popen(
                ['./server', self.server_name, self.tcp_port],
                cwd=self.server_path,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                preexec_fn=os.setsid
            )
            
            time.sleep(2)
            
            if self.server_process.poll() is not None:
                stdout, stderr = self.server_process.communicate()
                print(f"Le serveur s'est arrêté immédiatement. stdout: {stdout}, stderr: {stderr}")
                return False
            
            print(f"Serveur démarré avec PID: {self.server_process.pid}")
            return True
            
        except Exception as e:
            print(f"Erreur lors du démarrage du serveur: {e}")
            return False
    
    def stop_server(self):
        if self.server_process:
            try:
                os.killpg(os.getpgid(self.server_process.pid), signal.SIGTERM)
                self.server_process.wait(timeout=5)
                print("Serveur arrêté proprement")
            except subprocess.TimeoutExpired:
                os.killpg(os.getpgid(self.server_process.pid), signal.SIGKILL)
                print("Serveur forcé à s'arrêter")
            except Exception as e:
                print(f"Erreur lors de l'arrêt du serveur: {e}")
            finally:
                self.server_process = None