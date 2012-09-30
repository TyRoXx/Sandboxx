#include <vector>
#include <algorithm>
#include <iostream>

int main()
{
	std::vector<int> elements = {1, 2, 3, 4, 5};

	//Iteriert �ber den vector und entfernt effizient die gew�nschten Elemente.
	//Effizient hei�t hier, dass der Aufwand f�r das Entfernen pro Element konstant ist. Der naive Ansatz ist �blicherweise ein erase(it) pro Element, was lineare Laufzeit hat.
	elements.erase(
		std::partition(
			elements.begin(),
			elements.end(),
			[](int &element) -> bool
		{
			//Einschr�nkungen: Der vector darf hier nicht in seiner Gr��e ver�ndert
			//werden, weil sonst die Iteratoren ung�ltig w�rden.
			//W�hrend des Iterierens kann sich die Reihenfolge der Elemente �ndern.
		   
			//..
		   
			if (element % 2)
			{
				//false f�r Entfernen
				return false;
			}
		   
			//..
			return true;
		}),
		elements.end());
	
	std::sort(elements.begin(), elements.end());
	for (auto i = elements.begin(); i != elements.end(); ++i)
	{
		std::cout << *i << "\n";
	}
}
