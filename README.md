# ProiectPCD


## Componentele aplicației
- Componenta server - gestionează și oferă resurse clienților. 
      Reprezintă nucleul aplicației.
- Componenta client cu funcție de utilizare
  - Subcomponenta procesare de imagine: procesează imaginile video primite de la mașină (componenta simulare). Rezultatele procesării vor fi trimise într-o coadă de așteptare de unde componenta de administrare le va memora într-o bază de date;
  - Subcomponenta verificare date de intrare: verifică dacă datele intrinseci ale mașinii sunt în parametri optimi și trimite într-o coadă de așteptare rezultatele obținute;
  - Subcomponenta cerere informații - mașina poate cere informații dacă zona de drum este periculoasă;
Subcomponentele trebuie să aibă o rulare paralelă iar numărul de instanțe ale lor este gestionat de către componenta client de administrare.
- Componenta client de administrare - salvează rezultatele calculelor din cozile de așteptare, oferă noi procese pentru subcomponentele din componenta client cu funcție de utilizare când cererea accesului la server este mare (mai multe mașini trimit date). Creează crash logs și general logs (după fiecare procesare) în cazul apariției unor situații unice(EXIT_ERROR, proces terminat de către Ctrl-C, etc).
- Componenta simulare - pentru că nu există mașini care să trimită datele către aplicația centrală, această funcție se delegă componentei simulare, care va trimite aplicației datele. Controlarea numărului de date trimise și vizualizarea rezultatelor se va realiza de către componenta web.
- Componenta web - integrată într-un MVC scris în php
  - Evidențiază funcționalitatea aplicației client-server.
  - Controlarea numărului de date trimise ale componentei simulare (numărul de mașini care accesează simultan aplicația) - are rolul de a face vizibilă funcționalitatea întregii aplicații: numărul de instanțe de procesare de imagine, timpi de execuție, salvarea datelor, etc.
  - Evidențiază funcționalitatea subcomponentei cerere informații.
  - Vizualizarea rezultatelor calculelor precedente.   


## Initial run
Use following command to install all dependencies and generate Makefile. If it's not running, run first `chmod +x build_util.sh`
```bash
./build_util.sh all
```

## Compiling project
Instead using standard `gcc -o file file.c`, run the following command. This will compile all existing C files. Find the executables in **build/**
```bash
./build_util.sh
```

## Adding new C files
To be compiled, new C files are needed to be specified in `CMakeLists.txt` by the following syntax: `add_executable(new_file new_file.c)`. After adding them run the next two commands:
```bash
./build.sh all
./build.sh
```
