# Registr vozidel a řidičů (C++)

Konzolová aplikace pro evidenci řidičů, jejich vozidel a dopravních přestupků. Semestrální práce z předmětu **Datové struktury a algoritmy** (VŠPJ, ZS 2025/26). Hlavní důraz je na **vlastní implementaci datových struktur bez STL kontejnerů**.

## Co aplikace umí

- přidat řidiče a vozidlo (SPZ), vyhledat vozidla podle řidiče a řidiče podle SPZ,
- zapsat přestupek s počtem bodů a zobrazit historii přestupků řidiče,
- žebříček řidičů s nejvíce body („top hříšníci“),
- vyhledávání podle jména, výpis všech řidičů, mazání řidiče včetně jeho dat.

## Použité datové struktury

| Struktura | K čemu slouží | Složitost |
|---|---|---|
| **AVL strom** (`DriverNode`) | řidiči podle ID, vždy vyvážený (rotace LL/RR/LR/RL) | vložení, hledání, mazání O(log n) |
| **Hashovací tabulka** (`VehicleHash`) | vozidla podle SPZ, řetězení kolizí | průměrně O(1) |
| **Spojový seznam** (`Offence`) | historie přestupků řidiče | přidání O(1) |
| **Binární halda** (`OffenderHeap`) | výběr řidičů s nejvíce body | vložení / odebrání O(log n) |

## Technologie

C++17 · Visual Studio 2022 (projekt `.sln` / `.vcxproj`) · bez externích knihoven

## Jak spustit

**Visual Studio:** otevřít `registr_vozidel.sln` a spustit (F5).

**g++ / clang:**

```bash
g++ -std=c++17 -O2 registr_vozidel/registr_vozidel.cpp -o registr
./registr
```

## Struktura

```
registr_vozidel/registr_vozidel.cpp   # celá aplikace (struktury + menu)
registr_vozidel.sln                   # řešení pro Visual Studio
```

---
*Školní projekt – historie commitů odpovídá datu odevzdání v Moodlu.*
