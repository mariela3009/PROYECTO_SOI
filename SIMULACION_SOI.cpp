#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>
#include <queue>
#include <iomanip>

using namespace std;

struct Proceso {
    int pid;
    int llegada;
    int servicio;
    int inicio = -1;
    int fin = -1;
    int tiempoRespuesta = -1;
    int tiempoEspera = -1;
    int tiempoRetorno = -1;
    int tiempoRestante;
    bool iniciado = false;
};

struct SolicitudMemoria {
    int pid;
    int tam;
};

struct ConfiguracionCPU {
    string algoritmo;
    int quantum;
};

struct ConfiguracionMemoria {
    int tam;
    string estrategia;
};

// Funcion para limpiar el buffer de entrada
void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Funcion para convertir string a minusculas
string toLowerCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// Funcion para validar algoritmos de CPU
bool esAlgoritmoValido(const string& algoritmo) {
    string alg = toLowerCase(algoritmo);
    return (alg == "fcfs" || alg == "spn" || alg == "rr" || alg == "priority");
}

// Funcion para validar estrategias de memoria
bool esEstrategiaValida(const string& estrategia) {
    string est = toLowerCase(estrategia);
    return (est == "first-fit" || est == "best-fit" || est == "worst-fit");
}

// Funcion para mostrar el menu principal
void mostrarMenuPrincipal() {
    cout << "\n";
    cout << "=========================================\n";
    cout << "    SIMULADOR DE SISTEMA OPERATIVO\n";
    cout << "=========================================\n";
    cout << "1. Planificacion de CPU\n";
    cout << "2. Gestion de Memoria\n";
    cout << "3. Simulacion Completa\n";
    cout << "4. Salir\n";
    cout << "=========================================\n";
    cout << "Seleccione una opcion: ";
}

// Funcion para mostrar el menu de algoritmos de CPU
void mostrarMenuCPU() {
    cout << "\n";
    cout << "==========================\n";
    cout << "   ALGORITMOS DE CPU\n";
    cout << "==========================\n";
    cout << "1. FCFS (First Come First Served)\n";
    cout << "2. SPN (Shortest Process Next)\n";
    cout << "3. RR (Round Robin)\n";
    cout << "4. Priority\n";
    cout << "5. Volver al menu principal\n";
    cout << "==========================\n";
    cout << "Seleccione un algoritmo: ";
}

// Funcion para mostrar el menu de estrategias de memoria
void mostrarMenuMemoria() {
    cout << "\n";
    cout << "=============================\n";
    cout << "   ESTRATEGIAS DE MEMORIA\n";
    cout << "=============================\n";
    cout << "1. First-Fit\n";
    cout << "2. Best-Fit\n";
    cout << "3. Worst-Fit\n";
    cout << "4. Volver al menu principal\n";
    cout << "=============================\n";
    cout << "Seleccione una estrategia: ";
}

// Funcion para pausar y esperar entrada del usuario
void pausar() {
    cout << "\nPresione Enter para continuar...";
    cin.ignore();
    cin.get();
}

// Funcion para solicitar configuracion de CPU con menu
ConfiguracionCPU solicitarConfiguracionCPU() {
    ConfiguracionCPU config;
    int opcion;
    
    do {
        mostrarMenuCPU();
        cin >> opcion;
        limpiarBuffer();
        
        switch (opcion) {
            case 1:
                config.algoritmo = "fcfs";
                config.quantum = 0;
                cout << "Algoritmo FCFS seleccionado.\n";
                return config;
            case 2:
                config.algoritmo = "spn";
                config.quantum = 0;
                cout << "Algoritmo SPN seleccionado.\n";
                return config;
            case 3:
                config.algoritmo = "rr";
                do {
                    cout << "Ingrese el quantum para Round Robin: ";
                    cin >> config.quantum;
                    if (cin.fail() || config.quantum <= 0) {
                        cout << "Error: El quantum debe ser un numero entero positivo.\n";
                        limpiarBuffer();
                        config.quantum = -1;
                    }
                } while (config.quantum <= 0);
                limpiarBuffer();
                cout << "Algoritmo Round Robin seleccionado con quantum " << config.quantum << ".\n";
                return config;
            case 4:
                cout << "Algoritmo Priority aun no implementado.\n";
                break;
            case 5:
                config.algoritmo = "";
                return config;
            default:
                cout << "Opcion invalida. Intente de nuevo.\n";
        }
    } while (opcion != 5);
    
    return config;
}

// Funcion alternativa para solicitar configuracion CPU (estilo del segundo codigo)
ConfiguracionCPU solicitarConfiguracionCPUCompleta() {
    ConfiguracionCPU config;
    
    cout << "\n=== CONFIGURACION DE CPU ===\n";
    
    // Solicitar algoritmo
    do {
        cout << "Ingrese el algoritmo de planificacion (FCFS, SPN, RR, Priority): ";
        getline(cin, config.algoritmo);
        
        if (config.algoritmo.empty()) {
            cout << "Error: El algoritmo no puede estar vacio.\n";
            continue;
        }
        
        if (!esAlgoritmoValido(config.algoritmo)) {
            cout << "Error: Algoritmo no valido. Use: FCFS, SPN, RR, o Priority\n";
        }
    } while (!esAlgoritmoValido(config.algoritmo));
    
    config.algoritmo = toLowerCase(config.algoritmo);
    
    // Solicitar quantum si es Round Robin
    if (config.algoritmo == "rr") {
        do {
            cout << "Ingrese el quantum: ";
            cin >> config.quantum;
            
            if (cin.fail() || config.quantum <= 0) {
                cout << "Error: El quantum debe ser un numero entero positivo.\n";
                limpiarBuffer();
                config.quantum = -1;
            }
        } while (config.quantum <= 0);
    } else {
        config.quantum = 0; // No se usa para otros algoritmos
    }
    
    limpiarBuffer();
    return config;
}

// Funcion para solicitar procesos
vector<Proceso> solicitarProcesos() {
    vector<Proceso> procesos;
    int numProcesos;
    
    cout << "\n==========================\n";
    cout << "  CONFIGURACION DE PROCESOS\n";
    cout << "==========================\n";
    
    do {
        cout << "Ingrese el numero de procesos: ";
        cin >> numProcesos;
        
        if (cin.fail() || numProcesos <= 0) {
            cout << "Error: Debe ingresar un numero entero positivo.\n";
            limpiarBuffer();
            numProcesos = -1;
        }
    } while (numProcesos <= 0);
    
    for (int i = 0; i < numProcesos; i++) {
        Proceso proceso;
        
        cout << "\n--- Proceso " << (i + 1) << " ---\n";
        
        // PID
        do {
            cout << "PID: ";
            cin >> proceso.pid;
            
            if (cin.fail() || proceso.pid <= 0) {
                cout << "Error: El PID debe ser un numero entero positivo.\n";
                limpiarBuffer();
                proceso.pid = -1;
            } else {
                bool duplicado = false;
                for (const auto& p : procesos) {
                    if (p.pid == proceso.pid) {
                        duplicado = true;
                        break;
                    }
                }
                if (duplicado) {
                    cout << "Error: El PID " << proceso.pid << " ya existe.\n";
                    proceso.pid = -1;
                }
            }
        } while (proceso.pid <= 0);
        
        // Tiempo de llegada
        do {
            cout << "Tiempo de llegada: ";
            cin >> proceso.llegada;
            
            if (cin.fail() || proceso.llegada < 0) {
                cout << "Error: El tiempo de llegada debe ser un numero entero no negativo.\n";
                limpiarBuffer();
                proceso.llegada = -1;
            }
        } while (proceso.llegada < 0);
        
        // Tiempo de servicio
        do {
            cout << "Tiempo de servicio: ";
            cin >> proceso.servicio;
            
            if (cin.fail() || proceso.servicio <= 0) {
                cout << "Error: El tiempo de servicio debe ser un numero entero positivo.\n";
                limpiarBuffer();
                proceso.servicio = -1;
            }
        } while (proceso.servicio <= 0);
        
        procesos.push_back(proceso);
    }
    
    limpiarBuffer();
    return procesos;
}

// Funcion para solicitar configuracion de memoria
ConfiguracionMemoria solicitarConfiguracionMemoria() {
    ConfiguracionMemoria config;
    int opcion;
    
    do {
        cout << "\nIngrese el tamaño total de memoria (bytes): ";
        cin >> config.tam;
        
        if (cin.fail() || config.tam <= 0) {
            cout << "Error: El tamaño debe ser un numero entero positivo.\n";
            limpiarBuffer();
            config.tam = -1;
        }
    } while (config.tam <= 0);
    
    do {
        mostrarMenuMemoria();
        cin >> opcion;
        limpiarBuffer();
        
        switch (opcion) {
            case 1:
                config.estrategia = "first-fit";
                cout << "Estrategia First-Fit seleccionada.\n";
                return config;
            case 2:
                config.estrategia = "best-fit";
                cout << "Estrategia Best-Fit seleccionada.\n";
                return config;
            case 3:
                config.estrategia = "worst-fit";
                cout << "Estrategia Worst-Fit seleccionada.\n";
                return config;
            case 4:
                config.estrategia = "";
                return config;
            default:
                cout << "Opcion invalida. Intente de nuevo.\n";
        }
    } while (opcion != 4);
    
    return config;
}

// Funcion alternativa para solicitar configuracion de memoria (estilo del segundo codigo)
ConfiguracionMemoria solicitarConfiguracionMemoriaCompleta() {
    ConfiguracionMemoria config;
    
    cout << "\n=== CONFIGURACION DE MEMORIA ===\n";
    
    // Solicitar tamaño de memoria
    do {
        cout << "Ingrese el tamaño total de memoria (bytes): ";
        cin >> config.tam;
        
        if (cin.fail() || config.tam <= 0) {
            cout << "Error: El tamaño debe ser un numero entero positivo.\n";
            limpiarBuffer();
            config.tam = -1;
        }
    } while (config.tam <= 0);
    
    limpiarBuffer();
    
    // Solicitar estrategia de asignación
    do {
        cout << "Ingrese la estrategia de asignacion (first-fit, best-fit, worst-fit): ";
        getline(cin, config.estrategia);
        
        if (config.estrategia.empty()) {
            cout << "Error: La estrategia no puede estar vacia.\n";
            continue;
        }
        
        if (!esEstrategiaValida(config.estrategia)) {
            cout << "Error: Estrategia no valida. Use: first-fit, best-fit, o worst-fit\n";
        }
    } while (!esEstrategiaValida(config.estrategia));
    
    config.estrategia = toLowerCase(config.estrategia);
    
    return config;
}

// Funcion para solicitar solicitudes de memoria
vector<SolicitudMemoria> solicitarSolicitudesMemoria(const vector<Proceso>& procesos) {
    vector<SolicitudMemoria> solicitudes;
    int numSolicitudes;
    
    cout << "\n=============================\n";
    cout << "   SOLICITUDES DE MEMORIA\n";
    cout << "=============================\n";
    
    do {
        cout << "Ingrese el numero de solicitudes de memoria: ";
        cin >> numSolicitudes;
        
        if (cin.fail() || numSolicitudes < 0) {
            cout << "Error: Debe ingresar un numero entero no negativo.\n";
            limpiarBuffer();
            numSolicitudes = -1;
        }
    } while (numSolicitudes < 0);
    
    if (numSolicitudes == 0) {
        cout << "No se registraran solicitudes de memoria.\n";
        limpiarBuffer();
        return solicitudes;
    }
    
    for (int i = 0; i < numSolicitudes; i++) {
        SolicitudMemoria solicitud;
        
        cout << "\n--- Solicitud " << (i + 1) << " ---\n";
        
        // PID
        do {
            cout << "PID del proceso: ";
            cin >> solicitud.pid;
            
            if (cin.fail()) {
                cout << "Error: Debe ingresar un numero entero.\n";
                limpiarBuffer();
                solicitud.pid = -1;
                continue;
            }
            
            bool encontrado = false;
            for (const auto& p : procesos) {
                if (p.pid == solicitud.pid) {
                    encontrado = true;
                    break;
                }
            }
            
            if (!encontrado) {
                cout << "Error: El PID " << solicitud.pid << " no existe en los procesos definidos.\n";
                solicitud.pid = -1;
            }
        } while (solicitud.pid == -1);
        
        // Tamaño de memoria solicitado
        do {
            cout << "Tamaño solicitado (bytes): ";
            cin >> solicitud.tam;
            
            if (cin.fail() || solicitud.tam <= 0) {
                cout << "Error: El tamaño debe ser un numero entero positivo.\n";
                limpiarBuffer();
                solicitud.tam = -1;
            }
        } while (solicitud.tam <= 0);
        
        solicitudes.push_back(solicitud);
    }
    
    limpiarBuffer();
    return solicitudes;
}

// Funcion para simular Round Robin mejorada (del segundo codigo)
void simularRoundRobin(vector<Proceso>& procesos, int quantum) {
    int n = procesos.size();
    vector<int> tiempoRestante(n);
    
    // Crear mapeo de PID a índice
    vector<int> indicePorPID(1000, -1); // Asumiendo PIDs < 1000
    for (int i = 0; i < n; i++) {
        tiempoRestante[i] = procesos[i].servicio;
        procesos[i].tiempoRestante = procesos[i].servicio;
        indicePorPID[procesos[i].pid] = i;
    }
    
    queue<int> colaListos;
    vector<bool> enCola(n, false);
    int tiempoActual = 0;
    int procesosCompletados = 0;
    // Agregar procesos iniciales
    for (int i = 0; i < n; i++) {
        if (procesos[i].llegada <= tiempoActual) {
            colaListos.push(i);
            enCola[i] = true;
        }
    }
    
    while (procesosCompletados < n) {
        if (colaListos.empty()) {
            int siguienteLlegada = numeric_limits<int>::max();
            for (int i = 0; i < n; i++) {
                if (tiempoRestante[i] > 0 && procesos[i].llegada > tiempoActual) {
                    siguienteLlegada = min(siguienteLlegada, procesos[i].llegada);
                }
            }
            if (siguienteLlegada != numeric_limits<int>::max()) {
                tiempoActual = siguienteLlegada;
            }
            
            for (int i = 0; i < n; i++) {
                if (procesos[i].llegada <= tiempoActual && !enCola[i] && tiempoRestante[i] > 0) {
                    colaListos.push(i);
                    enCola[i] = true;
                }
            }
            continue;
        }
        
        int procesoActual = colaListos.front();
        colaListos.pop();
        enCola[procesoActual] = false;
        
        if (!procesos[procesoActual].iniciado) {
            procesos[procesoActual].inicio = tiempoActual;
            procesos[procesoActual].tiempoRespuesta = tiempoActual - procesos[procesoActual].llegada;
            procesos[procesoActual].iniciado = true;
        }
        
        int tiempoEjecucion = min(quantum, tiempoRestante[procesoActual]);
        tiempoRestante[procesoActual] -= tiempoEjecucion;
        int tiempoAnterior = tiempoActual;
        tiempoActual += tiempoEjecucion;
        
        for (int i = 0; i < n; i++) {
            if (procesos[i].llegada > tiempoAnterior && 
                procesos[i].llegada <= tiempoActual && 
                !enCola[i] && tiempoRestante[i] > 0 && i != procesoActual) {
                colaListos.push(i);
                enCola[i] = true;
            }
        }
        
        if (tiempoRestante[procesoActual] == 0) {
            procesos[procesoActual].fin = tiempoActual;
            procesos[procesoActual].tiempoRetorno = procesos[procesoActual].fin - procesos[procesoActual].llegada;
            procesos[procesoActual].tiempoEspera = procesos[procesoActual].tiempoRetorno - procesos[procesoActual].servicio;
            procesosCompletados++;
        } else {
            colaListos.push(procesoActual);
            enCola[procesoActual] = true;
        }
    }
}

// Funcion para simular FCFS
void simularFCFS(vector<Proceso>& procesos) {
    sort(procesos.begin(), procesos.end(), 
         [](const Proceso& a, const Proceso& b) {
             return a.llegada < b.llegada;
         });
    
    int tiempoActual = 0;
    
    for (auto& proceso : procesos) {
        if (tiempoActual < proceso.llegada) {
            tiempoActual = proceso.llegada;
        }
        
        proceso.inicio = tiempoActual;
        proceso.tiempoRespuesta = proceso.inicio - proceso.llegada;
        proceso.fin = proceso.inicio + proceso.servicio;
        proceso.tiempoRetorno = proceso.fin - proceso.llegada;
        proceso.tiempoEspera = proceso.tiempoRetorno - proceso.servicio;
        
        tiempoActual = proceso.fin;
    }
}

// Funcion para simular SPN (Shortest Process Next)
void simularSPN(vector<Proceso>& procesos) {
    int tiempoActual = 0;
    vector<bool> completado(procesos.size(), false);
    int procesosCompletados = 0;
    
    while (procesosCompletados < static_cast<int>(procesos.size())) {
        int indiceSeleccionado = -1;
        int menorTiempoServicio = numeric_limits<int>::max();
        
        for (size_t i = 0; i < procesos.size(); i++) {
            if (!completado[i] && procesos[i].llegada <= tiempoActual && 
                procesos[i].servicio < menorTiempoServicio) {
                menorTiempoServicio = procesos[i].servicio;
                indiceSeleccionado = i;
            }
        }
        
        if (indiceSeleccionado == -1) {
            int siguienteLlegada = numeric_limits<int>::max();
            for (size_t i = 0; i < procesos.size(); i++) {
                if (!completado[i] && procesos[i].llegada > tiempoActual) {
                    siguienteLlegada = min(siguienteLlegada, procesos[i].llegada);
                }
            }
            
            if (siguienteLlegada != numeric_limits<int>::max()) {
                tiempoActual = siguienteLlegada;
            } else {
                break;
            }
            continue;
        }
        
        Proceso& proceso = procesos[indiceSeleccionado];
        
        if (tiempoActual < proceso.llegada) {
            tiempoActual = proceso.llegada;
        }
        
        proceso.inicio = tiempoActual;
        proceso.tiempoRespuesta = proceso.inicio - proceso.llegada;
        proceso.fin = proceso.inicio + proceso.servicio;
        proceso.tiempoRetorno = proceso.fin - proceso.llegada;
        proceso.tiempoEspera = proceso.tiempoRetorno - proceso.servicio;
        
        tiempoActual = proceso.fin;
        completado[indiceSeleccionado] = true;
        procesosCompletados++;
    }
}

// Funcion para ejecutar la simulacion segun el algoritmo
void ejecutarSimulacion(vector<Proceso>& procesos, const ConfiguracionCPU& configCPU) {
    if (configCPU.algoritmo == "rr") {
        simularRoundRobin(procesos, configCPU.quantum);
    } else if (configCPU.algoritmo == "fcfs") {
        simularFCFS(procesos);
    } else if (configCPU.algoritmo == "spn") {
        simularSPN(procesos);
    } else {
        cout << "Algoritmo " << configCPU.algoritmo << " no implementado aun.\n";
        return;
    }
}

// Funcion para mostrar tabla de resultados
void mostrarTablaResultados(const vector<Proceso>& procesos) {
    cout << "\n=============================\n";
    cout << "      TABLA DE RESULTADOS\n";
    cout << "=============================\n";
    cout << "PID | Llegada | Servicio | Inicio | Fin | Respuesta | Espera | Retorno\n";
    cout << "----+---------+----------+--------+-----+-----------+--------+--------\n";
    
    vector<Proceso> procesosOrdenados = procesos;
    sort(procesosOrdenados.begin(), procesosOrdenados.end(),
         [](const Proceso& a, const Proceso& b) {
             return a.pid < b.pid;
         });
    
    for (const auto& proceso : procesosOrdenados) {
        cout << setw(3) << proceso.pid << " |"
             << setw(8) << proceso.llegada << " |"
             << setw(9) << proceso.servicio << " |"
             << setw(7) << proceso.inicio << " |"
             << setw(4) << proceso.fin << " |"
             << setw(10) << proceso.tiempoRespuesta << " |"
             << setw(7) << proceso.tiempoEspera << " |"
             << setw(7) << proceso.tiempoRetorno << endl;
    }
    
    double promedioRespuesta = 0, promedioEspera = 0, promedioRetorno = 0;
    for (const auto& proceso : procesos) {
        promedioRespuesta += proceso.tiempoRespuesta;
        promedioEspera += proceso.tiempoEspera;
        promedioRetorno += proceso.tiempoRetorno;
    }
    
    int numProcesos = procesos.size();
    promedioRespuesta /= numProcesos;
    promedioEspera /= numProcesos;
    promedioRetorno /= numProcesos;
    
    cout << "\n=============================\n";
    cout << "          PROMEDIOS\n";
    cout << "=============================\n";
    cout << "Tiempo de Respuesta Promedio: " << fixed << setprecision(2) << promedioRespuesta << endl;
    cout << "Tiempo de Espera Promedio: " << fixed << setprecision(2) << promedioEspera << endl;
    cout << "Tiempo de Retorno Promedio: " << fixed << setprecision(2) << promedioRetorno << endl;
}

// Funcion para manejar planificacion de CPU
void manejarPlanificacionCPU() {
    ConfiguracionCPU configCPU = solicitarConfiguracionCPU();
    
    if (configCPU.algoritmo.empty()) {
        return; // Usuario cancelo
    }
    
    vector<Proceso> procesos = solicitarProcesos();
    
    cout << "\nEjecutando simulacion de " << configCPU.algoritmo << "...\n";
    ejecutarSimulacion(procesos, configCPU);
    
    mostrarTablaResultados(procesos);
    pausar();
}

// Funcion para manejar gestion de memoria
void manejarGestionMemoria() {
    ConfiguracionMemoria configMemoria = solicitarConfiguracionMemoria();
    
    if (configMemoria.estrategia.empty()) {
        return; // Usuario cancelo
    }
    
    vector<Proceso> procesos = solicitarProcesos();
    vector<SolicitudMemoria> solicitudes = solicitarSolicitudesMemoria(procesos);
    
    cout << "\nSimulacion de memoria con estrategia " << configMemoria.estrategia 
         << " y tamaño " << configMemoria.tam << " bytes.\n";
    cout << "Funcionalidad de memoria aun no implementada completamente.\n";
    
    pausar();
}

// Funcion para simulacion completa mejorada (usando la logica del segundo codigo)
void simulacionCompleta() {
    cout << "\n===============================\n";
    cout << "      SIMULACION COMPLETA\n";
    cout << "===============================\n";
    cout << "Ingrese la configuracion del sistema:\n";
    
    try {
        // Solicitar configuracion de CPU usando el estilo del segundo codigo
        ConfiguracionCPU configCPU = solicitarConfiguracionCPUCompleta();
        
        if (configCPU.algoritmo.empty()) {
            return;
        }
        
        // Solicitar procesos
        vector<Proceso> procesos = solicitarProcesos();
        
        // Solicitar configuracion de memoria usando el estilo del segundo codigo
        ConfiguracionMemoria configMemoria = solicitarConfiguracionMemoriaCompleta();
        
        if (configMemoria.estrategia.empty()) {
            return;
        }
        
        // Solicitar solicitudes de memoria
        vector<SolicitudMemoria> solicitudes = solicitarSolicitudesMemoria(procesos);
        
        cout << "\nEjecutando simulacion completa...\n";
        cout << "Algoritmo CPU: " << configCPU.algoritmo;
        if (configCPU.quantum > 0) {
            cout << " (Quantum: " << configCPU.quantum << ")";
        }
        cout << endl;
        cout << "Estrategia Memoria: " << configMemoria.estrategia 
             << " (" << configMemoria.tam << " bytes)\n";
        
        // Ejecutar simulacion de CPU
        ejecutarSimulacion(procesos, configCPU);
        
        // Mostrar resultados de CPU
        mostrarTablaResultados(procesos);
        
        cout << "\nGestion de memoria aun no implementada completamente.\n";
        pausar();
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        pausar();
    }
}

int main() {
    int opcion;
    
    do {
        mostrarMenuPrincipal();
        cin >> opcion;
        limpiarBuffer();
        
        switch (opcion) {
            case 1:
                manejarPlanificacionCPU();
                break;
            case 2:
                manejarGestionMemoria();
                break;
            case 3:
                simulacionCompleta();
                break;
            case 4:
                cout << "\nGracias por usar el Simulador de Sistema Operativo!\n";
                break;
            default:
                cout << "\nOpcion invalida. Por favor seleccione una opcion valida.\n";
                pausar();
        }
    } while (opcion != 4);
    
    return 0;
}