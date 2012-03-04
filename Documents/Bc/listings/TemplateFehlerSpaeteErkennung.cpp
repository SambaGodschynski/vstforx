template <typename T>
void foo() {
	x = 0; // keine Variable x Deklariert
}

int main() {
	// Kommentar entfernen, um Fehler zu erhalten:
	//foo<int>();
	return 0;
}