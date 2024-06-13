#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <cstring>
#include <cctype>
#include <cmath>
#include <string>
#include <fstream>
#include <complex>
using namespace std;

const int szerokosc_pliku = 1440;
const int wysokosc_pliku = 600;
const int grubosc = 3;
const int L_PRZYCISKOW = 16;
const int LICZBA_ZNAKOW = 6;
const char ZNAK = '.';
const int probki = 400; // ilość próbek
const long double PI = 3.1415926535;
const int odleglosc_miedzy_duzymi_prostokatami = szerokosc_pliku / 28;
const int odleglosc_duzych_prostokatow_od_sufitu = wysokosc_pliku / 14;
const int duza_szerokosc = szerokosc_pliku / 7 * 2;
const int duza_wysokosc = wysokosc_pliku / 3;

struct charfloat {
	float f;
	char c[LICZBA_ZNAKOW];
};

struct wazneobszary {
	int x;
	int y;
	int szerokosc;
	int wysokosc;
	charfloat* pole;
};

struct wymiaryokienek {
	int szerokosc;
	int wysokosc;
};

struct wykresybode {
	sf::Image amplitudowa;
	sf::Image fazowa;
};

void rysuj(sf::RenderWindow& window, sf::Sprite ekran, sf::Text przyciski_text[], sf::RectangleShape rama[], sf::VertexArray wykres, wykresybode bode, sf::Text stabilnosc_txt, wazneobszary przyciski[], sf::Text* os[]) {

	window.clear(sf::Color::White);
	window.draw(ekran);

	for (int i = 0; i < L_PRZYCISKOW - 3; i++) {
		przyciski_text[i].setString((*przyciski[i + 3].pole).c);
		window.draw(przyciski_text[i]);
	}

	for (int i = 0; i < 15; i++) {
		window.draw((*os[i]));
	}

	window.draw(rama[0]);
	window.draw(rama[1]);
	window.draw(rama[2]);

	window.draw(stabilnosc_txt);

	window.draw(wykres);

	sf::Texture texture;
	if (!texture.loadFromImage(bode.amplitudowa)) {
		cout << "BRAK CHARAKTERYSTYKI AMPLITUDOWEJ";
	}
	else {
		sf::Sprite sprite;
		sprite.setTexture(texture);
		sprite.setPosition(odleglosc_miedzy_duzymi_prostokatami * 2 + duza_szerokosc, odleglosc_duzych_prostokatow_od_sufitu);
		window.draw(sprite);
	}
	if (!texture.loadFromImage(bode.fazowa)) {
		cout << "BRAK CHARAKTERYSTYKI FAZOWEJ";
	}
	else {
		sf::Sprite sprite;
		sprite.setTexture(texture);
		sprite.setPosition(odleglosc_miedzy_duzymi_prostokatami * 3 + duza_szerokosc * 2, odleglosc_duzych_prostokatow_od_sufitu);
		window.draw(sprite);
	}

	window.display();
	return;
}

void klik(int x, int y, wazneobszary przyciski[], int& sygnal, charfloat*& transmitancja) {
	for (int i = 0; i < L_PRZYCISKOW; i++) {
		if (x >= przyciski[i].x && y >= przyciski[i].y && x <= przyciski[i].x + przyciski[i].szerokosc && y <= przyciski[i].y + przyciski[i].wysokosc) {
			if (i < 3 && i != sygnal) {
				sygnal = i;
				return;
			}
			else {
				transmitancja = przyciski[i].pole;
			}
			return;
		}
	}
}

void wpisywanie(char& nowy, charfloat*& transmitancja) {
	if (!isdigit(nowy) && nowy != ZNAK && nowy != '-' || nowy == ZNAK && strlen((*transmitancja).c) == 0 || nowy == '-' && strlen((*transmitancja).c) != 0) {
		return;
	}
	if (strlen((*transmitancja).c) >= LICZBA_ZNAKOW - 1) {
		for (int i = 0; i < LICZBA_ZNAKOW - 1; i++) {
			(*transmitancja).c[i] = NULL;
		}
	}
	else {
		for (int i = 0; i < LICZBA_ZNAKOW - 1; i++) {
			if ((*transmitancja).c[i] == NULL) {
				(*transmitancja).c[i] = nowy;
				nowy = NULL;
				return;
			}
		}
	}
	(*transmitancja).c[0] = nowy;
	nowy = NULL;
	return;
}

void zatwierdz(wazneobszary przyciski[]) {
	for (int ii = 3; ii < L_PRZYCISKOW; ii++) {
		if (strlen((*przyciski[ii].pole).c) == 0) {
			(*przyciski[ii].pole).f = 0;
			(*przyciski[ii].pole).c[0] = '0';
			continue;
		}
		else {
			int przecinek = NULL;
			(*przyciski[ii].pole).f = 0;

			for (int i = 0; i < LICZBA_ZNAKOW - 1; i++) {
				if ((*przyciski[ii].pole).c[i] == ZNAK) {
					przecinek = i;
					break;
				}
			}

			if (przecinek == NULL) {
				(*przyciski[ii].pole).f = stoi((*przyciski[ii].pole).c);
			}
			else {
				for (int i = 0; i < LICZBA_ZNAKOW - 1; i++) {
					if (i != przecinek && (*przyciski[ii].pole).c[i] != NULL && (*przyciski[ii].pole).c[i] != '-') {
						if (i < przecinek) {
							(*przyciski[ii].pole).f += ((*przyciski[ii].pole).c[i] - 48) * pow(10, przecinek - 1 - i);
						}
						else {
							(*przyciski[ii].pole).f += ((*przyciski[ii].pole).c[i] - 48) * pow(10, przecinek - i);
						}
					}
				}
				if ((*przyciski[ii].pole).c[0] == '-') {
					(*przyciski[ii].pole).f = -1 * (*przyciski[ii].pole).f;
				}
			}
		}
	}
	return;
}

void usun(charfloat*& transmitancja) {
	for (int i = LICZBA_ZNAKOW - 2; i >= 0; i--) {
		if ((*transmitancja).c[i] != NULL) {
			(*transmitancja).c[i] = NULL;
			return;
		}
	}
	return;
}

void uzupelnijosie(float max, sf::Text& maxy, sf::Text& miny, sf::Text& maxx, float h) {
	cout << endl << max << endl;
	int liczbapoprzecinku = 0;
	maxx.setString(to_string(static_cast<int>(probki * h)));
	if (max > 100) {
		maxy.setString(to_string(static_cast<int>(round(max))));
		miny.setString(to_string(static_cast<int>(-round(max))));
		return;
	}
	else if (max > 1) {
		if (round((max - floor(max)) * 10) == 10) {
			maxy.setString(to_string(static_cast<int>(round(max))));
			miny.setString(to_string(static_cast<int>(-round(max))));
		}
		else {
			maxy.setString(to_string(static_cast<int>(floor(max))) + "." + to_string(static_cast<int>(round((max - floor(max)) * 10))));
			miny.setString(to_string(static_cast<int>(-floor(max))) + "." + to_string(static_cast<int>(round((max - floor(max)) * 10))));
		}
		return;
	}
	do {

		if (max < 1) {
			liczbapoprzecinku--;
			max = max * 10;

		}
		else {
			break;
		}
	} while (1);
	string y;
	y = to_string(static_cast<int>(round(max))) + "e" + to_string(liczbapoprzecinku);
	maxy.setString(y);
	y = to_string(static_cast<int>(-round(max))) + "e" + to_string(liczbapoprzecinku);
	miny.setString(y);
	return;
}

sf::VertexArray sinusoida(charfloat Acf[4], charfloat Bcf[5], wymiaryokienek duze, float h, float M, float w, float& max) {

	double u[probki + 1];
	float x[4] = { 0,0,0,0 }; // kazda cyfra to kolejna pochodna
	double y[probki + 1];

	for (int i = 0; i < probki + 1; i++)
	{
		u[i] = M * sin(w * 2 * PI * i * h);
	}

	float mac_A[4][4]; // pierwsza cyfra macierzy to numer wiersza druga kolumny
	float mac_B[4];
	float mac_C[4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mac_A[i][j] = 0;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		mac_A[3][i] = -Bcf[i].f / Bcf[4].f;
		mac_B[i] = 0;
		mac_C[i] = Acf[i].f / Bcf[4].f;
	}
	for (int i = 0; i < 3; i++)
	{
		mac_A[i][i + 1] = 1;
	}
	mac_B[3] = 1;

	float mac_Ax[4] = { 0,0,0,0 };
	float mac_Bu[4] = { 0,0,0,0 };
	float mac_Cx = 0;
	float xi[4] = { 0,0,0,0 }; //warunki poczatkowe kolejne pochodne

	for (int i = 0; i < probki + 1; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mac_Ax[j] = mac_A[j][0] * x[0] + mac_A[j][1] * x[1] + mac_A[j][2] * x[2] + mac_A[j][3] * x[3];
			mac_Bu[j] = mac_B[j] * u[i];
			mac_Cx = mac_C[0] * x[0] + mac_C[1] * x[1] + mac_C[2] * x[2] + mac_C[3] * x[3];
		}
		for (int j = 0; j < 4; j++)
		{
			xi[j] = mac_Ax[j] + mac_Bu[j];
			xi[j] = xi[j] * h;
			xi[j] = xi[j] + x[j];
			x[j] = xi[j];
		}
		y[i] = mac_Cx;
	}

	max = 0;

	for (int i = 0; i < probki; i++) {
		if (y[i] > max && y[i] > 0 || y[i] < 0 && -y[i] > max) {
			if (y[i] < 0) {
				max = -y[i];
			}
			else {
				max = y[i];
			}
		}
	}



	for (int i = 0; i < probki; i++) {
		y[i] = y[i] * 99 / max;
	}


	sf::Vector2u size;
	size.x = duze.szerokosc;
	size.y = duze.wysokosc;

	sf::VertexArray line(sf::LinesStrip, probki);
	for (int i = 0; i < probki; i++) {
		if (y[i] < 101 && y[i] > -101) {
			line[i] = sf::Vertex(sf::Vector2f(i + odleglosc_miedzy_duzymi_prostokatami, -y[i] + odleglosc_duzych_prostokatow_od_sufitu + 100), sf::Color::Black);
		}
	}
	return line;
}

sf::VertexArray prostokat(charfloat Acf[4], charfloat Bcf[5], wymiaryokienek duze, float h, float M, float t, float& max) {

	double u[probki + 1];
	float x[4] = { 0,0,0,0 }; // kazda cyfra to kolejna pochodna
	double y[probki + 1];

	for (int i = 0; i < probki + 1; i++)
	{
		u[i] = 0;
	}

	for (int i = 0; i < t / h + 1; i++)
	{
		u[i] = M;
	}

	float mac_A[4][4]; // pierwsza cyfra macierzy to numer wiersza druga kolumny
	float mac_B[4];
	float mac_C[4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mac_A[i][j] = 0;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		mac_A[3][i] = -Bcf[i].f / Bcf[4].f;
		mac_B[i] = 0;
		mac_C[i] = Acf[i].f / Bcf[4].f;
	}
	for (int i = 0; i < 3; i++)
	{
		mac_A[i][i + 1] = 1;
	}
	mac_B[3] = 1;

	float mac_Ax[4] = { 0,0,0,0 };
	float mac_Bu[4] = { 0,0,0,0 };
	float mac_Cx = 0;
	float xi[4] = { 0,0,0,0 }; //warunki poczatkowe kolejne pochodne

	for (int i = 0; i < probki + 1; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mac_Ax[j] = mac_A[j][0] * x[0] + mac_A[j][1] * x[1] + mac_A[j][2] * x[2] + mac_A[j][3] * x[3];
			mac_Bu[j] = mac_B[j] * u[i];
			mac_Cx = mac_C[0] * x[0] + mac_C[1] * x[1] + mac_C[2] * x[2] + mac_C[3] * x[3];
		}
		for (int j = 0; j < 4; j++)
		{
			xi[j] = mac_Ax[j] + mac_Bu[j];
			xi[j] = xi[j] * h;
			xi[j] = xi[j] + x[j];
			x[j] = xi[j];
		}
		y[i] = mac_Cx;
	}

	max = 0;

	for (int i = 0; i < probki; i++) {
		if (y[i] > max && y[i] > 0 || y[i] < 0 && -y[i] > max) {
			if (y[i] < 0) {
				max = -y[i];
			}
			else {
				max = y[i];
			}
		}
	}

	for (int i = 0; i < probki; i++) {
		y[i] = y[i] * 99 / max;
	}


	sf::Vector2u size;
	size.x = duze.szerokosc;
	size.y = duze.wysokosc;

	sf::VertexArray line(sf::LinesStrip, probki);
	for (int i = 0; i < probki; i++) {
		if (y[i] < 101 && y[i] > -101) {
			line[i] = sf::Vertex(sf::Vector2f(i + odleglosc_miedzy_duzymi_prostokatami, -y[i] + odleglosc_duzych_prostokatow_od_sufitu + 100), sf::Color::Black);
		}
	}
	return line;
}

sf::VertexArray trojkat(charfloat Acf[4], charfloat Bcf[5], wymiaryokienek duze, float h, float M, float w, float& max) {

	double A[4], B[5];
	for (int i = 0; i < 4; i++) {
		A[i] = Acf[i].f;
	}
	for (int i = 0; i < 5; i++) {
		B[i] = Bcf[i].f;
	}

	double u[probki + 1];
	float x[4] = { 0,0,0,0 }; // kazda cyfra to kolejna pochodna
	double y[probki + 1];

	int etap = 0;
	int j = 0;

	for (int i = 0; i < probki + 1; i++)
	{
		if (etap == 2)
		{
			u[i] = (j * h * 4 * M / w) - 4 * M;
			j++;
		}

		if (etap == 1)
		{
			u[i] = (j * h * (-4) * M / w) + 2 * M;
			j++;
		}

		if (etap == 0)
		{
			u[i] = j * h * 4 * M / w;
			j++;
		}

		if (u[i] >= M && etap == 0)
		{
			etap = 1;
		}

		if (u[i] <= -M && etap == 1)
		{
			etap = 2;
		}

		if (u[i] == 0 && etap == 2)
		{
			etap = 0;
			j = 1;
		}
	}

	float mac_A[4][4]; // pierwsza cyfra macierzy to numer wiersza druga kolumny
	float mac_B[4];
	float mac_C[4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mac_A[i][j] = 0;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		mac_A[3][i] = -Bcf[i].f / Bcf[4].f;
		mac_B[i] = 0;
		mac_C[i] = Acf[i].f / Bcf[4].f;
	}
	for (int i = 0; i < 3; i++)
	{
		mac_A[i][i + 1] = 1;
	}
	mac_B[3] = 1;

	float mac_Ax[4] = { 0,0,0,0 };
	float mac_Bu[4] = { 0,0,0,0 };
	float mac_Cx = 0;
	float xi[4] = { 0,0,0,0 }; //warunki poczatkowe kolejne pochodne

	for (int i = 0; i < probki + 1; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mac_Ax[j] = mac_A[j][0] * x[0] + mac_A[j][1] * x[1] + mac_A[j][2] * x[2] + mac_A[j][3] * x[3];
			mac_Bu[j] = mac_B[j] * u[i];
			mac_Cx = mac_C[0] * x[0] + mac_C[1] * x[1] + mac_C[2] * x[2] + mac_C[3] * x[3];
		}
		for (int j = 0; j < 4; j++)
		{
			xi[j] = mac_Ax[j] + mac_Bu[j];
			xi[j] = xi[j] * h;
			xi[j] = xi[j] + x[j];
			x[j] = xi[j];
		}
		y[i] = mac_Cx;
	}


	max = 0;

	for (int i = 0; i < probki; i++) {
		if (y[i] > max && y[i] > 0 || y[i] < 0 && -y[i] > max) {
			if (y[i] < 0) {
				max = -y[i];
			}
			else {
				max = y[i];
			}
		}
	}

	for (int i = 0; i < probki; i++) {
		y[i] = y[i] * 99 / max;
	}


	sf::Vector2u size;
	size.x = duze.szerokosc;
	size.y = duze.wysokosc;

	sf::VertexArray line(sf::LinesStrip, probki);
	for (int i = 0; i < probki; i++) {
		if (y[i] < 101 && y[i] > -101) {
			line[i] = sf::Vertex(sf::Vector2f(i + odleglosc_miedzy_duzymi_prostokatami, -y[i] + odleglosc_duzych_prostokatow_od_sufitu + 100), sf::Color::Black);
		}
	}
	return line;
}

complex<float> potegajeden(int pot) {
	complex<float> jednostkazesp(0, 1);
	complex<float> wynik(0, 1);
	if (pot == 0) {
		return 1;
	}
	for (int i = 0; i < pot - 1; i++) {
		wynik = wynik * jednostkazesp;
	}
	return wynik;
}

wykresybode bodefun(charfloat Acf[4], charfloat Bcf[5], wymiaryokienek duze, charfloat czestotliwos_poczatkowa) {
	complex<float> licznik, mianownik;
	complex<float> a[4], b[5];
	for (int i = 0; i < 4; i++) {
		a[i] = Acf[i].f * potegajeden(i);
	}
	for (int i = 0; i < 5; i++) {
		b[i] = Bcf[i].f * potegajeden(i);
	}

	sf::Vector2u size;
	size.x = duze.szerokosc;
	size.y = duze.wysokosc;

	wykresybode bode;
	bode.amplitudowa.create(size.x, size.y, sf::Color(255, 255, 255, 0));
	bode.fazowa.create(size.x, size.y, sf::Color(255, 255, 255, 0));

	for (float i = 0; i < probki; i++) {
		float x = pow(10, i / 100 + czestotliwos_poczatkowa.f);

		licznik = complex<float>(0, 0);
		mianownik = complex<float>(0, 0);
		for (int j = 0; j < 4; j++) {
			float y = pow(x, j);
			licznik += a[j] * y;
		}
		for (int j = 0; j < 5; j++) {
			float y = pow(x, j);
			mianownik += b[j] * y;
		}
		complex<float> wynik = licznik / mianownik;
		float y;
		y = abs(wynik);
		y = 20 * log10(y);
		if (y < 100 && y > -100)
		{
			bode.amplitudowa.setPixel(i, -y + 100, sf::Color(0, 0, 0));
		}
		y = arg(wynik);
		y = y * 90 / PI;
		if (y < 100 && y > -100)
		{
			bode.fazowa.setPixel(i, -y + 100, sf::Color(0, 0, 0));
		}
	}

	return bode;
}

bool stabilnosc(charfloat b[]) {

	if ((b[4].f > 0 && b[3].f > 0 && b[2].f > 0 && b[1].f > 0 && b[0].f > 0) || (b[4].f < 0 && b[3].f < 0 && b[2].f < 0 && b[1].f < 0 && b[0].f < 0))
	{
		double x = -(b[4].f * b[1].f - b[3].f * b[2].f) / (b[3].f);

		if ((-(b[4].f * b[1].f - b[3].f * b[2].f) > 0) && (-b[3].f * b[0].f + b[1].f * x > 0))
		{
			return 1;
		}
		else
		{
			return 0;
		}

		return 1;
	}
	return 0;
}


int main()
{

	sf::Font czcionka;
	if (!czcionka.loadFromFile("arial_narrow_7.ttf"))
	{
		std::cout << "Error loading font !";
	}

	int rozmiar_czcionki = szerokosc_pliku / 70;
	int dlugosc_ulamka = duza_szerokosc * 1.75;

	charfloat a[4];
	charfloat b[5];

	for (int i = 0; i < 5; i++) {
		if (i != 4) {
			a[i].c[0] = '1';
			a[i].f = 1;
		}
		b[i].c[0] = '1';
		b[i].f = 1;
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 1; j < LICZBA_ZNAKOW; j++) {
			if (i != 4) {
				a[i].c[j] = NULL;
			}
			b[i].c[j] = NULL;
		}
	}


	charfloat czestotliwos_poczatkowa;
	czestotliwos_poczatkowa.f = -1;
	czestotliwos_poczatkowa.c[0] = '-';
	czestotliwos_poczatkowa.c[1] = '1';

	for (int i = 2; i < LICZBA_ZNAKOW; i++) {
		czestotliwos_poczatkowa.c[i] = NULL;
	}


	charfloat podstawa_czasu;
	podstawa_czasu.f = 0.05;
	podstawa_czasu.c[0] = '0';
	podstawa_czasu.c[1] = '.';
	podstawa_czasu.c[2] = '0';
	podstawa_czasu.c[3] = '5';

	for (int i = 4; i < LICZBA_ZNAKOW; i++) {
		podstawa_czasu.c[i] = NULL;
	}


	charfloat amplituda;
	amplituda.f = 1;
	amplituda.c[0] = '1';

	for (int i = 1; i < LICZBA_ZNAKOW; i++) {
		amplituda.c[i] = NULL;
	}

	charfloat drugi_parametr;
	drugi_parametr.f = 1;
	drugi_parametr.c[0] = '1';

	for (int i = 1; i < LICZBA_ZNAKOW; i++) {
		drugi_parametr.c[i] = NULL;
	}


	// OBECNY OBSZAR SYGNALU OD 0 DO 2, PRZYPISANIA JAK ID PRZYCISKOW
	int obecny_sygnal = 0;
	// OBECNY OBSZAR TRANSMITANCJI OD 3 DO 11, PRZYPISANIA JAK ID PRZYCISKOW
	charfloat* obecne_pole;
	obecne_pole = &a[3];


	/* KLASYFIKACJA:
	* male - okienka z elementami transmitancji
	* srednie - okienka z rodzajami sygnalow
	* duze - okienka z wyjsciami
	*/
	wymiaryokienek male;
	wymiaryokienek srednie;
	wymiaryokienek duze;

	male.szerokosc = dlugosc_ulamka / 14;
	male.wysokosc = duza_wysokosc / 7;

	srednie.szerokosc = duza_szerokosc * 2 / 7;
	srednie.wysokosc = duza_wysokosc / 3;

	duze.szerokosc = duza_szerokosc;
	duze.wysokosc = duza_wysokosc;



	/* PRZYPISANIE ID:
	* 0 - prostokat
	* 1 - trojkat
	* 2 - sinus
	* 3 - a3
	* 4 - a2
	* 5 - a1
	* 6 - a0
	* 7 - b4
	* 8 - b3
	* 9 - b2
	* 10 - b1
	* 11 - b0
	* 12 - wybor poczatkowej czestotliwosci dla bode
	* 13 - krok obliczeniowy
	* 14 - amplituda
	* 15 - drugi parametr sygnalu
	*/
	wazneobszary przyciski[L_PRZYCISKOW];

	for (int i = 0; i < L_PRZYCISKOW; i++) {
		if (i < 3) {
			przyciski[i].szerokosc = srednie.szerokosc;
			przyciski[i].wysokosc = srednie.wysokosc;
		}
		else if (i < 12) {
			przyciski[i].szerokosc = male.szerokosc;
			przyciski[i].wysokosc = male.wysokosc;
			if (i < 7) {
				przyciski[i].pole = &a[6 - i];
			}
			else {
				przyciski[i].pole = &b[11 - i];
			}
		}
		else if (i == 12) {
			break;
		}
	}

	// PRZYCISK SYGNAL PROSTOKATNY
	przyciski[0].x = odleglosc_miedzy_duzymi_prostokatami;
	przyciski[0].y = odleglosc_duzych_prostokatow_od_sufitu * 4 + duza_wysokosc;
	// PRZYCISK SYGNAL TROJKATNY
	przyciski[1].x = odleglosc_miedzy_duzymi_prostokatami + duza_szerokosc * 5 / 14;
	przyciski[1].y = przyciski[0].y;
	// PRZYCISK SYGNAL SINUSOIDALNY
	przyciski[2].x = odleglosc_miedzy_duzymi_prostokatami + duza_szerokosc * 10 / 14;
	przyciski[2].y = przyciski[0].y;
	// POLE a3
	przyciski[3].x = odleglosc_miedzy_duzymi_prostokatami * 6 + duza_szerokosc;
	przyciski[3].y = duza_wysokosc * 2 + odleglosc_duzych_prostokatow_od_sufitu * 0.9 - duza_wysokosc / 4.5;
	// POLE a2
	przyciski[4].x = odleglosc_miedzy_duzymi_prostokatami * 9.33 + duza_szerokosc;
	przyciski[4].y = przyciski[3].y;
	// POLE a1
	przyciski[5].x = odleglosc_miedzy_duzymi_prostokatami * 12.66 + duza_szerokosc;
	przyciski[5].y = przyciski[3].y;
	// POLE a0
	przyciski[6].x = odleglosc_miedzy_duzymi_prostokatami * 16 + duza_szerokosc;
	przyciski[6].y = przyciski[3].y;
	// POLE b4
	przyciski[7].x = odleglosc_miedzy_duzymi_prostokatami * 5 + duza_szerokosc;
	przyciski[7].y = duza_wysokosc * 2 + odleglosc_duzych_prostokatow_od_sufitu * 0.9 + 7 * grubosc;
	// POLE b3
	przyciski[8].x = odleglosc_miedzy_duzymi_prostokatami * 5 + duza_szerokosc + dlugosc_ulamka / 5;
	przyciski[8].y = przyciski[7].y;
	// POLE b2
	przyciski[9].x = odleglosc_miedzy_duzymi_prostokatami * 5 + duza_szerokosc + dlugosc_ulamka * 2 / 5;
	przyciski[9].y = przyciski[7].y;
	// POLE b1
	przyciski[10].x = odleglosc_miedzy_duzymi_prostokatami * 5 + duza_szerokosc + dlugosc_ulamka * 3 / 5;
	przyciski[10].y = przyciski[7].y;
	// POLE b0
	przyciski[11].x = odleglosc_miedzy_duzymi_prostokatami * 5 + duza_szerokosc + dlugosc_ulamka * 4 / 5;
	przyciski[11].y = przyciski[7].y;

	//POLE WYBORU POCZATKOWEJ CZESTOTLIWOSCI
	przyciski[12].szerokosc = male.szerokosc;
	przyciski[12].wysokosc = male.wysokosc;
	przyciski[12].pole = &czestotliwos_poczatkowa;
	przyciski[12].x = odleglosc_miedzy_duzymi_prostokatami * 3 + duza_szerokosc * 1.85;
	przyciski[12].y = odleglosc_duzych_prostokatow_od_sufitu + duze.wysokosc + 20;

	//POLE PODSTAWY CZASU ODPOWIEDZI
	przyciski[13].szerokosc = male.szerokosc;
	przyciski[13].wysokosc = male.wysokosc;
	przyciski[13].pole = &podstawa_czasu;
	przyciski[13].x = odleglosc_miedzy_duzymi_prostokatami * 2 + duza_szerokosc * 0.85;
	przyciski[13].y = odleglosc_duzych_prostokatow_od_sufitu + duze.wysokosc + 20;

	//POLE AMPLITUDY
	przyciski[14].szerokosc = male.szerokosc;
	przyciski[14].wysokosc = male.wysokosc;
	przyciski[14].pole = &amplituda;
	przyciski[14].x = (przyciski[0].x + przyciski[1].x) / 2;
	przyciski[14].y = przyciski[0].y + srednie.wysokosc * 2;

	//POLE DRUGIEGO PARAMETRU SYGNALU
	przyciski[15].szerokosc = male.szerokosc;
	przyciski[15].wysokosc = male.wysokosc;
	przyciski[15].pole = &drugi_parametr;
	przyciski[15].x = (przyciski[2].x + przyciski[1].x + male.szerokosc) / 2;
	przyciski[15].y = przyciski[0].y + srednie.wysokosc * 2;

	//MIEJSCA RYSOWANIA DUZYCH PROSTOKATOW
	int poczatek_rys_odp, poczatek_rys_amp, poczatek_rys_faz;
	poczatek_rys_odp = odleglosc_miedzy_duzymi_prostokatami;
	poczatek_rys_amp = odleglosc_miedzy_duzymi_prostokatami * 2 + duza_szerokosc;
	poczatek_rys_faz = odleglosc_miedzy_duzymi_prostokatami * 3 + duza_szerokosc * 2;

	//SKALE WYKRESOW OD GORY DO DOLU, OD LEWEJ DO PRAWEJ
	sf::Text os_y_odp[3];
	sf::Text os_x_odp[2];
	sf::Text os_y_amp[3];
	sf::Text os_x_amp[2];
	sf::Text os_y_faz[3];
	sf::Text os_x_faz[2];
	sf::Text* wsk_osie[15];
	wsk_osie[0] = &os_y_odp[0];
	wsk_osie[1] = &os_y_odp[1];
	wsk_osie[2] = &os_y_odp[2];
	wsk_osie[3] = &os_x_odp[0];
	wsk_osie[4] = &os_x_odp[1];
	wsk_osie[5] = &os_y_amp[0];
	wsk_osie[6] = &os_y_amp[1];
	wsk_osie[7] = &os_y_amp[2];
	wsk_osie[8] = &os_x_amp[0];
	wsk_osie[9] = &os_x_amp[1];
	wsk_osie[10] = &os_y_faz[0];
	wsk_osie[11] = &os_y_faz[1];
	wsk_osie[12] = &os_y_faz[2];
	wsk_osie[13] = &os_x_faz[0];
	wsk_osie[14] = &os_x_faz[1];

	for (int i = 0; i < 15; i++) {
		(*wsk_osie[i]).setFont(czcionka);
		(*wsk_osie[i]).setCharacterSize(rozmiar_czcionki * 0.8);
		(*wsk_osie[i]).setFillColor(sf::Color(0, 0, 0));

	}

	for (int i = 0; i < 3; i++) {
		os_y_odp[i].setPosition(poczatek_rys_odp - 30, odleglosc_duzych_prostokatow_od_sufitu - 7 + static_cast<float>(i) * duza_wysokosc / 2);
		os_y_amp[i].setPosition(poczatek_rys_amp - 30, odleglosc_duzych_prostokatow_od_sufitu - 7 + static_cast<float>(i) * duza_wysokosc / 2);
		os_y_faz[i].setPosition(poczatek_rys_faz - 30, odleglosc_duzych_prostokatow_od_sufitu - 7 + static_cast<float>(i) * duza_wysokosc / 2);
		//cout << endl <<static_cast<float>(i) * duza_wysokosc / 2 << endl;
	}

	for (int i = 0; i < 2; i++) {
		os_x_odp[i].setPosition(poczatek_rys_odp - 5 + static_cast<float>(i) * duza_szerokosc - static_cast<float>(i) * 10, odleglosc_duzych_prostokatow_od_sufitu + duza_wysokosc);
		os_x_amp[i].setPosition(poczatek_rys_amp - 5 + static_cast<float>(i) * duza_szerokosc - static_cast<float>(i) * 10, odleglosc_duzych_prostokatow_od_sufitu + duza_wysokosc);
		os_x_faz[i].setPosition(poczatek_rys_faz - 5 + static_cast<float>(i) * duza_szerokosc - static_cast<float>(i) * 10, odleglosc_duzych_prostokatow_od_sufitu + duza_wysokosc);
	}

	os_y_odp[1].setPosition(poczatek_rys_odp - 13, odleglosc_duzych_prostokatow_od_sufitu - 7 + duza_wysokosc / 2);
	os_y_amp[1].setPosition(poczatek_rys_amp - 13, odleglosc_duzych_prostokatow_od_sufitu - 7 + duza_wysokosc / 2);
	os_y_faz[1].setPosition(poczatek_rys_faz - 13, odleglosc_duzych_prostokatow_od_sufitu - 7 + duza_wysokosc / 2);
	os_y_odp[1].setString("0");
	os_y_amp[0].setString("100");
	os_y_amp[1].setString("0");
	os_y_amp[2].setString("-100");
	os_y_faz[0].setString("180");
	os_y_faz[1].setString("0");
	os_y_faz[2].setString("-180");
	os_x_odp[0].setString("0");



	sf::Text opis_wykres_1;
	sf::Text opis_wykres_2;
	sf::Text opis_wykres_3;
	sf::Text transmitancja;
	sf::Text sygnal;
	sf::Text czy_jest_stabilnosc;
	sf::Text poczatkowa_czestotliwosc_txt;
	sf::Text s_a_3txt;
	sf::Text s_a_2txt;
	sf::Text s_a_1txt;
	sf::Text s_a_0txt;
	sf::Text s_b_4txt;
	sf::Text s_b_3txt;
	sf::Text s_b_2txt;
	sf::Text s_b_1txt;
	sf::Text s_b_0txt;
	sf::Text p_a_3txt;
	sf::Text p_a_2txt;
	sf::Text p_a_1txt;
	sf::Text p_a_0txt;
	sf::Text p_b_4txt;
	sf::Text p_b_3txt;
	sf::Text p_b_2txt;
	sf::Text p_b_1txt;
	sf::Text p_b_0txt;
	sf::Text amplituda_txt;
	sf::Text drugi_parametr_txt_1;
	sf::Text drugi_parametr_txt_2;
	sf::Text drugi_parametr_txt_3;
	sf::RenderWindow window(sf::VideoMode(szerokosc_pliku, wysokosc_pliku), "Projekt_MMM_BAK_GOZDZIELEWSKI");
	sf::RectangleShape ulamek(sf::Vector2f(dlugosc_ulamka, grubosc * 2));
	sf::RectangleShape prostokat_odp(sf::Vector2f(duze.szerokosc, duze.wysokosc));
	sf::RectangleShape prostokat_bode_amp(sf::Vector2f(duze.szerokosc, duze.wysokosc));
	sf::RectangleShape prostokat_bode_faza(sf::Vector2f(duze.szerokosc, duze.wysokosc));
	sf::RectangleShape prostokat_a3(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_a2(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_a1(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_a0(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_b4(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_b3(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_b2(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_b1(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_b0(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_pobudzenie_prostokat(sf::Vector2f(srednie.szerokosc, srednie.wysokosc));
	sf::RectangleShape prostokat_pobudzenie_trojkat(sf::Vector2f(srednie.szerokosc, srednie.wysokosc));
	sf::RectangleShape prostokat_pobudzenie_sinus(sf::Vector2f(srednie.szerokosc, srednie.wysokosc));
	sf::RectangleShape prostokat_poczatkowa_czestotliwosc_bode(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_podstawa_czasu(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_amplituda(sf::Vector2f(male.szerokosc, male.wysokosc));
	sf::RectangleShape prostokat_drugi_parametr(sf::Vector2f(male.szerokosc, male.wysokosc));




	prostokat_odp.setFillColor(sf::Color::White);
	prostokat_odp.setOutlineThickness(grubosc);
	prostokat_odp.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_odp.setPosition(poczatek_rys_odp, odleglosc_duzych_prostokatow_od_sufitu);

	prostokat_bode_amp.setFillColor(sf::Color::White);
	prostokat_bode_amp.setOutlineThickness(grubosc);
	prostokat_bode_amp.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_bode_amp.setPosition(poczatek_rys_amp, odleglosc_duzych_prostokatow_od_sufitu);

	prostokat_bode_faza.setFillColor(sf::Color::White);
	prostokat_bode_faza.setOutlineThickness(grubosc);
	prostokat_bode_faza.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_bode_faza.setPosition(poczatek_rys_faz, odleglosc_duzych_prostokatow_od_sufitu);

	prostokat_poczatkowa_czestotliwosc_bode.setFillColor(sf::Color::White);
	prostokat_poczatkowa_czestotliwosc_bode.setOutlineThickness(grubosc);
	prostokat_poczatkowa_czestotliwosc_bode.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_poczatkowa_czestotliwosc_bode.setPosition(przyciski[12].x, przyciski[12].y);

	prostokat_podstawa_czasu.setFillColor(sf::Color::White);
	prostokat_podstawa_czasu.setOutlineThickness(grubosc);
	prostokat_podstawa_czasu.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_podstawa_czasu.setPosition(przyciski[13].x, przyciski[13].y);

	prostokat_amplituda.setFillColor(sf::Color::White);
	prostokat_amplituda.setOutlineThickness(grubosc);
	prostokat_amplituda.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_amplituda.setPosition(przyciski[14].x, przyciski[14].y);

	prostokat_drugi_parametr.setFillColor(sf::Color::White);
	prostokat_drugi_parametr.setOutlineThickness(grubosc);
	prostokat_drugi_parametr.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_drugi_parametr.setPosition(przyciski[15].x, przyciski[15].y);

	prostokat_pobudzenie_prostokat.setFillColor(sf::Color::White);
	prostokat_pobudzenie_prostokat.setOutlineThickness(grubosc);
	prostokat_pobudzenie_prostokat.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_pobudzenie_prostokat.setPosition(przyciski[0].x, przyciski[0].y);

	prostokat_pobudzenie_trojkat.setFillColor(sf::Color::White);
	prostokat_pobudzenie_trojkat.setOutlineThickness(grubosc);
	prostokat_pobudzenie_trojkat.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_pobudzenie_trojkat.setPosition(przyciski[1].x, przyciski[1].y);

	prostokat_pobudzenie_sinus.setFillColor(sf::Color::White);
	prostokat_pobudzenie_sinus.setOutlineThickness(grubosc);
	prostokat_pobudzenie_sinus.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_pobudzenie_sinus.setPosition(przyciski[2].x, przyciski[2].y);

	//obrazki sygnalow
	sf::Texture prostokat_texture;
	if (!prostokat_texture.loadFromFile("prostokat.png"))
	{
		std::cout << "Error loading 'prostokat.png' !";
	}

	sf::Sprite prostokat_sprite;
	prostokat_sprite.setTexture(prostokat_texture);
	prostokat_sprite.setPosition(przyciski[0].x, przyciski[0].y);

	sf::Texture trojkat_texture;
	if (!trojkat_texture.loadFromFile("trojkat.png"))
	{
		std::cout << "Error loading 'trojkat.png' !";
	}

	sf::Sprite trojkat_sprite;
	trojkat_sprite.setTexture(trojkat_texture);
	trojkat_sprite.setPosition(przyciski[1].x, przyciski[1].y);

	sf::Texture sinus_texture;
	if (!sinus_texture.loadFromFile("sinus.png"))
	{
		std::cout << "Error loading 'sinus.png' !";
	}

	sf::Sprite sinus_sprite;
	sinus_sprite.setTexture(sinus_texture);
	sinus_sprite.setPosition(przyciski[2].x, przyciski[2].y);

	sf::Texture siatkalog_texture;
	if (!siatkalog_texture.loadFromFile("siatkalog.png"))
	{
		std::cout << "Error loading 'siatkalog.png' !";
	}

	sf::Sprite siatkalog_sprite1;
	siatkalog_sprite1.setTexture(siatkalog_texture);
	siatkalog_sprite1.setPosition(poczatek_rys_amp, odleglosc_duzych_prostokatow_od_sufitu);

	sf::Sprite siatkalog_sprite2;
	siatkalog_sprite2.setTexture(siatkalog_texture);
	siatkalog_sprite2.setPosition(poczatek_rys_faz, odleglosc_duzych_prostokatow_od_sufitu);



	prostokat_a3.setFillColor(sf::Color::White);
	prostokat_a3.setOutlineThickness(grubosc);
	prostokat_a3.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_a3.setPosition(przyciski[3].x, przyciski[3].y);


	prostokat_a2.setFillColor(sf::Color::White);
	prostokat_a2.setOutlineThickness(grubosc);
	prostokat_a2.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_a2.setPosition(przyciski[4].x, przyciski[4].y);


	prostokat_a1.setFillColor(sf::Color::White);
	prostokat_a1.setOutlineThickness(grubosc);
	prostokat_a1.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_a1.setPosition(przyciski[5].x, przyciski[5].y);


	prostokat_a0.setFillColor(sf::Color::White);
	prostokat_a0.setOutlineThickness(grubosc);
	prostokat_a0.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_a0.setPosition(przyciski[6].x, przyciski[6].y);


	prostokat_b4.setFillColor(sf::Color::White);
	prostokat_b4.setOutlineThickness(grubosc);
	prostokat_b4.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_b4.setPosition(przyciski[7].x, przyciski[7].y);


	prostokat_b3.setFillColor(sf::Color::White);
	prostokat_b3.setOutlineThickness(grubosc);
	prostokat_b3.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_b3.setPosition(przyciski[8].x, przyciski[8].y);


	prostokat_b2.setFillColor(sf::Color::White);
	prostokat_b2.setOutlineThickness(grubosc);
	prostokat_b2.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_b2.setPosition(przyciski[9].x, przyciski[9].y);


	prostokat_b1.setFillColor(sf::Color::White);
	prostokat_b1.setOutlineThickness(grubosc);
	prostokat_b1.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_b1.setPosition(przyciski[10].x, przyciski[10].y);


	prostokat_b0.setFillColor(sf::Color::White);
	prostokat_b0.setOutlineThickness(grubosc);
	prostokat_b0.setOutlineColor(sf::Color(0, 0, 0));
	prostokat_b0.setPosition(przyciski[11].x, przyciski[11].y);

	ulamek.setFillColor(sf::Color(0, 0, 0));
	ulamek.setOutlineThickness(grubosc);
	ulamek.setOutlineColor(sf::Color(0, 0, 0));
	ulamek.setPosition(odleglosc_miedzy_duzymi_prostokatami * 5 + duza_szerokosc, duza_wysokosc * 2 + odleglosc_duzych_prostokatow_od_sufitu * 0.9);

	opis_wykres_1.setFont(czcionka);
	opis_wykres_1.setString("Odpowiedz impulsowa ukladu");
	opis_wykres_1.setCharacterSize(rozmiar_czcionki);
	opis_wykres_1.setFillColor(sf::Color(0, 0, 0));
	opis_wykres_1.setPosition(odleglosc_miedzy_duzymi_prostokatami * 2.4, odleglosc_duzych_prostokatow_od_sufitu + duza_wysokosc + wysokosc_pliku / 28);

	opis_wykres_2.setFont(czcionka);
	opis_wykres_2.setString("Charakterystyka amplitudowa");
	opis_wykres_2.setCharacterSize(rozmiar_czcionki);
	opis_wykres_2.setFillColor(sf::Color(0, 0, 0));
	opis_wykres_2.setPosition(odleglosc_miedzy_duzymi_prostokatami * 3.4 + duza_szerokosc, odleglosc_duzych_prostokatow_od_sufitu + duza_wysokosc + wysokosc_pliku / 28);

	opis_wykres_3.setFont(czcionka);
	opis_wykres_3.setString("Charakterystyka fazowa");
	opis_wykres_3.setCharacterSize(rozmiar_czcionki);
	opis_wykres_3.setFillColor(sf::Color(0, 0, 0));
	opis_wykres_3.setPosition(odleglosc_miedzy_duzymi_prostokatami * 4.8 + duza_szerokosc * 2, odleglosc_duzych_prostokatow_od_sufitu + duza_wysokosc + wysokosc_pliku / 28);

	transmitancja.setFont(czcionka);
	transmitancja.setString("G(s) =");
	transmitancja.setCharacterSize(rozmiar_czcionki * 3);
	transmitancja.setFillColor(sf::Color(0, 0, 0));
	transmitancja.setPosition(odleglosc_miedzy_duzymi_prostokatami * 1.5 + duza_szerokosc, duza_wysokosc * 2);

	sygnal.setFont(czcionka);
	sygnal.setString("Rodzaj sygnalu wejsciowego");
	sygnal.setCharacterSize(rozmiar_czcionki * 1.5);
	sygnal.setFillColor(sf::Color(0, 0, 0));
	sygnal.setPosition(odleglosc_miedzy_duzymi_prostokatami * 2, odleglosc_duzych_prostokatow_od_sufitu * 2.75 + duza_wysokosc);

	czy_jest_stabilnosc.setFont(czcionka);
	czy_jest_stabilnosc.setString("Czy uklad jest stabilny :");
	czy_jest_stabilnosc.setCharacterSize(rozmiar_czcionki * 1.5);
	czy_jest_stabilnosc.setFillColor(sf::Color(0, 0, 0));
	czy_jest_stabilnosc.setPosition(szerokosc_pliku * 0.36, wysokosc_pliku * 0.9);

	poczatkowa_czestotliwosc_txt.setFont(czcionka);
	poczatkowa_czestotliwosc_txt.setString("Czestotliwosc poczatkowa wykresu (potega liczby 10)");
	poczatkowa_czestotliwosc_txt.setCharacterSize(rozmiar_czcionki);
	poczatkowa_czestotliwosc_txt.setFillColor(sf::Color(0, 0, 0));
	poczatkowa_czestotliwosc_txt.setPosition(przyciski[12].x - male.szerokosc * 3.5, przyciski[12].y + male.wysokosc + 5);

	s_a_3txt.setFont(czcionka);
	s_a_3txt.setString("s    +");
	s_a_3txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_a_3txt.setFillColor(sf::Color(0, 0, 0));
	s_a_3txt.setPosition(przyciski[3].x + 60, przyciski[3].y - 5);

	p_a_3txt.setFont(czcionka);
	p_a_3txt.setString("3");
	p_a_3txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_a_3txt.setFillColor(sf::Color(0, 0, 0));
	p_a_3txt.setPosition(przyciski[3].x + 60 + 13, przyciski[3].y - 5);

	s_a_2txt.setFont(czcionka);
	s_a_2txt.setString("s    +");
	s_a_2txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_a_2txt.setFillColor(sf::Color(0, 0, 0));
	s_a_2txt.setPosition(przyciski[4].x + 60, przyciski[4].y - 5);

	p_a_2txt.setFont(czcionka);
	p_a_2txt.setString("2");
	p_a_2txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_a_2txt.setFillColor(sf::Color(0, 0, 0));
	p_a_2txt.setPosition(przyciski[4].x + 60 + 13, przyciski[4].y - 5);

	s_a_1txt.setFont(czcionka);
	s_a_1txt.setString("s    +");
	s_a_1txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_a_1txt.setFillColor(sf::Color(0, 0, 0));
	s_a_1txt.setPosition(przyciski[5].x + 60, przyciski[5].y - 5);

	p_a_1txt.setFont(czcionka);
	p_a_1txt.setString("1");
	p_a_1txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_a_1txt.setFillColor(sf::Color(0, 0, 0));
	p_a_1txt.setPosition(przyciski[5].x + 60 + 13, przyciski[5].y - 5);

	s_a_0txt.setFont(czcionka);
	s_a_0txt.setString("s");
	s_a_0txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_a_0txt.setFillColor(sf::Color(0, 0, 0));
	s_a_0txt.setPosition(przyciski[6].x + 60, przyciski[6].y - 5);

	p_a_0txt.setFont(czcionka);
	p_a_0txt.setString("0");
	p_a_0txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_a_0txt.setFillColor(sf::Color(0, 0, 0));
	p_a_0txt.setPosition(przyciski[6].x + 60 + 13, przyciski[6].y - 5);

	s_b_4txt.setFont(czcionka);
	s_b_4txt.setString("s   +");
	s_b_4txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_b_4txt.setFillColor(sf::Color(0, 0, 0));
	s_b_4txt.setPosition(przyciski[7].x + 60, przyciski[7].y - 5);

	p_b_4txt.setFont(czcionka);
	p_b_4txt.setString("4");
	p_b_4txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_b_4txt.setFillColor(sf::Color(0, 0, 0));
	p_b_4txt.setPosition(przyciski[7].x + 60 + 13, przyciski[7].y - 5);

	s_b_3txt.setFont(czcionka);
	s_b_3txt.setString("s   +");
	s_b_3txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_b_3txt.setFillColor(sf::Color(0, 0, 0));
	s_b_3txt.setPosition(przyciski[8].x + 60, przyciski[8].y - 5);

	p_b_3txt.setFont(czcionka);
	p_b_3txt.setString("3");
	p_b_3txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_b_3txt.setFillColor(sf::Color(0, 0, 0));
	p_b_3txt.setPosition(przyciski[8].x + 60 + 13, przyciski[8].y - 5);

	s_b_2txt.setFont(czcionka);
	s_b_2txt.setString("s   +");
	s_b_2txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_b_2txt.setFillColor(sf::Color(0, 0, 0));
	s_b_2txt.setPosition(przyciski[9].x + 60, przyciski[9].y - 5);

	p_b_2txt.setFont(czcionka);
	p_b_2txt.setString("2");
	p_b_2txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_b_2txt.setFillColor(sf::Color(0, 0, 0));
	p_b_2txt.setPosition(przyciski[9].x + 60 + 13, przyciski[9].y - 5);

	s_b_1txt.setFont(czcionka);
	s_b_1txt.setString("s   +");
	s_b_1txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_b_1txt.setFillColor(sf::Color(0, 0, 0));
	s_b_1txt.setPosition(przyciski[10].x + 60, przyciski[10].y - 5);

	p_b_1txt.setFont(czcionka);
	p_b_1txt.setString("1");
	p_b_1txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_b_1txt.setFillColor(sf::Color(0, 0, 0));
	p_b_1txt.setPosition(przyciski[10].x + 60 + 13, przyciski[10].y - 5);

	s_b_0txt.setFont(czcionka);
	s_b_0txt.setString("s");
	s_b_0txt.setCharacterSize(rozmiar_czcionki * 1.5);
	s_b_0txt.setFillColor(sf::Color(0, 0, 0));
	s_b_0txt.setPosition(przyciski[11].x + 60, przyciski[11].y - 5);

	p_b_0txt.setFont(czcionka);
	p_b_0txt.setString("0");
	p_b_0txt.setCharacterSize(rozmiar_czcionki * 0.8);
	p_b_0txt.setFillColor(sf::Color(0, 0, 0));
	p_b_0txt.setPosition(przyciski[11].x + 60 + 13, przyciski[11].y - 5);

	amplituda_txt.setFont(czcionka);
	amplituda_txt.setString("Amplituda");
	amplituda_txt.setCharacterSize(rozmiar_czcionki * 1.5);
	amplituda_txt.setFillColor(sf::Color(0, 0, 0));
	amplituda_txt.setPosition(przyciski[14].x - 30, przyciski[14].y - 45);

	drugi_parametr_txt_1.setFont(czcionka);
	drugi_parametr_txt_1.setString("Drugi parametr");
	drugi_parametr_txt_1.setCharacterSize(rozmiar_czcionki * 1.5);
	drugi_parametr_txt_1.setFillColor(sf::Color(0, 0, 0));
	drugi_parametr_txt_1.setPosition(przyciski[14].x + 120, przyciski[14].y - 45);

	drugi_parametr_txt_2.setFont(czcionka);
	drugi_parametr_txt_2.setString("dla prostokata - dlugosc trwania impulsu");
	drugi_parametr_txt_2.setCharacterSize(rozmiar_czcionki * 0.8);
	drugi_parametr_txt_2.setFillColor(sf::Color(0, 0, 0));
	drugi_parametr_txt_2.setPosition(przyciski[14].x + 80, przyciski[14].y + 35);

	drugi_parametr_txt_3.setFont(czcionka);
	drugi_parametr_txt_3.setString("dla trojkata i sinusa - okres");
	drugi_parametr_txt_3.setCharacterSize(rozmiar_czcionki * 0.8);
	drugi_parametr_txt_3.setFillColor(sf::Color(0, 0, 0));
	drugi_parametr_txt_3.setPosition(przyciski[14].x + 80, przyciski[14].y + 60);

	sf::Text przyciski_text[L_PRZYCISKOW - 3];

	for (int i = 0; i < L_PRZYCISKOW - 3; i++) {
		przyciski_text[i].setFont(czcionka);
		przyciski_text[i].setFillColor(sf::Color(0, 0, 0));
		przyciski_text[i].setPosition(przyciski[i + 3].x + 3, przyciski[i + 3].y + 3);
		przyciski_text[i].setCharacterSize(rozmiar_czcionki);
	}


	prostokat_pobudzenie_prostokat.setOutlineColor(sf::Color(255, 223, 0));




	sf::RenderTexture ekran;
	ekran.create(szerokosc_pliku, wysokosc_pliku);
	ekran.draw(prostokat_odp);
	ekran.draw(prostokat_bode_amp);
	ekran.draw(prostokat_bode_faza);
	ekran.draw(siatkalog_sprite1);
	ekran.draw(siatkalog_sprite2);
	ekran.draw(prostokat_poczatkowa_czestotliwosc_bode);
	ekran.draw(prostokat_podstawa_czasu);
	ekran.draw(prostokat_amplituda);
	ekran.draw(prostokat_drugi_parametr);
	ekran.draw(prostokat_pobudzenie_prostokat);
	ekran.draw(prostokat_pobudzenie_trojkat);
	ekran.draw(prostokat_pobudzenie_sinus);
	ekran.draw(opis_wykres_1);
	ekran.draw(opis_wykres_2);
	ekran.draw(opis_wykres_3);
	ekran.draw(transmitancja);
	ekran.draw(ulamek);
	ekran.draw(sygnal);
	ekran.draw(czy_jest_stabilnosc);
	ekran.draw(poczatkowa_czestotliwosc_txt);
	ekran.draw(s_a_3txt);
	ekran.draw(s_a_2txt);
	ekran.draw(s_a_1txt);
	ekran.draw(s_a_0txt);
	ekran.draw(s_b_4txt);
	ekran.draw(s_b_3txt);
	ekran.draw(s_b_2txt);
	ekran.draw(s_b_1txt);
	ekran.draw(s_b_0txt);
	ekran.draw(p_a_3txt);
	ekran.draw(p_a_2txt);
	ekran.draw(p_a_1txt);
	ekran.draw(p_a_0txt);
	ekran.draw(p_b_4txt);
	ekran.draw(p_b_3txt);
	ekran.draw(p_b_2txt);
	ekran.draw(p_b_1txt);
	ekran.draw(p_b_0txt);
	ekran.draw(amplituda_txt);
	ekran.draw(drugi_parametr_txt_1);
	ekran.draw(drugi_parametr_txt_2);
	ekran.draw(drugi_parametr_txt_3);
	ekran.draw(prostokat_a3);
	ekran.draw(prostokat_a2);
	ekran.draw(prostokat_a1);
	ekran.draw(prostokat_a0);
	ekran.draw(prostokat_b4);
	ekran.draw(prostokat_b3);
	ekran.draw(prostokat_b2);
	ekran.draw(prostokat_b1);
	ekran.draw(prostokat_b0);
	ekran.draw(prostokat_sprite);
	ekran.draw(trojkat_sprite);
	ekran.draw(sinus_sprite);
	ekran.display();


	sf::RectangleShape rama[3];
	rama[0] = prostokat_pobudzenie_prostokat;
	rama[1] = prostokat_pobudzenie_trojkat;
	rama[2] = prostokat_pobudzenie_sinus;

	rama[0].setFillColor(sf::Color(0, 0, 0, 0));
	rama[1].setFillColor(sf::Color(0, 0, 0, 0));
	rama[2].setFillColor(sf::Color(0, 0, 0, 0));

	sf::VertexArray odpowiedz;

	wykresybode bode;

	sf::Text stabilnosc_txt;
	stabilnosc_txt.setFont(czcionka);
	stabilnosc_txt.setFillColor(sf::Color(0, 0, 0));
	stabilnosc_txt.setPosition(szerokosc_pliku * 0.36 + 280, wysokosc_pliku * 0.9);
	stabilnosc_txt.setCharacterSize(rozmiar_czcionki * 1.5);

	sf::Sprite ekran_s(ekran.getTexture());

	rysuj(window, ekran_s, przyciski_text, rama, odpowiedz, bode, stabilnosc_txt, przyciski, wsk_osie);

	//NAJWYZSZA WARTOSC ODP
	float max;

	while (window.isOpen())
	{

		sf::Event event;

		while (window.pollEvent(event))
		{
			char nowy = NULL;
			float x;
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					klik(event.mouseButton.x, event.mouseButton.y, przyciski, obecny_sygnal, obecne_pole);
					for (int i = 0; i < 3; i++) {
						if (i == obecny_sygnal) {
							rama[i].setOutlineColor(sf::Color(255, 223, 0));
						}
						else {
							rama[i].setOutlineColor(sf::Color(0, 0, 0));
						}
					}
					rysuj(window, ekran_s, przyciski_text, rama, odpowiedz, bode, stabilnosc_txt, przyciski, wsk_osie);
				}
			}

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.scancode == sf::Keyboard::Scan::Enter) {
					zatwierdz(przyciski);
					switch (obecny_sygnal) {
					case 0:
						odpowiedz = prostokat(a, b, duze, podstawa_czasu.f, amplituda.f, drugi_parametr.f, max);
						break;
					case 1:
						odpowiedz = trojkat(a, b, duze, podstawa_czasu.f, amplituda.f, drugi_parametr.f, max);
						break;

					case 2:
						odpowiedz = sinusoida(a, b, duze, podstawa_czasu.f, amplituda.f, drugi_parametr.f, max);
						break;
					}
					//cout << podstawa_czasu.c << endl;
					bode = bodefun(a, b, duze, czestotliwos_poczatkowa);
					if (stabilnosc(b)) {
						stabilnosc_txt.setString("TAK");
					}
					else {
						stabilnosc_txt.setString("NIE");
					}
					os_x_amp[0].setString(to_string(static_cast<int>(czestotliwos_poczatkowa.f)));
					os_x_amp[1].setString(to_string(static_cast<int>(czestotliwos_poczatkowa.f) + 4));
					os_x_faz[0].setString(to_string(static_cast<int>(czestotliwos_poczatkowa.f)));
					os_x_faz[1].setString(to_string(static_cast<int>(czestotliwos_poczatkowa.f) + 4));
					uzupelnijosie(max, os_y_odp[0], os_y_odp[2], os_x_odp[1], podstawa_czasu.f);
					rysuj(window, ekran_s, przyciski_text, rama, odpowiedz, bode, stabilnosc_txt, przyciski, wsk_osie);
				}
				if (event.key.scancode == sf::Keyboard::Scan::Backspace) {
					usun(obecne_pole);
					rysuj(window, ekran_s, przyciski_text, rama, odpowiedz, bode, stabilnosc_txt, przyciski, wsk_osie);
				}
			}

			if (event.type == sf::Event::TextEntered)
			{
				nowy = static_cast<char>(event.text.unicode);
			}

			if (nowy != NULL) {
				wpisywanie(nowy, obecne_pole);
				rysuj(window, ekran_s, przyciski_text, rama, odpowiedz, bode, stabilnosc_txt, przyciski, wsk_osie);
			}

			if (event.type == sf::Event::Closed)
				window.close();
		}

	}

	return 0;
}

