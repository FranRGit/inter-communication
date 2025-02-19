#include <iostream>
#include <thread>
#include <mutex>
#include <barrier>
#include <future>
#include <vector>
#include <random>
#include <chrono>

const int NUM_SENSORES = 5;
std::mutex mtx_datos;
std::barrier barrera(NUM_SENSORES);
std::vector<int> datos_sensores;

void sensor(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500 * id)); 
    std::cout << "Sensor " << id << " está listo.\n";

    barrera.arrive_and_wait(); 

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(10, 100);
    int medicion = distrib(gen);

    {
        std::lock_guard<std::mutex> lock(mtx_datos);
        datos_sensores.push_back(medicion);
    }

    std::cout << "Sensor " << id << " registró el valor: " << medicion << "\n";
}

void ordenarDatos() {
    { 
        std::lock_guard<std::mutex> lock(mtx_datos);
        std::sort(datos_sensores.begin(), datos_sensores.end());
    }

    std::cout << "Datos ordenados: ";
    for (int dato : datos_sensores) {
        std::cout << dato << "\n";
    }

    
}

int main() {
    std::vector<std::thread> hilos;

    for (int i = 0; i < NUM_SENSORES; ++i) {
        hilos.emplace_back(sensor, i + 1);
    }


    for (auto &hilo : hilos) {
        hilo.join();
    }
    
    std::future<void> reporte = std::async(std::launch::async, ordenarDatos);

    return 0;
}
