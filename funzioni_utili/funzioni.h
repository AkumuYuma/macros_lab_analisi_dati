#ifndef FUNZIONI_H 
#define FUNZIONI_H

// Serie di funzioni utili per le macro. 
// Per evitare troppo casino con i file .C e .h, andrò a dichiarare e definire tutte le funzioni in questo file. 
// In teoria qui andrebbero solo le dichiarazioni e bisognerebbe fare un file .C a parte con le definizioni. 
// Ma sticazzi, poi è un casino a compilare tutto 😅 

#include <ctime>
#include <string> 

namespace utils {
    const std::string currentDate(); 
};

const std::string utils::currentDate() {
    /* Restituisce la data corrente nel formato anno-mese-giorno */
    time_t     now = time(0); // Momento corrente 
    char       buf[80];
    struct tm tstruct{*localtime(&now)}; // *localtime restituisce la rappresentazione in "struct tm" del tempo locale nella tua timezone
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct); // Scrive la data (contenuta in tstruct) nel buffer buf
    std::string date{buf}; // Inizializzo una stringa di c++
    return date.substr(0, 10); // Ne prendo solo i primi 10 caratteri solo la data 
}

#endif