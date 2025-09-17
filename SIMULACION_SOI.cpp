#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

using namespace std;

// Estructura del PCB (Process Control Block)
struct Proceso {
    int pid;
    int llegada;
    int servicio;
    int inicio;
    int fin;
    int respuesta;
    int espera;
    int retorno;
    
    Proceso(int p, int ll, int s) : pid(p), llegada(ll), servicio(s), 
           inicio(-1), fin(-1), respuesta(-1), espera(-1), retorno(-1) {}
};

class SimuladorSO {
private:
    vector<Proceso> procesos;
    
public:
    SimuladorSO() {}
    
    // Ingresar datos por consola
    void ingresarDatos() {
        int num_procesos;
        cout << "=== SIMULADOR DE SISTEMA OPERATIVO ===" << endl;
        cout << "Ingrese el numero de procesos: ";
        cin >> num_procesos;
        
        procesos.clear();
        for(int i = 0; i < num_procesos; i++) {
            int pid, llegada, servicio;
            cout << "Proceso " << (i+1) << ":" << endl;
            cout << "  PID: ";
            cin >> pid;
            cout << "  Tiempo de llegada: ";
            cin >> llegada;
            cout << "  Tiempo de servicio: ";
            cin >> servicio;
            
            procesos.push_back(Proceso(pid, llegada, servicio));
        }
    }
    
    // Algoritmo FCFS (First Come First Served)
    void ejecutarFCFS() {
        cout << "\n=== EJECUTANDO FCFS ===" << endl;
        
        // Ordenar por tiempo de llegada
        sort(procesos.begin(), procesos.end(), 
             [](const Proceso& a, const Proceso& b) {
                 return a.llegada < b.llegada;
             });
        
        int tiempo_actual = 0;
        
        for(auto& proceso : procesos) {
            // El proceso inicia cuando llega o cuando termina el anterior
            proceso.inicio = max(tiempo_actual, proceso.llegada);
            proceso.fin = proceso.inicio + proceso.servicio;
            tiempo_actual = proceso.fin;
            
            // Calcular metricas
            proceso.respuesta = proceso.inicio - proceso.llegada;
            proceso.espera = proceso.fin - proceso.llegada - proceso.servicio;
            proceso.retorno = proceso.fin - proceso.llegada;
        }
        
        mostrarResultados("FCFS");
    }
    
    // Algoritmo SPN (Shortest Process Next)
    void ejecutarSPN() {
        cout << "\n=== EJECUTANDO SPN ===" << endl;
        
        vector<Proceso> cola_listos;
        vector<Proceso> resultados;
        int tiempo_actual = 0;
        int indice = 0;
        
        // Ordenar por tiempo de llegada
        sort(procesos.begin(), procesos.end(), 
             [](const Proceso& a, const Proceso& b) {
                 return a.llegada < b.llegada;
             });
        
        while(resultados.size() < procesos.size()) {
            // Agregar procesos que han llegado a la cola de listos
            while(indice < procesos.size() && procesos[indice].llegada <= tiempo_actual) {
                cola_listos.push_back(procesos[indice]);
                indice++;
            }
            
            if(cola_listos.empty()) {
                // No hay procesos listos, avanzar al proximo proceso que llega
                tiempo_actual = procesos[indice].llegada;
                continue;
            }
            
            // Seleccionar el proceso con menor tiempo de servicio
            auto it = min_element(cola_listos.begin(), cola_listos.end(),
                                 [](const Proceso& a, const Proceso& b) {
                                     if(a.servicio == b.servicio)
                                         return a.llegada < b.llegada;
                                     return a.servicio < b.servicio;
                                 });
            
            Proceso proceso_actual = *it;
            cola_listos.erase(it);
            
            // Ejecutar el proceso
            proceso_actual.inicio = tiempo_actual;
            proceso_actual.fin = tiempo_actual + proceso_actual.servicio;
            tiempo_actual = proceso_actual.fin;
            
            // Calcular metricas
            proceso_actual.respuesta = proceso_actual.inicio - proceso_actual.llegada;
            proceso_actual.espera = proceso_actual.fin - proceso_actual.llegada - proceso_actual.servicio;
            proceso_actual.retorno = proceso_actual.fin - proceso_actual.llegada;
            
            resultados.push_back(proceso_actual);
        }
        
        procesos = resultados;
        mostrarResultados("SPN");
    }
    
    void mostrarResultados(const string& algoritmo) {
        cout << "\n=== RESULTADOS " << algoritmo << " ===" << endl;
        cout << left << setw(5) << "PID" << "| " 
             << setw(8) << "Llegada" << "| "
             << setw(9) << "Servicio" << "| "
             << setw(7) << "Inicio" << "| "
             << setw(5) << "Fin" << "| "
             << setw(10) << "Respuesta" << "| "
             << setw(7) << "Espera" << "| "
             << setw(8) << "Retorno" << endl;
        
        cout << string(70, '-') << endl;
        
        double suma_respuesta = 0, suma_espera = 0, suma_retorno = 0;
        int tiempo_total = 0;
        
        for(const auto& proceso : procesos) {
            cout << left << setw(5) << proceso.pid << "| "
                 << setw(8) << proceso.llegada << "| "
                 << setw(9) << proceso.servicio << "| "
                 << setw(7) << proceso.inicio << "| "
                 << setw(5) << proceso.fin << "| "
                 << setw(10) << proceso.respuesta << "| "
                 << setw(7) << proceso.espera << "| "
                 << setw(8) << proceso.retorno << endl;
            
            suma_respuesta += proceso.respuesta;
            suma_espera += proceso.espera;
            suma_retorno += proceso.retorno;
            tiempo_total = max(tiempo_total, proceso.fin);
        }
        
        // Metricas globales
        int n = procesos.size();
        cout << "\n=== METRICAS GLOBALES ===" << endl;
        cout << fixed << setprecision(2);
        cout << "Tiempo de respuesta promedio: " << suma_respuesta / n << endl;
        cout << "Tiempo de espera promedio: " << suma_espera / n << endl;
        cout << "Tiempo de retorno promedio: " << suma_retorno / n << endl;
        cout << "Throughput: " << (double)n / tiempo_total << " procesos/unidad tiempo" << endl;
    }
    
    void ejecutar() {
        ingresarDatos();
        
        while(true) {
            cout << "\n=== MENU PRINCIPAL ===" << endl;
            cout << "1. Ejecutar FCFS" << endl;
            cout << "2. Ejecutar SPN" << endl;
            cout << "3. Salir" << endl;
            cout << "Seleccione una opcion: ";
            
            int opcion;
            cin >> opcion;
            
            switch(opcion) {
                case 1:
                    ejecutarFCFS();
                    break;
                case 2:
                    ejecutarSPN();
                    break;
                case 3:
                    cout << "¡Hasta luego!" << endl;
                    return;
                default:
                    cout << "Opcion invalida" << endl;
            }
        }
    }
};

int main() {
    SimuladorSO simulador;
    simulador.ejecutar();
    return 0;
}