#include <iostream>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

/* =====================================================
   PRESTUPEK – spojovy seznam (Linked List)
   ===================================================== */
struct Offence {
    string popis;
    int body;
    string datum;
    Offence* next;

    Offence(const string& p = "", int b = 0, const string& d = "")
        : popis(p), body(b), datum(d), next(nullptr) {}
};

void freeOffences(Offence* h) {
    while (h) {
        Offence* t = h;
        h = h->next;
        delete t;
    }
}

/* =====================================================
   RIDIC (Driver)
   ===================================================== */
struct Driver {
    int id;
    string jmeno;
    string prijmeni;
    int body;
    Offence* offences;

    Driver(int i = 0, const string& j = "", const string& p = "")
        : id(i), jmeno(j), prijmeni(p), body(0), offences(nullptr) {}
};

/* =====================================================
   AVL STROM RIDICU
   ===================================================== */
struct DriverNode {
    Driver data;
    DriverNode* left;
    DriverNode* right;
    int height;

    DriverNode(const Driver& d)
        : data(d), left(nullptr), right(nullptr), height(1) {}

    ~DriverNode() {
        freeOffences(data.offences);
    }
};

int h(DriverNode* n) { return n ? n->height : 0; }

int bf(DriverNode* n) { return n ? h(n->left) - h(n->right) : 0; }

void upd(DriverNode* n) {
    n->height = 1 + max(h(n->left), h(n->right));
}

// Rotace doprava
DriverNode* rotR(DriverNode* y) {
    DriverNode* x = y->left;
    DriverNode* t = x->right;
    x->right = y;
    y->left = t;
    upd(y); upd(x);
    return x;
}

// Rotace doleva
DriverNode* rotL(DriverNode* x) {
    DriverNode* y = x->right;
    DriverNode* t = y->left;
    y->left = x;
    x->right = t;
    upd(x); upd(y);
    return y;
}

// Vlozeni
DriverNode* avlInsert(DriverNode* r, const Driver& d) {
    if (!r) return new DriverNode(d);

    if (d.id < r->data.id) r->left = avlInsert(r->left, d);
    else if (d.id > r->data.id) r->right = avlInsert(r->right, d);
    else return r; // Duplicity nepovolujeme

    upd(r);
    int b = bf(r);

    if (b > 1 && d.id < r->left->data.id) return rotR(r);
    if (b < -1 && d.id > r->right->data.id) return rotL(r);
    if (b > 1 && d.id > r->left->data.id) {
        r->left = rotL(r->left);
        return rotR(r);
    }
    if (b < -1 && d.id < r->right->data.id) {
        r->right = rotR(r->right);
        return rotL(r);
    }
    return r;
}

DriverNode* minNode(DriverNode* n) {
    while (n->left) n = n->left;
    return n;
}

DriverNode* avlDelete(DriverNode* r, int id) {
    if (!r) return nullptr;

    if (id < r->data.id) r->left = avlDelete(r->left, id);
    else if (id > r->data.id) r->right = avlDelete(r->right, id);
    else {
        if (!r->left || !r->right) {
            DriverNode* t = r->left ? r->left : r->right;
            delete r;
            return t;
        }

        DriverNode* t = minNode(r->right);

        freeOffences(r->data.offences);
        r->data = t->data;              
        t->data.offences = nullptr;     

        r->right = avlDelete(r->right, t->data.id);
    }

    upd(r);
    int b = bf(r);

    if (b > 1 && bf(r->left) >= 0) return rotR(r);
    if (b > 1 && bf(r->left) < 0) {
        r->left = rotL(r->left);
        return rotR(r);
    }
    if (b < -1 && bf(r->right) <= 0) return rotL(r);
    if (b < -1 && bf(r->right) > 0) {
        r->right = rotR(r->right);
        return rotL(r);
    }
    return r;
}

DriverNode* avlFind(DriverNode* r, int id) {
    if (!r) return nullptr;
    if (id == r->data.id) return r;
    return id < r->data.id ? avlFind(r->left, id) : avlFind(r->right, id);
}

// In-order pruchod stromem
template<typename F>
void inorder(DriverNode* n, F f) {
    if (!n) return;
    inorder(n->left, f);
    f(n);
    inorder(n->right, f);
}

void freeAVL(DriverNode* n) {
    if (!n) return;
    freeAVL(n->left);
    freeAVL(n->right);
    delete n;
}

/* =====================================================
   HASH TABULKA VOZIDEL (Chaining)
   ===================================================== */
struct Vehicle {
    string spz;
    string typ;
    int driverId;
};

struct VehicleNode {
    Vehicle data;
    VehicleNode* next;
};

class VehicleHash {
    VehicleNode** table;
    int cap;

    int hash(const string& s) {
        unsigned long h = 5381;
        for (char c : s) h = ((h << 5) + h) + c;
        return h % cap;
    }

public:
    VehicleHash(int c = 211) : cap(c) {
        table = new VehicleNode * [cap];
        for (int i = 0; i < cap; i++) table[i] = nullptr;
    }

    ~VehicleHash() {
        for (int i = 0; i < cap; i++)
            while (table[i]) {
                VehicleNode* t = table[i];
                table[i] = table[i]->next;
                delete t;
            }
        delete[] table;
    }

    bool insert(const Vehicle& v) {
        int i = hash(v.spz);
        for (VehicleNode* n = table[i]; n; n = n->next)
            if (n->data.spz == v.spz) return false;
        table[i] = new VehicleNode{ v, table[i] };
        return true;
    }

    Vehicle* find(const string& spz) {
        int i = hash(spz);
        for (VehicleNode* n = table[i]; n; n = n->next)
            if (n->data.spz == spz) return &n->data;
        return nullptr;
    }

    void printByDriver(int id) {
        bool any = false;
        for (int i = 0; i < cap; i++)
            for (VehicleNode* n = table[i]; n; n = n->next)
                if (n->data.driverId == id) {
                    cout << n->data.typ << "/" << n->data.spz << " ";
                    any = true;
                }
        if (!any) cout << "zadne";
        cout << "\n";
    }

    // Odstraneni vsech vozidel daneho ridice
    void removeByDriver(int id) {
        for (int i = 0; i < cap; i++) {
            VehicleNode** p = &table[i];
            while (*p) {
                if ((*p)->data.driverId == id) {
                    VehicleNode* t = *p;
                    *p = (*p)->next;
                    delete t;
                }
                else p = &(*p)->next;
            }
        }
    }
};

/* =====================================================
   HALDA (Heap) – pro Top hrisniky
   ===================================================== */
struct HeapEntry {
    int id;
    int body;
};

class OffenderHeap {
    HeapEntry* A;
    int size;
    int cap;

    void heapify(int i) {
        int m = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        if (l < size && A[l].body > A[m].body) m = l;
        if (r < size && A[r].body > A[m].body) m = r;
        if (m != i) {
            swap(A[i], A[m]);
            heapify(m);
        }
    }

public:
    OffenderHeap() : A(nullptr), size(0), cap(0) {}

    ~OffenderHeap() { delete[] A; }

    void build(DriverNode* root) {
        delete[] A;
        cap = 32;
        size = 0;
        A = new HeapEntry[cap];

        inorder(root, [&](DriverNode* n) {
            if (size == cap) {
                cap *= 2;
                HeapEntry* B = new HeapEntry[cap];
                for (int i = 0; i < size; i++) B[i] = A[i];
                delete[] A;
                A = B;
            }
            A[size++] = { n->data.id, n->data.body };
            });

        for (int i = size / 2 - 1; i >= 0; i--)
            heapify(i);
    }

    void printTop(int n, DriverNode* root) {
        for (int i = 0; i < n && size > 0; i++) {
            HeapEntry e = A[0];
            A[0] = A[--size];
            heapify(0);
            DriverNode* d = avlFind(root, e.id);
            if (d)
                cout << d->data.id << " - " << d->data.jmeno << " "
                << d->data.prijmeni << " (" << d->data.body << " bodu)\n";
        }
    }
};

/* =====================================================
   GLOBALNI PROMENNE A POMOCNE FUNKCE
   ===================================================== */
DriverNode* drivers = nullptr;
VehicleHash vehicles;
OffenderHeap heap;

int readInt() {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return x;
}

string lower(const string& s) {
    string r = s;
    transform(r.begin(), r.end(), r.begin(), ::tolower);
    return r;
}

/* =====================================================
   HLAVNI PROGRAM (Menu)
   ===================================================== */
int main() {
    while (true) {
        cout << "\n--- Registr vozidel ---\n";
        cout << "1 Pridat ridice\n";
        cout << "2 Pridat vozidlo\n";
        cout << "3 Najit vozidla podle ridice\n";
        cout << "4 Najit ridice podle vozidla\n";
        cout << "5 Pridat prestupek\n";
        cout << "6 Historie prestupku\n";
        cout << "7 Top hrisnici\n";
        cout << "8 Smazat ridice\n";
        cout << "9 Seznam ridicu\n";
        cout << "10 Hledat podle jmena\n";
        cout << "0 Konec\n";
        cout << "Volba: ";

        int v = readInt();

        if (v == 0) {
            freeAVL(drivers);
            return 0;
        }
        if (v == 1) {
            int id; string j, p;
            cout << "ID: "; id = readInt();
            cout << "Jmeno: "; getline(cin, j);
            cout << "Prijmeni: "; getline(cin, p);
            drivers = avlInsert(drivers, Driver(id, j, p));
        }
        if (v == 2) {
            string spz, typ; int id;
            cout << "SPZ: "; getline(cin, spz);
            cout << "Typ: "; getline(cin, typ);
            cout << "ID ridice: "; id = readInt();
            if (avlFind(drivers, id)) {
                if (vehicles.insert({ spz, typ, id }))
                    cout << "Vozidlo pridano.\n";
                else
                    cout << "Chyba: SPZ jiz existuje.\n";
            }
            else {
                cout << "Chyba: Ridic s timto ID neexistuje.\n";
            }
        }
        if (v == 3) {
            int id; cout << "ID ridice: "; id = readInt();
            vehicles.printByDriver(id);
        }
        if (v == 4) {
            string spz; cout << "SPZ: "; getline(cin, spz);
            Vehicle* v = vehicles.find(spz);
            if (!v) cout << "Vozidlo nenalezeno.\n";
            else {
                DriverNode* d = avlFind(drivers, v->driverId);
                if (d)
                    cout << "Majitel: " << d->data.jmeno << " " << d->data.prijmeni << "\n";
                else
                    cout << "Vozidlo existuje, ale majitel (ID " << v->driverId << ") nenalezen.\n";
            }
        }
        if (v == 5) {
            int id, b; string pop, dat;
            cout << "ID ridice: "; id = readInt();
            DriverNode* d = avlFind(drivers, id);
            if (!d) { cout << "Ridic nenalezen.\n"; continue; }
            cout << "Popis prestupku: "; getline(cin, pop);
            cout << "Body: "; b = readInt();
            cout << "Datum: "; getline(cin, dat);

            Offence* o = new Offence(pop, b, dat);
            o->next = d->data.offences;
            d->data.offences = o;
            d->data.body += b;
            cout << "Prestupek pridan.\n";
        }
        if (v == 6) {
            int id; cout << "ID ridice: "; id = readInt();
            DriverNode* d = avlFind(drivers, id);
            if (!d) cout << "Ridic nenalezen.\n";
            else {
                cout << "Historie prestupku:\n";
                for (Offence* o = d->data.offences; o; o = o->next)
                    cout << "- " << o->datum << " (+" << o->body << "b) " << o->popis << "\n";
            }
        }
        if (v == 7) {
            heap.build(drivers);
            cout << "Pocet zaznamu k zobrazeni: ";
            int n = readInt();
            heap.printTop(n, drivers);
        }
        if (v == 8) {
            int id; cout << "ID ridice: "; id = readInt();
            if (avlFind(drivers, id)) {
                vehicles.removeByDriver(id); // Nejprve smazeme vozidla
                drivers = avlDelete(drivers, id); // Pak ridice
                cout << "Ridic a jeho vozidla smazany.\n";
            }
            else {
                cout << "Ridic nenalezen.\n";
            }
        }
        if (v == 9) {
            cout << "Seznam vsech ridicu:\n";
            inorder(drivers, [&](DriverNode* n) {
                cout << "ID: " << n->data.id << " | " << n->data.jmeno << " "
                    << n->data.prijmeni << " | Body: " << n->data.body << " | Vozidla: ";
                vehicles.printByDriver(n->data.id);
                });
        }
        if (v == 10) {
            string s; cout << "Hledat jmeno/prijmeni: "; getline(cin, s);
            string t = lower(s);
            bool found = false;
            inorder(drivers, [&](DriverNode* n) {
                if (lower(n->data.jmeno).find(t) != string::npos ||
                    lower(n->data.prijmeni).find(t) != string::npos) {
                    cout << n->data.id << " " << n->data.jmeno
                        << " " << n->data.prijmeni << "\n";
                    found = true;
                }
                });
            if (!found) cout << "Zadna shoda.\n";
        }
    }
}