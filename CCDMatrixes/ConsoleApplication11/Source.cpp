#include <iostream>
#include <list>
#include <stdlib.h>

#define pi 3.14159265358979323846
using namespace std;

struct zespolone {
	double re, im;
	zespolone operator * (zespolone &b) 
	{ 
		return zespolone{ re*b.re - im*b.im,  re*b.im + im*b.re }; 
	}
	zespolone operator + (zespolone &b) 
	{
		return zespolone{ re + b.re,im + b.im }; 
	}
	zespolone operator / (zespolone &b) 
	{ 
		double pomoc = ((re*b.re + im*b.im) / ((b.re*b.re) + (b.im*b.im))) + ((im*b.re - re*b.im) / ((b.re*b.re) + (b.im*b.im))); 
		return zespolone{ pomoc, 0 }; 
	}
};

struct macierz {
	zespolone tab[2][2] = { { { 1 },{ 1 } },{ { 1 },{ 1 } } };
	macierz operator*(macierz b) 
	{
		macierz wynikowa;
		wynikowa.tab[0][0] = tab[0][0] * b.tab[0][0] + tab[0][1] * b.tab[1][0];
		wynikowa.tab[0][1] = tab[0][0] * b.tab[0][1] + tab[0][1] * b.tab[1][1];
		wynikowa.tab[1][0] = tab[1][0] * b.tab[0][0] + tab[1][1] * b.tab[1][0];
		wynikowa.tab[1][1] = tab[1][0] * b.tab[0][1] + tab[1][1] * b.tab[1][1]; 
		return wynikowa;
	}
};

class element
{
public:
	virtual zespolone Z() = 0;
	bool rownolegle; macierz m;
	   void podst_macierz() {
		   if (rownolegle == false) 
		   { 
			m.tab[0][1] = Z();
			m.tab[1][0] = zespolone{ 0,0 }; 
		   }
		   else 
		   {
			   m.tab[0][1] = zespolone{ 0,0 }; 
			   m.tab[1][0] = (zespolone{ 1,0 } / Z()); 
		   }
	   }
	   element(bool rownolegle); 
	   element();
	   ~element();
};

element::element(bool rownolegle) : rownolegle(rownolegle) {}
element::element() {}
element::~element() {}

class opornik :
	public element { 
		public:
			double R; 
			opornik(double R, bool rownolegle); 
			zespolone Z();
			~opornik(); 
};

opornik::opornik(double R, bool rownolegle) :R(R), element(rownolegle) {}
zespolone opornik::Z() { return zespolone{ R,0 }; }
opornik::~opornik() {}

class kondensator :
	public element {
		public:double C, f; 
			   kondensator(double C, double f, bool rownolegle); 
			   zespolone Z(); 
			   ~kondensator();
};

kondensator::kondensator(double C, double f, bool rownolegle) :C(C), f(f), element(rownolegle) {}
zespolone kondensator::Z() { return zespolone{ 1,0 } / zespolone{ 0,2 * pi*f*C }; }
kondensator::~kondensator() {}

class cewka :
	public element {
		public:
			double L, f;
			cewka(double L, double f, bool rownolegle); 
			zespolone Z();
			~cewka(); 
};

cewka::cewka(double L, double f, bool rownolegle) :L(L), f(f), element(rownolegle) {}
zespolone cewka::Z() { return zespolone{ 0, 2 * pi*f*L }; }
cewka::~cewka() {}

int main(int argc, char *argv[]) {

	list<element*> lista_elementow;
	double czestotliwosc_min = atof(argv[1]);
	double czestotliwosc_max = atof(argv[3]);
	double skok = atof(argv[2]);

	macierz wynikowa;

	for (int f = czestotliwosc_min; f <= czestotliwosc_max; f += skok) {

		FILE* plik = fopen(argv[4], "r");
		macierz wynikowa;

		int komentarz[1000];
		bool rownolegle; 
		char znak[2000], check; 
		double liczba;

		element *elem;

		check = ' ';

		for (int y = 1; y < 20; y++) {

			check = znak[y];
			if (check == '#') break;

			znak[y] = fgetc(plik); 
			fscanf(plik, "%lf", &liczba);
			
			if (znak[y] == '-')	rownolegle = false; 
			else rownolegle = true;
			
			if (znak[y + 1] == 'R') 
			{ 
				elem = new opornik(liczba, rownolegle); 
				lista_elementow.push_back(elem); 
				elem->m; 
				elem->podst_macierz(); 
				wynikowa = wynikowa * elem->m; 
			}

			else if (znak[y + 1] == 'C') 
			{ 
				elem = new kondensator(liczba, f, rownolegle); 
				lista_elementow.push_back(elem); 
				elem->m;
				elem->podst_macierz(); 
				wynikowa = wynikowa * elem->m; 
			}

			else if (znak[y + 1] == 'L') 
			{
				elem = new cewka(liczba, f, rownolegle);
				lista_elementow.push_back(elem);
				elem->m;
				elem->podst_macierz();
				wynikowa = wynikowa * elem->m;
			}

			if (znak[y] == '*') {
				do { 
					znak[y] = komentarz[1000];
				} while (znak[y] == '$');
			}

		}
		
		fclose(plik);

		zespolone jeden{ 1,0 }; 
		zespolone Ku = (jeden / wynikowa.tab[0][0]);
		zespolone Zin = (wynikowa.tab[0][0] / wynikowa.tab[1][0]);
		cout << f << " | Ku = " << Ku.re << " + " << Ku.im << "i | Zin = " << Zin.re << " + " << Zin.im << "i" << endl;
	}return 0;
}